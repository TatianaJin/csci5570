#pragma once

#include "base/serialization.hpp"
#include "base/threadsafe_queue.hpp"

#include <condition_variable>
#include <memory>
#include <thread>
#include <unordered_map>

namespace csci5570 {

class Actor {
 public:
  Actor(uint32_t actor_id) : id_(actor_id) {}

  virtual void Start() = 0;                                 // start a working thread
  virtual void Stop() = 0;                                  // return when the working thread finishes
  virtual ThreadsafeQueue<BinStream>* GetWorkQueue() = 0;   // getter of work queue
  virtual uint32_t GetId() const = 0;                       // getter of actor thread id

 protected:
  virtual void Main() = 0;                                  // where the actor polls events and reacts

  uint32_t id_;
  std::thread working_thread_;
  ThreadsafeQueue<BinStream> work_queue_;
};

}  // namespace csci5570
