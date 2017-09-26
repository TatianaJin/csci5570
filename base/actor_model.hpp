#pragma once

#include "base/message.hpp"
#include "base/threadsafe_queue.hpp"

#include <condition_variable>
#include <memory>
#include <thread>
#include <unordered_map>

namespace csci5570 {

class Actor {
 public:
  Actor(uint32_t actor_id) : id_(actor_id) {}

  void Start() {  // start a working thread
    working_thread_ = std::thread([this] { Main(); });
  }

  void Stop() { // return when the working thread finishes
    working_thread_.join();
  }

  ThreadsafeQueue<Message>* GetWorkQueue() { return &work_queue_; }   // getter of work queue

  uint32_t GetId() const { return id_; }                       // getter of actor thread id
 protected:
  virtual void Main() = 0;                                  // where the actor polls events and reacts

  uint32_t id_;
  std::thread working_thread_;
  ThreadsafeQueue<Message> work_queue_;
};

}  // namespace csci5570
