#include "server/abstract_model.hpp"

#include "base/message.hpp"
#include "base/threadsafe_queue.hpp"
#include "server/abstract_storage.hpp"
#include "server/util/pending_buffer.hpp"
#include "server/util/progress_tracker.hpp"

#include <map>
#include <vector>

namespace csci5570 {

/**
 * A wrapper for model with Stale Synchronous Parallel consistency
 */
class SSPModel : public AbstractModel {
 public:
  explicit SSPModel(uint32_t model_id, std::unique_ptr<AbstractStorage>&& storage_ptr, int staleness,
                    ThreadsafeQueue<Message>* reply_queue);

  virtual void Clock(Message& msg) override;
  virtual void Add(Message& msg) override;
  virtual void Get(Message& msg) override;
  virtual int GetProgress(int tid) override;
  virtual void ResetWorker(Message& msg) override;

  /**
   * Return the number of requests waiting at the specific progress
   *
   * @param progress    the progress clock number
   */
  int GetPendingSize(int progress);

 private:
  uint32_t model_id_;
  uint32_t staleness_;

  ThreadsafeQueue<Message>* reply_queue_;
  std::unique_ptr<AbstractStorage> storage_;
  ProgressTracker progress_tracker_;
  PendingBuffer buffer_;
};

}  // namespace csci5570
