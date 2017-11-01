#include "gtest/gtest.h"

#include "glog/logging.h"

#include "driver/simple_id_mapper.hpp"
#include "driver/worker_spec.hpp"

namespace csci5570 {
namespace {

class TestWorkerSpec : public testing::Test {
 public:
  TestWorkerSpec() {}
  ~TestWorkerSpec() {}

 protected:
  void SetUp() {}
  void TearDown() {}
};

TEST_F(TestWorkerSpec, Init) {
  // 3 workers on node 0, 2 workers on node 1.
  // {0, {0,1,2}}, {1, {3,4}}
  WorkerSpec worker_spec({{0, 3}, {1, 2}});
  EXPECT_EQ(worker_spec.HasLocalWorkers(0), true);
  EXPECT_EQ(worker_spec.HasLocalWorkers(1), true);
}

TEST_F(TestWorkerSpec, GetWorker) {
  // 3 workers on node 0, 2 workers on node 1.
  // node_to_workers_ : {0, {0,1,2}}, {1, {3,4}}
  // node_to_threads_ : {0, {100,101,102}}, {1, {1100,1101}}
  WorkerSpec worker_spec({{0, 3}, {1, 2}});
  ASSERT_EQ(worker_spec.GetLocalWorkers(0).size(), 3);
  ASSERT_EQ(worker_spec.GetLocalWorkers(1).size(), 2);
  EXPECT_EQ(worker_spec.GetLocalWorkers(0)[0], 0);
  EXPECT_EQ(worker_spec.GetLocalWorkers(1)[1], 4);

  std::map<uint32_t, std::vector<uint32_t>> node_to_workers_ = worker_spec.GetNodeToWorkers();
  auto it0 = node_to_workers_.find(0);
  auto it1 = node_to_workers_.find(1);
  ASSERT_NE(it0, node_to_workers_.end());
  ASSERT_NE(it1, node_to_workers_.end());

  EXPECT_EQ(it0->second[1], 1);
  EXPECT_EQ(it0->second[2], 2);
  EXPECT_EQ(it1->second[0], 3);
  EXPECT_EQ(it1->second[1], 4);
}

TEST_F(TestWorkerSpec, InsertWorkerIdThreadId) {
  // 3 workers on node 0, 2 workers on node 1.
  // node_to_workers_ : {0, {0,1,2}}, {1, {3,4}}
  // node_to_threads_ : {0, {100,101,102}}, {1, {1100,1101}}
  WorkerSpec worker_spec({{0, 3}, {1, 2}});

  worker_spec.InsertWorkerIdThreadId(0, SimpleIdMapper::kMaxBgThreadsPerNode);
  worker_spec.InsertWorkerIdThreadId(1, SimpleIdMapper::kMaxBgThreadsPerNode + 1);
  worker_spec.InsertWorkerIdThreadId(2, SimpleIdMapper::kMaxBgThreadsPerNode + 2);
  worker_spec.InsertWorkerIdThreadId(3, SimpleIdMapper::kMaxThreadsPerNode + SimpleIdMapper::kMaxBgThreadsPerNode);
  worker_spec.InsertWorkerIdThreadId(4, SimpleIdMapper::kMaxThreadsPerNode + SimpleIdMapper::kMaxBgThreadsPerNode + 1);

  EXPECT_EQ(worker_spec.GetLocalThreads(0).size(), 3);
  EXPECT_EQ(worker_spec.GetLocalThreads(1).size(), 2);

  EXPECT_EQ(worker_spec.GetLocalThreads(0)[0], SimpleIdMapper::kMaxBgThreadsPerNode);
  EXPECT_EQ(worker_spec.GetLocalThreads(0)[2], SimpleIdMapper::kMaxBgThreadsPerNode + 2);
  EXPECT_EQ(worker_spec.GetLocalThreads(1)[0],
            SimpleIdMapper::kMaxThreadsPerNode + SimpleIdMapper::kMaxBgThreadsPerNode);
  EXPECT_EQ(worker_spec.GetLocalThreads(1)[1],
            SimpleIdMapper::kMaxThreadsPerNode + SimpleIdMapper::kMaxBgThreadsPerNode + 1);

  std::vector<uint32_t> thread_ids_ = worker_spec.GetAllThreadIds();
  EXPECT_EQ(thread_ids_[0], SimpleIdMapper::kMaxBgThreadsPerNode);
  EXPECT_EQ(thread_ids_[1], SimpleIdMapper::kMaxBgThreadsPerNode + 1);
  EXPECT_EQ(thread_ids_[2], SimpleIdMapper::kMaxBgThreadsPerNode + 2);
  EXPECT_EQ(thread_ids_[3], SimpleIdMapper::kMaxThreadsPerNode + SimpleIdMapper::kMaxBgThreadsPerNode);
  EXPECT_EQ(thread_ids_[4], SimpleIdMapper::kMaxThreadsPerNode + SimpleIdMapper::kMaxBgThreadsPerNode + 1);
}

}  // namespace
}  // namespace csci5570
