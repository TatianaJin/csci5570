#pragma once

#include "base/abstract_id_mapper.hpp"

#include <cinttypes>
#include <map>
#include <set>
#include <vector>

#include "base/node.hpp"

namespace csci5570 {

/*
 * node_id = tid / kMaxThreadsPerNode
 * e.g. the thread id on node 0 range from [0,kMaxThreadsPerNode)
 *      the thread id on node 1 range from [kMaxThreadsPerNode, 2* kMaxThreadsPerNode)
 */
class SimpleIdMapper : public AbstractIdMapper {
 public:
  SimpleIdMapper(Node node, const std::vector<Node>& nodes);

  /**
   * Finds the host id of a specific thread
   *
   * @param tid    the thread id
   * @return       the host id of the specified thread
   */
  virtual uint32_t GetNodeIdForThread(uint32_t tid) override;

  /**
   * Initiate the id mapper with the global information of worker and server threads
   * 1. Do some checking on the <num_server_threads_per_node>, which should be in [1, kWorkerThreadId]
   * 2. For each node of all available nodes
   *    a. update node2server_
   *    b. update node2worker_ (for simplication, only one worker thread on each process for now)
   */
  void Init(int num_server_threads_per_node);

  /**
   * Allocates an id to a worker(user) thread on the specified node
   * Returns the allocated thread id on success, and -1 on failure
   * 1. Check that the worker thread is registered and there is slot for creating a new worker(user) thread (max is
   * kMaxThreadsPerNode - kMaxBgThreadsPerNode)
   * 2. Find an unoccupied id for the specified node
   *
   * @param node_id the node to assign worker(user) thread
   * @return        the allocated thread id or -1
   */
  uint32_t AllocateWorkerThread(uint32_t node_id);
  /**
   * Deallocates the worker thread and free the id it occupies
   *
   * @param node_id the node where the worker(user) thread is on
   * @param tid     the worker(user) thread to deallocate
   */
  void DeallocateWorkerThread(uint32_t node_id, uint32_t tid);

  /**
   * Returns the ids of servers on the specified node
   * @param node_id     the node id
   */
  std::vector<uint32_t> GetServerThreadsForId(uint32_t node_id);
  /**
   * Returns the ids of background workers on the specified node
   * @param node_id     the node id
   */
  std::vector<uint32_t> GetWorkerHelperThreadsForId(uint32_t node_id);
  /**
   * Returns the ids of user worker threads on the specified node
   * @param node_id     the node id
   */
  std::vector<uint32_t> GetWorkerThreadsForId(uint32_t node_id);

  /**
   * Returns the ids of all the server threads globally
   */
  std::vector<uint32_t> GetAllServerThreads();

  static const uint32_t kMaxNodeId = 1000;
  static const uint32_t kMaxThreadsPerNode = 1000;
  // BgThreads include server threads and worker threads
  // Their ids are [0, 100) for node id 0.
  static const uint32_t kMaxBgThreadsPerNode = 100;
  // The server thread id for node 0 is in [0, 50)
  // The worker thread id for node id 0 is in [50, 100)
  static const uint32_t kWorkerHelperThreadId = 50;

 private:
  // The server thread's id in each node
  std::map<uint32_t, std::vector<uint32_t>> node2server_;
  std::map<uint32_t, std::vector<uint32_t>> node2worker_helper_;
  std::map<uint32_t, std::set<uint32_t>> node2worker_;

  Node node_;
  std::vector<Node> nodes_;
};

}  // namespace csci5570
