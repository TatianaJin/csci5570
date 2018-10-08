#include <cmath>

#include <iostream>
#include <random>
#include <thread>

#include "gflags/gflags.h"
#include "glog/logging.h"

#include "driver/engine.hpp"
#include "lib/abstract_data_loader.hpp"
#include "lib/labeled_sample.hpp"
#include "lib/parser.hpp"

using namespace csci5570;

using Sample = double;
using DataStore = std::vector<Sample>;

DEFINE_string(config_file, "", "The config file path");
DEFINE_string(input, "", "The hdfs input url");

int main(int argc, char** argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  FLAGS_stderrthreshold = 0;
  FLAGS_colorlogtostderr = true;

  LOG(INFO) << FLAGS_config_file;

  Node node{0, "localhost", 12353};
  Engine engine(node, {node});

  // 1. Start system
  engine.StartEverything();

  // 1.1 Create table
  const auto kTableId = engine.CreateTable<double>(ModelType::SSP, StorageType::Map);  // table 0

  // 1.2 Load data
  engine.Barrier();

  // 2. Start training task
  MLTask task;
  task.SetWorkerAlloc({{0, 3}});  // 3 workers on node 0
  task.SetTables({kTableId});     // Use table 0
  task.SetLambda([kTableId](const Info& info) {
    LOG(INFO) << info.DebugString();

    KVClientTable<double> table = info.CreateKVClientTable<double>(kTableId);

    for (int i = 0; i < 1e8; ++i) {
      std::vector<Key> keys{1};

      std::vector<double> ret;
      table.Get(keys, &ret);
      LOG(INFO) << ret[0];

      std::vector<double> vals{0.5};
      table.Add(keys, vals);

      table.Clock();
    }
  });

  engine.Run(task);

  // 3. Stop
  engine.StopEverything();
  return 0;
}
