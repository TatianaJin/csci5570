#include <memory>

#include "glog/logging.h"
#include "gtest/gtest.h"

#include "base/message.hpp"
#include "base/threadsafe_queue.hpp"
#include "server/abstract_model.hpp"
#include "server/consistency/asp_model.hpp"
#include "server/map_storage.hpp"

namespace csci5570 {
namespace {

class TestASPModel : public testing::Test {
 public:
  TestASPModel() {}
  ~TestASPModel() {}

 protected:
  void SetUp() {}
  void TearDown() {}
};

TEST_F(TestASPModel, CheckConstructor) {
  ThreadsafeQueue<Message> reply_queue;
  int model_id = 0;
  // TODO the test should be independent of the storage implementation
  std::unique_ptr<AbstractStorage> storage(new MapStorage<int>());
  ASPModel model(model_id, std::move(storage), &reply_queue);
}

TEST_F(TestASPModel, CheckGetAndAdd) {
  ThreadsafeQueue<Message> reply_queue;
  int model_id = 0;
  std::unique_ptr<AbstractStorage> storage(new MapStorage<int>());
  std::unique_ptr<AbstractModel> model(new ASPModel(model_id, std::move(storage), &reply_queue));
  Message reset_msg;
  third_party::SArray<uint32_t> tids({2, 3});  // worker thread ids
  reset_msg.AddData(tids);
  model->ResetWorker(reset_msg);  // Test ResetWorker
  Message reset_reply_msg;
  reply_queue.WaitAndPop(&reset_reply_msg);
  EXPECT_EQ(reset_reply_msg.meta.flag, Flag::kResetWorkerInModel);  // Expect reply for ResetWorker

  // for Check use
  Message check_msg;
  auto rep_keys = third_party::SArray<int>();
  auto rep_vals = third_party::SArray<int>();

  Message msg;
  // Get_2_0
  msg.meta.flag = Flag::kGet;
  msg.meta.model_id = 0;
  msg.meta.sender = 2;
  msg.meta.recver = 0;
  msg.AddData(third_party::SArray<int>{0});
  model->Get(msg);
  // Get_3_0
  msg = Message();
  msg.meta.flag = Flag::kGet;
  msg.meta.model_id = 0;
  msg.meta.sender = 3;
  msg.meta.recver = 0;
  msg.AddData(third_party::SArray<int>{1});
  model->Get(msg);

  // Expect reply to thread 2
  reply_queue.WaitAndPop(&check_msg);
  EXPECT_EQ(check_msg.meta.flag, Flag::kGet);
  EXPECT_EQ(check_msg.meta.sender, 0);
  EXPECT_EQ(check_msg.meta.recver, 2);
  ASSERT_EQ(check_msg.data.size(), 2);
  rep_keys = third_party::SArray<int>(check_msg.data[0]);
  rep_vals = third_party::SArray<int>(check_msg.data[1]);
  ASSERT_EQ(rep_keys.size(), 1);
  ASSERT_EQ(rep_vals.size(), 1);
  EXPECT_EQ(rep_keys[0], 0);
  EXPECT_EQ(rep_vals[0], 0);

  // Expect reply to thread 3
  reply_queue.WaitAndPop(&check_msg);
  EXPECT_EQ(check_msg.meta.flag, Flag::kGet);
  EXPECT_EQ(check_msg.meta.sender, 0);
  EXPECT_EQ(check_msg.meta.recver, 3);
  ASSERT_EQ(check_msg.data.size(), 2);
  rep_keys = third_party::SArray<int>(check_msg.data[0]);
  rep_vals = third_party::SArray<int>(check_msg.data[1]);
  ASSERT_EQ(rep_keys.size(), 1);
  ASSERT_EQ(rep_vals.size(), 1);
  EXPECT_EQ(rep_keys[0], 1);
  EXPECT_EQ(rep_vals[0], 0);

  // Get should not be blocked and get/add should be in order
  msg = Message();
  msg.meta.flag = Flag::kGet;
  msg.meta.model_id = 0;
  msg.meta.sender = 3;
  msg.meta.recver = 0;
  msg.AddData(third_party::SArray<int>{1});
  model->Get(msg);

  msg = Message();
  msg.meta.flag = Flag::kAdd;
  msg.meta.model_id = 0;
  msg.meta.sender = 2;
  msg.meta.recver = 0;
  msg.AddData(third_party::SArray<int>{1});  // key
  msg.AddData(third_party::SArray<int>{1});  // value
  model->Add(msg);

  ASSERT_EQ(reply_queue.Size(), 1);
  reply_queue.WaitAndPop(&check_msg);
  EXPECT_EQ(check_msg.meta.flag, Flag::kGet);
  EXPECT_EQ(check_msg.meta.sender, 0);
  EXPECT_EQ(check_msg.meta.recver, 3);
  ASSERT_EQ(check_msg.data.size(), 2);
  rep_keys = third_party::SArray<int>(check_msg.data[0]);
  rep_vals = third_party::SArray<int>(check_msg.data[1]);
  ASSERT_EQ(rep_keys.size(), 1);
  ASSERT_EQ(rep_vals.size(), 1);
  EXPECT_EQ(rep_keys[0], 1);
  EXPECT_EQ(rep_vals[0], 0);

  // Clock and Get
  msg = Message();
  msg.meta.flag = Flag::kGet;
  msg.meta.model_id = 0;
  msg.meta.sender = 2;
  msg.meta.recver = 0;
  msg.AddData(third_party::SArray<int>{1});  // key
  model->Clock(msg);
  model->Get(msg);

  ASSERT_EQ(reply_queue.Size(), 1);
  reply_queue.WaitAndPop(&check_msg);
  EXPECT_EQ(check_msg.meta.flag, Flag::kGet);
  EXPECT_EQ(check_msg.meta.sender, 0);
  EXPECT_EQ(check_msg.meta.recver, 2);
  ASSERT_EQ(check_msg.data.size(), 2);
  rep_keys = third_party::SArray<int>(check_msg.data[0]);
  rep_vals = third_party::SArray<int>(check_msg.data[1]);
  ASSERT_EQ(rep_keys.size(), 1);
  ASSERT_EQ(rep_vals.size(), 1);
  EXPECT_EQ(rep_keys[0], 1);
  EXPECT_EQ(rep_vals[0], 1);

  // Clock and Get
  msg = Message();
  msg.meta.flag = Flag::kAdd;
  msg.meta.model_id = 0;
  msg.meta.sender = 3;
  msg.meta.recver = 0;
  msg.AddData(third_party::SArray<int>{0});  // key
  msg.AddData(third_party::SArray<int>{1});  // value
  model->Add(msg);
  model->Clock(msg);

  msg = Message();
  msg.meta.flag = Flag::kGet;
  msg.meta.model_id = 0;
  msg.meta.sender = 3;
  msg.meta.recver = 0;
  msg.AddData(third_party::SArray<int>{0});  // key
  model->Get(msg);

  ASSERT_EQ(reply_queue.Size(), 1);
  reply_queue.WaitAndPop(&check_msg);
  EXPECT_EQ(check_msg.meta.flag, Flag::kGet);
  EXPECT_EQ(check_msg.meta.sender, 0);
  EXPECT_EQ(check_msg.meta.recver, 3);
  ASSERT_EQ(check_msg.data.size(), 2);
  rep_keys = third_party::SArray<int>(check_msg.data[0]);
  rep_vals = third_party::SArray<int>(check_msg.data[1]);
  ASSERT_EQ(rep_keys.size(), 1);
  ASSERT_EQ(rep_vals.size(), 1);
  EXPECT_EQ(rep_keys[0], 0);
  EXPECT_EQ(rep_vals[0], 1);
}

}  // namespace
}  // namespace csci5570
