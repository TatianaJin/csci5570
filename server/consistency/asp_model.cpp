#include "server/consistency/asp_model.hpp"
#include "glog/logging.h"

namespace csci5570 {

ASPModel::ASPModel(uint32_t model_id, std::unique_ptr<AbstractStorage>&& storage_ptr,
                   ThreadsafeQueue<Message>* reply_queue) {
  // TODO
}

void ASPModel::Clock(Message& msg) {
  // TODO
}

void ASPModel::Add(Message& msg) {
  // TODO
}

void ASPModel::Get(Message& msg) {
  // TODO
}

int ASPModel::GetProgress(int tid) {
  // TODO
}

void ASPModel::ResetWorker(Message& msg) {
  // TODO
}

}  // namespace csci5570
