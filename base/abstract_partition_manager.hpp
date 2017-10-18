#pragma once

#include <cinttypes>
#include <vector>

#include "base/magic.hpp"
#include "base/third_party/sarray.h"

namespace csci5570 {

/*
 * Implments the interface of a PartitionManager which provides the model partitioning scheme
 */
class AbstractPartitionManager {
 public:
  // using Keys = std::vector<Key>;
  // using KVPairs = std::pair<std::vector<Key>, std::vector<double>>;
  using Keys = third_party::SArray<Key>;
  using KVPairs = std::pair<third_party::SArray<Key>, third_party::SArray<double>>;

  AbstractPartitionManager(const std::vector<uint32_t>& server_thread_ids) : server_thread_ids_(server_thread_ids) {}

  size_t GetNumServers() const;
  const std::vector<uint32_t>& GetServerThreadIds() const;

  // slice keys into <server_id, key_partition> pairs
  virtual void Slice(const Keys& keys, std::vector<std::pair<int, Keys>>* sliced) const = 0;
  // slice key-value pairs into <server_id, key_value_partition> pairs
  virtual void Slice(const KVPairs& kvs, std::vector<std::pair<int, KVPairs>>* sliced) const = 0;

 protected:
  std::vector<uint32_t> server_thread_ids_;
};  // class AbstractPartitionManager

}  // namespace csci5570
