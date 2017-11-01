#pragma once

#include "driver/ml_task.hpp"
#include "driver/simple_id_mapper.hpp"

#include <map>
#include <set>
#include <vector>

namespace csci5570 {

/*
 * node_id: 0, 1
 * worker_id for this task: 0, 1, 2 .. consecutive
 * thread_id: global worker thread id
 *
 * worker here refers to the user threads to run UDF
 */
class WorkerSpec {
 public:
  // {{0, 3}, {1, 2}}: 3 workers on node 0, 2 workers on node 1.
  WorkerSpec() = delete;
  WorkerSpec(const std::vector<WorkerAlloc>& worker_alloc);

  /**
   * Check if the local process is allocatd any worker for a task
   */
  bool HasLocalWorkers(uint32_t node_id) const;

  /**
   * Return the ids of worker threads on the local process
   */
  const std::vector<uint32_t>& GetLocalWorkers(uint32_t node_id) const;

  /**
   * Return the ids of threads on the local process
   */
  const std::vector<uint32_t>& GetLocalThreads(uint32_t node_id) const;

  /**
   * Return the ids of threads on the local process
   */
  std::map<uint32_t, std::vector<uint32_t>> GetNodeToWorkers();

  /**
   * Returns all worker threads
   */
  std::vector<uint32_t> GetAllThreadIds();

  /**
   * Register worker id (specific to a task) along with the corresponding thread id
   */
  void InsertWorkerIdThreadId(uint32_t worker_id, uint32_t thread_id);

 private:
  /**
   * Initiates the worker specification with the specified allocation
   * Update worker_to_node_, node_to_workers_ and num_workers_
   */
  void Init(const std::vector<WorkerAlloc>& worker_alloc);

  // {worker_id: node_id}: {0,0}, {1,0}, {2,0}, {3,1}, {4,1}
  std::map<uint32_t, uint32_t> worker_to_node_;
  // {node_id, {worker_ids}}: {0, {0,1,2}}, {1, {3,4}}
  std::map<uint32_t, std::vector<uint32_t>> node_to_workers_;

  // {0, 100}, {1, 101}, {2, 102}, {3, 1100}, {4, 1101}
  std::map<uint32_t, uint32_t> worker_to_thread_;
  std::map<uint32_t, uint32_t> thread_to_worker_;

  // {0,100}, {0,101}, {0,102}, {1,1100}, {1,1101}
  std::map<uint32_t, std::vector<uint32_t>> node_to_threads_;
  // {100, 101, 102, 1100, 1101}
  std::set<uint32_t> thread_ids_;
  uint32_t num_workers_ = 0;
};

}  // namespace csci5570
