#pragma once

#include <cinttypes>
#include <functional>
#include <map>
#include <vector>

#include "driver/info.hpp"

namespace csci5570 {

struct WorkerAlloc {
  uint32_t node_id;
  uint32_t num_workers;
};

class MLTask {
 public:
  /**
   * Set the UDF
   *
   * @param func    the user defined function with one argument Info
   */
  void SetLambda(const std::function<void(const Info&)>& func) { func_ = func; }

  /**
   * Run the UDF
   *
   * @param info    the context for the thread running UDF
   */
  void RunLambda(const Info& info) const { func_(info); }
  void SetWorkerAlloc(const std::vector<WorkerAlloc>& worker_alloc) { worker_alloc_ = worker_alloc; }
  const std::vector<WorkerAlloc>& GetWorkerAlloc() const { return worker_alloc_; }
  void SetTables(const std::vector<uint32_t>& tables) { tables_ = tables; }
  const std::vector<uint32_t>& GetTables() const { return tables_; }
  bool IsSetup() const { return func_ && !worker_alloc_.empty() && !tables_.empty(); }

 private:
  std::function<void(const Info&)> func_;  // UDF
  std::vector<WorkerAlloc> worker_alloc_;  // the allocation of worker to the current task
  std::vector<uint32_t> tables_;           // model ids
};

}  // namespace csci5570
