#pragma once

#include "base/actor_model.hpp"
#include "base/threadsafe_queue.hpp"
#include "server/abstract_model.hpp"

#include <thread>
#include <unordered_map>

namespace csci5570 {

class AbstractServerThread : public Actor {
 public:
  AbstractServerThread(uint32_t server_id) : Actor(server_id) {}

  // for model maintenance
  virtual void RegisterModel(uint32_t model_id, std::unique_ptr<AbstractModel>&& model) = 0;
  virtual AbstractModel* GetModel(uint32_t model_id) = 0;

 protected:
  std::unordered_map<uint32_t, std::unique_ptr<AbstractModel>> models_;
};

}  // namespace csci5570
