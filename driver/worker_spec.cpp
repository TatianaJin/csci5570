#include "driver/worker_spec.hpp"
#include "glog/logging.h"

namespace csci5570 {

WorkerSpec::WorkerSpec(const std::vector<WorkerAlloc>& worker_alloc) {
  // TODO
}
bool WorkerSpec::HasLocalWorkers(uint32_t node_id) const {
  // TODO
}
const std::vector<uint32_t>& WorkerSpec::GetLocalWorkers(uint32_t node_id) const {
  // TODO
}
const std::vector<uint32_t>& WorkerSpec::GetLocalThreads(uint32_t node_id) const {
  // TODO
}

std::map<uint32_t, std::vector<uint32_t>> WorkerSpec::GetNodeToWorkers() {
  // TODO
}

std::vector<uint32_t> WorkerSpec::GetAllThreadIds() {
  // TODO
}

void WorkerSpec::InsertWorkerIdThreadId(uint32_t worker_id, uint32_t thread_id) {
  // TODO
}

void WorkerSpec::Init(const std::vector<WorkerAlloc>& worker_alloc) {
  // TODO
}
}  // namespace csci5570
