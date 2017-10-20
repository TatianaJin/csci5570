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

	void RangePartitionManager::Slice(const Keys& keys, std::vector<std::pair<int, Keys>>* sliced) const {
		LOG(INFO) << "Test by Andy 1";
		const int keys_size = keys.size();//Num of keys
		const int range_size = this->ranges_.size();

		for (int j = 0; j < range_size; j++)
		{
			Keys tempKeys;
			for (int i = 0; i < keys_size; i++) {
				if (keys[i] >= this->ranges_[j].begin() && keys[i] < this->ranges_[j].end()) {
					tempKeys.push_back(keys[i]);
				}
			}
			if (tempKeys.size() > 0) {
				std::pair<int, Keys> tempPair (this->server_thread_ids_[j], tempKeys);
				sliced->push_back(tempPair);
			}

		}

	}

	void RangePartitionManager::Slice(const KVPairs& kvs, std::vector<std::pair<int, KVPairs>>* sliced)const  {
		LOG(INFO) << "Test by Andy 2";
		Keys keys = kvs.first;
		third_party::SArray<double> vals = kvs.second;
		const int keys_size = keys.size();//Num of keys
		LOG(INFO) <<"Keys size"<< keys_size;
		const int range_size = this->ranges_.size();
		LOG(INFO) <<"Range size"<< range_size;
		for (int j = 0; j < range_size; j++)
		{
			Keys temp_keys;
			third_party::SArray<double> temp_vals;
			for (int i = 0; i < keys_size; i++) {
				LOG(INFO) <<i<<"Key:"<< keys[i]<<" Val:"<<vals[i];
				
				if (keys[i] >= this->ranges_[j].begin() && keys[i] < this->ranges_[j].end()) {
					//tempKVs.push_back(kvs[i]);
					temp_keys.push_back(keys[i]);
					temp_vals.push_back(vals[i]);
				}
			}
			if (temp_keys.size() > 0) {
				std::pair<int, KVPairs> temp_pair(this->server_thread_ids_[j], std::make_pair(temp_keys, temp_vals));
				sliced->push_back(temp_pair);
			}

		}
	}


}  // namespace csci5570
