#include "base/abstract_partition_manager.hpp"
#include "glog/logging.h"

namespace csci5570 {

	/*
	 * Implments the interface of a PartitionManager which provides the model partitioning scheme
	 */
	
		AbstractPartitionManager::AbstractPartitionManager(const std::vector<uint32_t>& server_thread_ids) : server_thread_ids_(server_thread_ids) {
			this->server_thread_ids_ = server_thread_ids;
		}
		size_t AbstractPartitionManager::GetNumServers() const {
			return server_thread_ids_.size();
		}
		const AbstractPartitionManager::std::vector<uint32_t>& GetServerThreadIds() const {
			return server_thread_ids_;
		}
		//// slice keys into <server_id, key_partition> pairs
		//void AbstractPartitionManager::Slice(const Keys& keys, std::vector<std::pair<int, Keys>>* sliced) {
		//	const int keys_size = keys.size();//Num of keys
		//	const int servers_size = GetNumServers();//Num of Server
		//	int kvs_per_server = keysCount / servers_size;	//keys per server
		//	//Range slice
		//	//Keys on first server to n-1 server
		//	pair<int, keys> tempPair;
		//	for (int i = 0; i < serverCount - 1; i++) {
		//		tempPair = { GetServerThreadIds[i] ,keys.segment(i*keys_per_server,(i + 1)*keys_per_server };
		//		sliced->push_back(tempPair);
		//	}
		//	//Keys on the last server
		//	tempPair = { GetServerThreadIds[servers_size - 1],keys.segment((servers_size - 1)*kvs_per_server,keys.size()) };
		//	sliced->push_back(tempPair);
		//	
		//}

		//// slice key-value pairs into <server_id, key_value_partition> pairs
		//void AbstractPartitionManager::Slice(const KVPairs& kvs, std::vector<std::pair<int, KVPairs>>* sliced) {
		//	EXPECT_EQ(kvs.first.size(), kvs.second.size());
		//	const int kvs_size = kvs.size();//Num of keys
		//	const int servers_size = GetNumServers();//Num of Server
		//	int kvs_per_server = keysCount / servers_size;	//keys per server
		//													//Range slice
		//													//Keys on first server to n-1 server
		//	pair<int, keys> tempPair;
		//	for (int i = 0; i < serverCount - 1; i++) {
		//		tempPair = { GetServerThreadIds[i] ,kvs.segment(i*keys_per_server,(i + 1)*keys_per_server };
		//		sliced->push_back(tempPair);
		//	}
		//	//Keys on the last server
		//	tempPair = { GetServerThreadIds[servers_size - 1],kvs.segment((servers_size - 1)*kvs_per_server,keys.size()) };
		//	sliced->push_back(tempPair);
		//	//const int N = kvs.size();
		//	////Total number of kvs 
		//	//int keys_per_server = N / GetNumServers();
		//	////Range slice
		//	//for (int i = 0; i < N; i++) {
		//	//	int index = i / divider;
		//	//	pair<int, KVPairs> tempPair = { GetServerThreadIds[index] ,kvs[i] };
		//	//	sliced->push_back(tempPair);
		//	//}
		//}

	
}  // namespace csci5570
