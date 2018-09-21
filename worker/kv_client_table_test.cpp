#include "glog/logging.h"
#include "gtest/gtest.h"

#include "base/abstract_partition_manager.hpp"
#include "base/magic.hpp"
#include "base/message.hpp"
#include "base/third_party/sarray.h"
#include "base/threadsafe_queue.hpp"
#include "worker/kv_client_table.hpp"

#include <condition_variable>
#include <mutex>
#include <thread>

namespace csci5570 {

const uint32_t kTestAppThreadId = 15;
const uint32_t kTestModelId = 23;

class FakePartitionManager : public AbstractPartitionManager {
 public:
  FakePartitionManager(const std::vector<uint32_t>& server_thread_ids, int split)
      : AbstractPartitionManager(server_thread_ids), split_(split) {}

  void Slice(const Keys& keys, std::vector<std::pair<int, Keys>>* sliced) const override {
    size_t n = keys.size();
    sliced->resize(2);
    auto pos = std::lower_bound(keys.begin(), keys.end(), split_) - keys.begin();
    sliced->at(0).first = 0;
    sliced->at(0).second = keys.segment(0, pos);
    sliced->at(1).first = 1;
    sliced->at(1).second = keys.segment(pos, n);
  }

  void Slice(const KVPairs& kvs, std::vector<std::pair<int, KVPairs>>* sliced) const override {
    size_t n = kvs.first.size();
    sliced->resize(2);
    auto pos = std::lower_bound(kvs.first.begin(), kvs.first.end(), split_) - kvs.first.begin();
    sliced->at(0).first = server_thread_ids_[0];
    sliced->at(0).second.first = kvs.first.segment(0, pos);
    sliced->at(0).second.second = kvs.second.segment(0, pos);
    sliced->at(1).first = server_thread_ids_[1];
    sliced->at(1).second.first = kvs.first.segment(pos, n);
    sliced->at(1).second.second = kvs.second.segment(pos, n);
  }

 private:
  int split_ = 0;
};

class FakeCallbackRunner : public AbstractCallbackRunner {
 public:
  FakeCallbackRunner() {}
  void RegisterRecvHandle(uint32_t app_thread_id, uint32_t model_id,
                          const std::function<void(Message&)>& recv_handle) override {
    EXPECT_EQ(app_thread_id, kTestAppThreadId);
    EXPECT_EQ(model_id, kTestModelId);
    recv_handle_ = recv_handle;
  }
  void RegisterRecvFinishHandle(uint32_t app_thread_id, uint32_t model_id,
                                const std::function<void()>& recv_finish_handle) override {
    EXPECT_EQ(app_thread_id, kTestAppThreadId);
    EXPECT_EQ(model_id, kTestModelId);
    recv_finish_handle_ = recv_finish_handle;
  }

  void NewRequest(uint32_t app_thread_id, uint32_t model_id, uint32_t expected_responses) override {
    EXPECT_EQ(app_thread_id, kTestAppThreadId);
    EXPECT_EQ(model_id, kTestModelId);
    tracker_ = {expected_responses, 0};
  }
  void WaitRequest(uint32_t app_thread_id, uint32_t model_id) override {
    EXPECT_EQ(app_thread_id, kTestAppThreadId);
    EXPECT_EQ(model_id, kTestModelId);
    std::unique_lock<std::mutex> lk(mu_);
    cond_.wait(lk, [this] { return tracker_.first == tracker_.second; });
  }
  void AddResponse(uint32_t app_thread_id, uint32_t model_id, Message& m) override {
    EXPECT_EQ(app_thread_id, kTestAppThreadId);
    EXPECT_EQ(model_id, kTestModelId);
    EXPECT_NE(recv_handle_, nullptr);
    bool recv_finish = false;
    {
      std::lock_guard<std::mutex> lk(mu_);
      recv_finish = tracker_.first == tracker_.second + 1 ? true : false;
    }
    recv_handle_(m);
    if (recv_finish) {
      recv_finish_handle_();
    }
    {
      std::lock_guard<std::mutex> lk(mu_);
      tracker_.second += 1;
      if (recv_finish) {
        cond_.notify_all();
      }
    }
  }

 private:
  std::function<void(Message&)> recv_handle_;
  std::function<void()> recv_finish_handle_;

