#include "glog/logging.h"
#include "gtest/gtest.h"

#include "driver/engine.hpp"
#include "worker/kv_client_table.hpp"

namespace csci5570 {
namespace {

/**
 * Test for Engine which depends on
 *  Mailbox
 *  WorkerThread
 *  ServerThread
 *  AbstractCallbackRunner
 *  ...
 *
 *  The failure of the test may be caused by each of these components.
 */
class TestEngine : public testing::Test {
 public:
  TestEngine() {}
  ~TestEngine() {}

 protected:
  void SetUp() {}
  void TearDown() {}
};

class FakeIdMapper : public AbstractIdMapper {
 public:
  virtual uint32_t GetNodeIdForThread(uint32_t tid) override { return tid; }
};

TEST_F(TestEngine, Construct) {
  Node node{0, "localhost", 12353};  // node: id 0, host localhost, port 12353
  Engine engine(node, {node});
}

TEST_F(TestEngine, StartMailbox) {
  Node node{0, "localhost", 12353};
  Engine engine(node, {node});
  engine.CreateIdMapper();
  engine.CreateMailbox();
  engine.StartMailbox();
  engine.StopMailbox();
}

TEST_F(TestEngine, StartEverything) {
  Node node{0, "localhost", 12353};
  Engine engine(node, {node});
  engine.StartEverything();
  engine.StopEverything();
}

TEST_F(TestEngine, MultipleStartEverything) {  // start three engines on the localhost
  std::vector<Node> nodes{{0, "localhost", 12353}, {1, "localhost", 12354}, {2, "localhost", 12355}};

  std::vector<std::thread> threads(nodes.size());
  for (int i = 0; i < nodes.size(); ++i) {
    threads[i] = std::thread([&nodes, i]() {
      Engine engine(nodes[i], nodes);
      engine.StartEverything();
      engine.StopEverything();
    });
  }
  for (auto& th : threads) {
    th.join();
  }
}

TEST_F(TestEngine, SimpleTaskMapStorage) {
  Node node{0, "localhost", 12353};
  Engine engine(node, {node});
  // start
  engine.StartEverything();

  auto table_id = engine.CreateTable<double>(ModelType::SSP, StorageType::Map);  // table 0
  engine.Barrier();
  MLTask task;
  task.SetWorkerAlloc({{0, 3}});  // 3 workers on node 0
  task.SetTables({table_id});     // Use table 0
  task.SetLambda([](const Info& info) { LOG(INFO) << "Hi"; });
  engine.Run(task);

  // stop
  engine.StopEverything();
}

TEST_F(TestEngine, MultipleTasks) {  // simulate multiple instances of engine running a distributed task
  std::vector<Node> nodes{{0, "localhost", 12353}, {1, "localhost", 12354}, {2, "localhost", 12355}};

  std::vector<std::thread> threads(nodes.size());
  for (int i = 0; i < nodes.size(); ++i) {
    threads[i] = std::thread([&nodes, i]() {
      Engine engine(nodes[i], nodes);
      // start
      engine.StartEverything();

      auto table_id = engine.CreateTable<double>(ModelType::SSP, StorageType::Map);
      engine.Barrier();
      MLTask task;
      // 3 workers on node 0, 2 workers on node 1, 3 workers on node 2
      task.SetWorkerAlloc({{0, 3}, {1, 2}, {2, 3}});
      task.SetTables({table_id});  // Use table 0
      task.SetLambda([](const Info& info) { LOG(INFO) << "Hi"; });
      engine.Run(task);

      // stop
      engine.StopEverything();
    });
  }
  for (auto& th : threads) {
    th.join();
  }
}

TEST_F(TestEngine, KVClientTableMapStorage) {
  Node node{0, "localhost", 12353};
  Engine engine(node, {node});
  // start
  engine.StartEverything();

  const auto kTableId = engine.CreateTable<double>(ModelType::SSP, StorageType::Map);  // table 0
  engine.Barrier();
  MLTask task;
  task.SetWorkerAlloc({{0, 3}});  // 3 workers on node 0
  task.SetTables({kTableId});     // Use table 0
  task.SetLambda([kTableId](const Info& info) {
    LOG(INFO) << "Hi";
    LOG(INFO) << info.DebugString();
    ASSERT_TRUE(info.partition_manager_map.find(kTableId) != info.partition_manager_map.end());
    KVClientTable<double> table(info.thread_id, kTableId, info.send_queue,
                                info.partition_manager_map.find(kTableId)->second, info.callback_runner);
    for (int i = 0; i < 5; ++i) {
      std::vector<Key> keys{1};
      std::vector<double> vals{0.5};
      table.Add(keys, vals);
      std::vector<double> ret;
      table.Get(keys, &ret);
      EXPECT_EQ(ret.size(), 1);
      LOG(INFO) << ret[0];
    }
  });
  engine.Run(task);

  // stop
  engine.StopEverything();
}

}  // namespace
}  // namespace csci5570
