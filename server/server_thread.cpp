#include "server/server_thread.hpp"

#include "glog/logging.h"

namespace csci5570 {

void ServerThread::RegisterModel(uint32_t model_id, std::unique_ptr<AbstractModel>&& model) {
  models_.insert(std::make_pair(model_id, std::move(model)));
}

AbstractModel* ServerThread::GetModel(uint32_t model_id) {
  auto it = models_.find(model_id);
  if (it != models_.end()){
     return it->second.get();
   } else {
     return nullptr;
   }
}

void ServerThread::Main() {
}

}  // namespace csci5570

