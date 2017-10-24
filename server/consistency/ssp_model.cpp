#include "server/consistency/ssp_model.hpp"
#include "glog/logging.h"

namespace csci5570 {

SSPModel::SSPModel(uint32_t model_id, std::unique_ptr<AbstractStorage>&& storage_ptr, int staleness,
                   ThreadsafeQueue<Message>* reply_queue) {
  // TODO
}

void SSPModel::Clock(Message& msg) {
  // TODO
}

void SSPModel::Add(Message& msg) {
  // TODO
}

void SSPModel::Get(Message& msg) {
  // TODO
}

int SSPModel::GetProgress(int tid) {
  // TODO
}

int SSPModel::GetPendingSize(int progress) {
  // TODO
}

void SSPModel::ResetWorker(Message& msg) {
  // TODO
}

}  // namespace csci5570
