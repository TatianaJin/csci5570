#pragma once

#include <cinttypes>
#include <vector>

#include "base/abstract_partition_manager.hpp"
#include "base/magic.hpp"
#include "base/third_party/sarray.h"

#include "glog/logging.h"

namespace csci5570 {

class RangePartitionManager : public AbstractPartitionManager {
 public:
  RangePartitionManager(const std::vector<uint32_t>& server_thread_ids, const std::vector<third_party::Range>& ranges)
      : AbstractPartitionManager(server_thread_ids) {}

  void Slice(const Keys& keys, std::vector<std::pair<int, Keys>>* sliced) const override {}

  void Slice(const KVPairs& kvs, std::vector<std::pair<int, KVPairs>>* sliced) const override {}

 private:
  std::vector<third_party::Range> ranges_;
};

}  // namespace csci5570
