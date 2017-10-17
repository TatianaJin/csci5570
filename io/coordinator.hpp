// Copyright 2016 Husky Team
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <mutex>

#include "glog/logging.h"
#include "zmq.hpp"

#include "base/serialization.hpp"
#include "base/zmq_helper.hpp"

namespace csci5570 {

/**
 * HDFS coordinator on the worker side connecting file splitter and master
 * Shared among worker threads in the same process for loading distibuted files
 */
class Coordinator {
 public:
  /**
   * Constructor
   *
   * @param proc_id      the id of the process
   * @param hostname     the hostname of the current process
   * @param context      zmq context for message passing
   * @param master_host  the hostname of HDFS master
   * @param master_port  HDFS master port number
   *
   */
  Coordinator(int proc_id, std::string hostname, zmq::context_t* context, std::string master_host, int master_port);

  ~Coordinator();

  /**
   * Build tcp connection with master
   */
  void serve();

  /**
   * Send messages to master and get replies
   *
   * @param question    serialized data to send to the master
   * @param type        message type
   *
   * @return            serialized response from the master
   */
  BinStream ask_master(BinStream& question, size_t type);

  /**
   * Send notifications to master
   *
   * @param question    serialized data to send to the master
   * @param type        message type
   *
   */
  void notify_master(BinStream& message, size_t type);

 private:
  std::mutex coord_lock_;
  int proc_id_;
  int master_port_;
  std::string hostname_;
  std::string master_host_;
  zmq::context_t* context_;
  zmq::socket_t* zmq_coord_ = nullptr;

};  // class Coordinator

}  // namespace csci5570
