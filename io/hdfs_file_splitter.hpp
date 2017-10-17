#pragma once

#include <string>

#include "boost/utility/string_ref.hpp"
#include "glog/logging.h"
#include "hdfs/hdfs.h"

#include "io/coordinator.hpp"

namespace csci5570 {

class HDFSFileSplitter {
 public:
  HDFSFileSplitter(int num_threads, int id, Coordinator* coordinator, std::string hostname, std::string hdfs_namenode,
                   int hdfs_namenode_port);

  virtual ~HDFSFileSplitter();

  boost::string_ref fetch_block(bool is_next = false);

  static void init_blocksize(hdfsFS fs, const std::string& url);

  void load(std::string url);

  size_t get_offset();

 private:
  int read_block(const std::string& fn);

  Coordinator* coordinator_;
  int num_threads_;
  int id_;
  size_t offset_ = 0;

  // using heap to store data
  char* data_;
  hdfsFile file_ = NULL;
  hdfsFS fs_;

  // url may be a directory, so that cur_file is different from url
  std::string url_;
  std::string hostname_;
  std::string hdfs_namenode_;
  int hdfs_namenode_port_;

  static size_t hdfs_block_size;

};  // class HDFSFileSplitter

}  // namespace csci5570