  std::mutex mu_;
  std::condition_variable cond_;
  std::pair<uint32_t, uint32_t> tracker_;
};

class TestKVClientTable : public testing::Test {
 protected:
  void SetUp() {}
  void TearDown() {}

};  // class TestKVClientTable

TEST_F(TestKVClientTable, Init) {
  ThreadsafeQueue<Message> queue;
  FakePartitionManager manager({0, 1}, 4);
  FakeCallbackRunner callback_runner;
  KVClientTable<double> table(kTestAppThreadId, kTestModelId, &queue, &manager, &callback_runner);
}

TEST_F(TestKVClientTable, Add) {
  ThreadsafeQueue<Message> queue;
  FakePartitionManager manager({0, 1}, 4);
  FakeCallbackRunner callback_runner;
  KVClientTable<double> table(kTestAppThreadId, kTestModelId, &queue, &manager, &callback_runner);

  std::vector<Key> keys = {3, 4, 5, 6};
  std::vector<double> vals = {0.1, 0.1, 0.1, 0.1};
  table.Add(keys, vals);  // {3,4,5,6} -> {3}, {4,5,6}
  Message m1, m2;
  queue.WaitAndPop(&m1);
  queue.WaitAndPop(&m2);

  EXPECT_EQ(m1.meta.sender, kTestAppThreadId);  // check the sender id
  EXPECT_EQ(m1.meta.recver, 0);                 // to server 0
  EXPECT_EQ(m1.meta.model_id, kTestModelId);    // check the model id
  EXPECT_EQ(m1.meta.flag, Flag::kAdd);          // check the command flag
  ASSERT_EQ(m1.data.size(), 2);                 // the message should contain both keys and vals
  third_party::SArray<Key> res_keys;
  res_keys = m1.data[0];
  ASSERT_EQ(res_keys.size(), 1);  // key 3
  EXPECT_EQ(res_keys[0], 3);
  third_party::SArray<double> res_vals;
  res_vals = m1.data[1];
  ASSERT_EQ(res_vals.size(), 1);  // val .1
  EXPECT_DOUBLE_EQ(res_vals[0], double(0.1));

  EXPECT_EQ(m2.meta.sender, kTestAppThreadId);
  EXPECT_EQ(m2.meta.recver, 1);
  EXPECT_EQ(m2.meta.model_id, kTestModelId);
  EXPECT_EQ(m2.meta.flag, Flag::kAdd);
  ASSERT_EQ(m2.data.size(), 2);
  res_keys = m2.data[0];
  ASSERT_EQ(res_keys.size(), 3);
  res_vals = m2.data[1];
  ASSERT_EQ(res_vals.size(), 3);
  EXPECT_EQ(res_keys[0], 4);
  EXPECT_EQ(res_keys[1], 5);
  EXPECT_EQ(res_keys[2], 6);
  EXPECT_DOUBLE_EQ(res_vals[0], double(0.1));
  EXPECT_DOUBLE_EQ(res_vals[1], double(0.1));
  EXPECT_DOUBLE_EQ(res_vals[2], double(0.1));
}

TEST_F(TestKVClientTable, Get) {
  ThreadsafeQueue<Message> queue;
  FakePartitionManager manager({0, 1}, 4);
  FakeCallbackRunner callback_runner;
  std::thread th([&queue, &manager, &callback_runner]() {
    KVClientTable<double> table(kTestAppThreadId, kTestModelId, &queue, &manager, &callback_runner);
    std::vector<Key> keys = {3, 4, 5, 6};
    std::vector<double> vals;
    table.Get(keys, &vals);  // {3,4,5,6} -> {3}, {4,5,6}
    std::vector<double> expected{0.1, 0.4, 0.2, 0.3};
    EXPECT_EQ(vals, expected);
  });
  // Check the requests in queue
  Message m1, m2;
  queue.WaitAndPop(&m1);
  queue.WaitAndPop(&m2);

  EXPECT_EQ(m1.meta.sender, kTestAppThreadId);
  EXPECT_EQ(m1.meta.recver, 0);
  EXPECT_EQ(m1.meta.model_id, kTestModelId);
  EXPECT_EQ(m1.meta.flag, Flag::kGet);
  ASSERT_EQ(m1.data.size(), 1);
  third_party::SArray<Key> res_keys;
  res_keys = m1.data[0];
  ASSERT_EQ(res_keys.size(), 1);
  EXPECT_EQ(res_keys[0], 3);

  EXPECT_EQ(m2.meta.sender, kTestAppThreadId);
  EXPECT_EQ(m2.meta.recver, 1);
  EXPECT_EQ(m2.meta.model_id, kTestModelId);
  EXPECT_EQ(m2.meta.flag, Flag::kGet);
  ASSERT_EQ(m2.data.size(), 1);
  res_keys = m2.data[0];
  ASSERT_EQ(res_keys.size(), 3);
  EXPECT_EQ(res_keys[0], 4);
  EXPECT_EQ(res_keys[1], 5);
  EXPECT_EQ(res_keys[2], 6);

  // AddResponse
  Message r1, r2;
  third_party::SArray<Key> r1_keys{3};
  third_party::SArray<double> r1_vals{0.1};
  r1.AddData(r1_keys);
  r1.AddData(r1_vals);
  third_party::SArray<Key> r2_keys{4, 5, 6};
  third_party::SArray<double> r2_vals{0.4, 0.2, 0.3};
  r2.AddData(r2_keys);
  r2.AddData(r2_vals);
  callback_runner.AddResponse(kTestAppThreadId, kTestModelId, r1);
  callback_runner.AddResponse(kTestAppThreadId, kTestModelId, r2);
  th.join();
}

}  // namespace csci5570
