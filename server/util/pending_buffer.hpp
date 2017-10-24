#pragma once

#include "base/message.hpp"

#include <unordered_map>

namespace csci5570 {

class PendingBuffer {
 public:
  /**
   * Return the pending requests at the specific progress clock
   */
  virtual std::vector<Message> Pop(const int clock);
  /**
   * Add the pending requests at the specific progress clock
   */
  virtual void Push(const int clock, Message& message);
  /**
   * Return the number of pending requests at the specific progress
   */
  virtual int Size(const int progress);
};

}  // namespace csci5570
