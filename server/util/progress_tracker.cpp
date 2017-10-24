#include "server/util/progress_tracker.hpp"

#include "glog/logging.h"

namespace csci5570 {

void ProgressTracker::Init(const std::vector<uint32_t>& tids) {
  // TODO
}

int ProgressTracker::AdvanceAndGetChangedMinClock(int tid) {
  // TODO
}

int ProgressTracker::GetNumThreads() const {
  // TODO
}

int ProgressTracker::GetProgress(int tid) const {
  // TODO
}

int ProgressTracker::GetMinClock() const {
  // TODO
}

bool ProgressTracker::IsUniqueMin(int tid) const {
  // TODO
}

bool ProgressTracker::CheckThreadValid(int tid) const {
  // TODO
}

}  // namespace csci5570
