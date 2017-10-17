#include "hdfs_assigner.hpp"

#include "glog/logging.h"

#include "base/serialization.hpp"
#include "base/zmq_helper.hpp"

namespace csci5570 {

const int HDFSBlockAssigner::kBlockRequest;
const int HDFSBlockAssigner::kExit;

bool HDFSBlockAssigner::BlkDesc::operator==(const BlkDesc& other) const {
  return filename == other.filename && offset == other.offset && block_location == block_location;
}

HDFSBlockAssigner::HDFSBlockAssigner(std::string hdfs_namenode, int hdfs_namenode_port, zmq::context_t* context,
                                     int master_port) {
  init_socket(master_port, context);
  init_hdfs(hdfs_namenode, hdfs_namenode_port);
}

void HDFSBlockAssigner::Serve() {
  LOG(INFO) << "HDFSBlockAssigner starts";
  while (running_) {
    zmq::message_t msg1, msg2, msg3;
    zmq_recv_common(master_socket_.get(), &msg1);
    std::string cur_client = std::string(reinterpret_cast<char*>(msg1.data()), msg1.size());
    zmq_recv_common(master_socket_.get(), &msg2);
    zmq_recv_common(master_socket_.get(), &msg3);
    int msg_int = *reinterpret_cast<int32_t*>(msg3.data());
    if (msg_int == kBlockRequest) {
      handle_block_request(cur_client);
    } else if (msg_int == kExit) {
      handle_exit();
    } else {
      CHECK(false) << "Unknown message: " << msg_int;
    }
  }
  LOG(INFO) << "HDFSBlockAssigner stops";
}

// =================== Private functions ===================

void HDFSBlockAssigner::halt() { running_ = false; }

/**
 * Handle worker exit, terminate when all workers exit
 */
void HDFSBlockAssigner::handle_exit() {
  std::string worker_name;
  int worker_id;
  BinStream stream;
  zmq::message_t msg;
  zmq_recv_common(master_socket_.get(), &msg);
  stream.push_back_bytes(reinterpret_cast<char*>(msg.data()), msg.size());
  stream >> worker_name >> worker_id;
  finished_workers_.insert(worker_id);

  LOG(INFO) << "master => worker finished @" << worker_name << "-" << std::to_string(worker_id);

  if ((finished_workers_.size() == num_workers_alive_)) {
    halt();
  }
}

void HDFSBlockAssigner::handle_block_request(const std::string& cur_client) {
  std::string url, host, load_type;
  int num_threads, id;

  zmq::message_t msg1;
  zmq_recv_common(master_socket_.get(), &msg1);
  BinStream stream;
  stream.push_back_bytes(reinterpret_cast<char*>(msg1.data()), msg1.size());
  stream >> url >> host >> num_threads >> id;

  // reset num_worker_alive
  num_workers_alive_ = num_threads;
  LOG(INFO) << url << " " << host << " " << num_threads << " " << id << " " << load_type;
  std::pair<std::string, size_t> ret = answer(host, url, id);
  stream.clear();
  stream << ret.first << ret.second;

  zmq_send_common(master_socket_.get(), cur_client.data(), cur_client.length(), ZMQ_SNDMORE);
  zmq_send_common(master_socket_.get(), nullptr, 0, ZMQ_SNDMORE);
  zmq_send_common(master_socket_.get(), stream.get_remained_buffer(), stream.size());
}

void HDFSBlockAssigner::init_socket(int master_port, zmq::context_t* zmq_context) {
  master_socket_.reset(new zmq::socket_t(*zmq_context, ZMQ_ROUTER));
  master_socket_->bind("tcp://*:" + std::to_string(master_port));
}

void HDFSBlockAssigner::init_hdfs(const std::string& node, const int& port) {
  int num_retries = 3;
  while (num_retries--) {
    struct hdfsBuilder* builder = hdfsNewBuilder();
    hdfsBuilderSetNameNode(builder, node.c_str());
    hdfsBuilderSetNameNodePort(builder, port);
    fs_ = hdfsBuilderConnect(builder);
    hdfsFreeBuilder(builder);
    if (fs_)
      break;
  }
  if (fs_) {
    return;
  }
  LOG(ERROR) << "Failed to connect to HDFS " << node << ":" << port;
}

/**
 * Build locality dictionary for files
 */
void HDFSBlockAssigner::browse_hdfs(int id, const std::string& url) {
  if (!fs_)
    return;

  // 1. List Url Directory
  int num_files;
  int dummy;
  hdfsFileInfo* file_info = hdfsListDirectory(fs_, url.c_str(), &num_files);

  // 2. Collect file locality info for specific task <id> and input <url>
  auto& files_locality = files_locality_multi_dict_[id][url];
  for (int i = 0; i < num_files; ++i) {         // for every file in a directory
    if (file_info[i].mKind != kObjectKindFile)  // omit directories
      continue;
    size_t k = 0;
    while (k < file_info[i].mSize) {
      // for every block in a file
      auto blk_loc = hdfsGetFileBlockLocations(fs_, file_info[i].mName, k, 1, &dummy);
      for (int j = 0; j < blk_loc->numOfNodes; ++j) {
        // for every replication in a block
        files_locality[blk_loc->hosts[j]].insert(
            BlkDesc{std::string(file_info[i].mName) + '\0', k, std::string(blk_loc->hosts[j])});
        // init finish_multi_dict_, 0 means no thread has been rejected by this (url, host)
        (finish_multi_dict_[id][url].first)[blk_loc->hosts[j]] = 0;
      }
      k += file_info[i].mBlockSize;
    }
  }

  // 3. Clear file info
  hdfsFreeFileInfo(file_info, num_files);
}

/**
 * Assign blocks to workers per request
 */
std::pair<std::string, size_t> HDFSBlockAssigner::answer(const std::string& host, const std::string& url, int id) {
  if (!fs_)
    return {"", 0};

  // 1. If id or url is not found, collect file locality information from hdfs
  if (files_locality_multi_dict_.find(id) == files_locality_multi_dict_.end() ||
      files_locality_multi_dict_[id].find(url) == files_locality_multi_dict_[id].end()) {
    browse_hdfs(id, url);
    // reset the url accessing times
    finish_multi_dict_[id][url].second = 0;
  }

  // selected_file, offset
  std::pair<std::string, size_t> ret = {"", 0};

  auto& all_files_locality = files_locality_multi_dict_[id][url];
  /*
   * when loading local host, selected_host equals host,
   * when loading gloabl host, selected_host equals an unfinished host
   */
  std::string selected_host;

  // if there is local file, allocate local file
  if (all_files_locality[host].size()) {
    selected_host = host;
  } else {  // there is no local file, so need to check all hosts
    bool is_finish = true;
    for (auto& item : all_files_locality) {
      if (item.second.size() != 0) {
        is_finish = false;
        selected_host = item.first;  // default allocate an unfinished host block
        break;
      }
    }

    if (is_finish) {
      finish_multi_dict_[id][url].second += 1;
      if (finish_multi_dict_[id][url].second == num_workers_alive_) {
        // this means all workers's requests about this url are rejected
        // blocks under this url are all allocated
        files_locality_multi_dict_[id].erase(url);
      }
      return {"", 0};
    }
  }

  // according selected_host to get the select file
  auto selected_file = all_files_locality[selected_host].begin();
  // select
  ret = {selected_file->filename, selected_file->offset};

  // caution: need to remove all replicas in different host
  for (auto its = all_files_locality.begin(); its != all_files_locality.end(); its++) {
    for (auto it = its->second.begin(); it != its->second.end(); it++) {
      if (it->filename == ret.first && it->offset == ret.second) {
        all_files_locality[its->first].erase(it);
        break;
      }
    }
  }

  return ret;
}

}  // namespace csci5570
