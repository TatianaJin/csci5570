#include "driver/simple_id_mapper.hpp"

#include <cinttypes>
#include <vector>

#include "base/node.hpp"

namespace csci5570 {

SimpleIdMapper::SimpleIdMapper(Node node, const std::vector<Node>& nodes) {
  // TODO
}

uint32_t SimpleIdMapper::GetNodeIdForThread(uint32_t tid) {
  // TODO
}

void SimpleIdMapper::Init(int num_server_threads_per_node) {
  // TODO
}

uint32_t SimpleIdMapper::AllocateWorkerThread(uint32_t node_id) {
  // TODO
}
void SimpleIdMapper::DeallocateWorkerThread(uint32_t node_id, uint32_t tid) {
  // TODO
}

std::vector<uint32_t> SimpleIdMapper::GetServerThreadsForId(uint32_t node_id) {
  // TODO
}
std::vector<uint32_t> SimpleIdMapper::GetWorkerHelperThreadsForId(uint32_t node_id) {
  // TODO
}
std::vector<uint32_t> SimpleIdMapper::GetWorkerThreadsForId(uint32_t node_id) {
  // TODO
}
std::vector<uint32_t> SimpleIdMapper::GetAllServerThreads() {
  // TODO
}

const uint32_t SimpleIdMapper::kMaxNodeId;
const uint32_t SimpleIdMapper::kMaxThreadsPerNode;
const uint32_t SimpleIdMapper::kMaxBgThreadsPerNode;
const uint32_t SimpleIdMapper::kWorkerHelperThreadId;

}  // namespace csci5570
