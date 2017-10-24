#pragma once

#include <map>
#include <vector>

namespace csci5570 {

class ProgressTracker {
 public:
  void Init(const std::vector<uint32_t>& tids);
  /**
   * Advance the progress of a worker thread
   * Return -1 if min_clock_ does not change,
   * return min_clock_ otherwise.
   *
   * @param tid worker thread id
   */
  int AdvanceAndGetChangedMinClock(int tid);
  /**
   * Get the progress of a worker thread
   *
   * @param tid worker thread id
   */
  int GetProgress(int tid) const;
  /**
   * Get the progress of the slowest worker
   */
  int GetMinClock() const;
  /**
   * Get the number of workers in the trace
   */
  int GetNumThreads() const;
  /**
   * Check whether a worker thread is the only one with the slowest progress
   *
   * @param tid worker thread id
   */
  bool IsUniqueMin(int tid) const;
  /**
   * Check whether a worker thread is in the trace (thus valid to interact with the model)
   *
   * @param tid worker thread id
   */
  bool CheckThreadValid(int tid) const;

 private:
  std::map<int, int> progresses_;  // {tid: progress}
  int min_clock_;                  // the slowest progress
};

}  // namespace csci5570
