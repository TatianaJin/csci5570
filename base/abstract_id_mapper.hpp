#pragma once

#include <cinttypes>

namespace csci5570 {

class AbstractIdMapper {
 public:
  virtual ~AbstractIdMapper() {}
  virtual uint32_t GetNodeIdForThread(uint32_t tid) = 0;
};

}  // namespace csci5570
