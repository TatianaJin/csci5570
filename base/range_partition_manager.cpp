#include "base/range_partition_manager.hpp"

namespace csci5570 {

	/*class RangePartitionManager : public AbstractPartitionManager {
	public:
		RangePartitionManager(const std::vector<uint32_t>& server_thread_ids, const std::vector<third_party::Range>& ranges)
			: AbstractPartitionManager(server_thread_ids) {}

		void Slice(const Keys& keys, std::vector<std::pair<int, Keys>>* sliced) override {}

		void Slice(const KVPairs& kvs, std::vector<std::pair<int, KVPairs>>* sliced) override {}
		private:
		std::vector<third_party::Range> ranges_;
	};*/


	RangePartitionManager::RangePartitionManager(const std::vector<uint32_t>& server_thread_ids, const std::vector<third_party::Range>& ranges)
		: AbstractPartitionManager(server_thread_ids) {
		this->ranges_ = ranges;

	}
	void RangePartitionManager::Slice(const Keys& keys, std::vector<std::pair<int, Keys>>* sliced) const override {
		LOG(INFO) << "Test by Andy";
		const int keys_size = keys.size();//Num of keys
		const int servers_size = this->GetNumServers();//Num of Server
		int keys_per_server = keys_size / servers_size;	//keys per server
														//Range slice			
		std::pair<int, Keys> tempPair;
		//Keys on first server to n-1 server
		for (int i = 0; i < servers_size - 1; i++) {
			tempPair = { (this->GetServerThreadIds())[i] ,keys.segment(i*keys_per_server,(i + 1)*keys_per_server) };
			sliced->push_back(tempPair);
		}
		//Keys on the last server
		tempPair = { (this->GetServerThreadIds())[servers_size - 1],keys.segment((servers_size - 1)*keys_per_server,keys.size()) };
		sliced->push_back(tempPair);

	}
	void RangePartitionManager::Slice(const KVPairs& kvs, std::vector<std::pair<int, KVPairs>>* sliced)const override {
		//const int kvs_size = kvs.size();//Num of keys
		//const int servers_size = GetNumServers();//Num of Server
		//int kvs_per_server = kvs_size / servers_size;	//keys per server
		//												//Range slice
		//												//Keys on first server to n-1 server
		//std::pair<int, keys> tempPair;
		//for (int i = 0; i < serverCount - 1; i++) {
		//	tempPair = { GetServerThreadIds[i] ,kvs.segment(i*kvs_per_server,(i + 1)*kvs_per_server };
		//	sliced->push_back(tempPair);
		//}
		////Keys on the last server
		//tempPair = { GetServerThreadIds[servers_size - 1],kvs.segment((servers_size - 1)*kvs_per_server,keys.size()) };
		//sliced->push_back(tempPair);

	}


}  // namespace csci5570
