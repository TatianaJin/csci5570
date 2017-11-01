#pragma once

#include <sstream>

#include "base/abstract_partition_manager.hpp"
#include "base/threadsafe_queue.hpp"
#include "worker/abstract_callback_runner.hpp"
#include "worker/kv_client_table.hpp"

#include "glog/logging.h"

namespace csci5570 {

/**
 * The Info object provides the context of user threads
 */
struct Info {
  uint32_t thread_id;
  uint32_t worker_id;
  ThreadsafeQueue<Message>* send_queue;
  std::map<uint32_t, AbstractPartitionManager*> partition_manager_map;
  AbstractCallbackRunner* callback_runner;
  std::string DebugString() const {
    std::stringstream ss;
    ss << "thread_id: " << thread_id << " worker_id: " << worker_id;
    return ss.str();
  }

  /**
   * The wrapper function (helper) creates a KVClientTable with the Info, so that users do not need to call the
   * KVClientTable constructor with so many arguments.
   *
   * @param table_id    the model id
   */
  template <typename Val>
  KVClientTable<Val> CreateKVClientTable(uint32_t table_id) const {
    // TODO
  }
};

}  // namespace csci5570
