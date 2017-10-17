#include <thread>

#include "boost/utility/string_ref.hpp"
#include "glog/logging.h"
#include "gtest/gtest.h"

#include "base/serialization.hpp"
#include "io/coordinator.hpp"
#include "io/hdfs_assigner.hpp"
#include "io/hdfs_file_splitter.hpp"
#include "io/line_input_format.hpp"

namespace csci5570 {

void HDFS_Read() {
  std::string hdfs_namenode = "proj10";
  int hdfs_namenode_port = 9000;
  int master_port = 19817;  // use a random port number to avoid collision with other users
  zmq::context_t zmq_context(1);

  // 1. Spawn the HDFS block assigner thread on the master
  std::thread master_thread([&zmq_context, master_port, hdfs_namenode_port, hdfs_namenode] {
    HDFSBlockAssigner hdfs_block_assigner(hdfs_namenode, hdfs_namenode_port, &zmq_context, master_port);
    hdfs_block_assigner.Serve();
  });

  // 2. Prepare meta info for the master and workers
  int proc_id = getpid();  // the actual process id, or you can assign a virtual one, as long as it is distinct
  std::string master_host = "proj10";  // change to the node you are actually using
  std::string worker_host = "proj10";  // change to the node you are actually using

  // 3. One coordinator for one process
  Coordinator coordinator(proc_id, worker_host, &zmq_context, master_host, master_port);
  coordinator.serve();
  LOG(INFO) << "Coordinator begins serving";

  // 4. The user thread runing UDF
  std::thread worker_thread([hdfs_namenode_port, hdfs_namenode, &coordinator, worker_host] {
    std::string input = "hdfs:///datasets/classification/a9";
    int num_threads = 1;
    int second_id = 0;
    LineInputFormat infmt(input, num_threads, second_id, &coordinator, worker_host, hdfs_namenode, hdfs_namenode_port);
    LOG(INFO) << "Line input is well prepared";

    // Line counting demo
    // Deserialing logic in UDF/application library
    bool success = true;
    int count = 0;
    boost::string_ref record;
    while (true) {
      success = infmt.next(record);
      if (success == false)
        break;
      ++count;
    }
    LOG(INFO) << "The number of lines in " << input << " is " << count;

    // Remember to notify master that the worker wants to exit
    BinStream finish_signal;
    finish_signal << worker_host << second_id;
    coordinator.notify_master(finish_signal, 300);
  });

  // Make sure zmq_context and coordinator live long enough
  master_thread.join();
  worker_thread.join();
}

}  // namespace csci5570

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);
  FLAGS_stderrthreshold = 0;
  FLAGS_colorlogtostderr = true;
  csci5570::HDFS_Read();
}
