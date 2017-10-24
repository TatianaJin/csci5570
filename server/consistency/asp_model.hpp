#pragma once

#include "server/abstract_model.hpp"

#include "base/message.hpp"
#include "base/threadsafe_queue.hpp"
#include "server/abstract_model.hpp"
#include "server/abstract_storage.hpp"
#include "server/util/pending_buffer.hpp"
#include "server/util/progress_tracker.hpp"

namespace csci5570 {

/**
 * A wrapper for model with Asynchronous Parallel consistency
 */
class ASPModel : public AbstractModel {
 public:
  explicit ASPModel(uint32_t model_id, std::unique_ptr<AbstractStorage>&& storage_ptr,
                    ThreadsafeQueue<Message>* reply_queue);

  virtual void Clock(Message& msg) override;
  virtual void Add(Message& msg) override;
  virtual void Get(Message& msg) override;
  virtual int GetProgress(int tid) override;
  virtual void ResetWorker(Message& msg) override;

 private:
  uint32_t model_id_;

  ThreadsafeQueue<Message>* reply_queue_;     // not owned, the queue where reply messages are put
  std::unique_ptr<AbstractStorage> storage_;  // actual storage
  ProgressTracker progress_tracker_;          // the progresses of all worker threads interacting with the model
};

}  // namespace csci5570
