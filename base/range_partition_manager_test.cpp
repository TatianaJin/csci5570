#include "glog/logging.h"
#include "gtest/gtest.h"

#include "base/magic.hpp"
#include "base/range_partition_manager.hpp"

namespace csci5570 {

class TestRangePartitionManager : public testing::Test {
 protected:
  void SetUp() {}
  void TearDown() {}
};  // class TestRangePartitionManager

TEST_F(TestRangePartitionManager, Init) { RangePartitionManager pm({0, 1, 2}, {{2, 4}, {4, 7}, {7, 10}}); }

TEST_F(TestRangePartitionManager, SliceKeys) {
  RangePartitionManager pm({0, 1, 2}, {{2, 4}, {4, 7}, {7, 10}});
  third_party::SArray<Key> keys({2, 8, 9});
  std::vector<std::pair<int, AbstractPartitionManager::Keys>> sliced;
  pm.Slice(keys, &sliced);

  ASSERT_EQ(sliced.size(), 2);            // 2 slices for 2 servers
  EXPECT_EQ(sliced[0].first, 0);          // key to server 0
  EXPECT_EQ(sliced[1].first, 2);          // keys to server 2
  ASSERT_EQ(sliced[0].second.size(), 1);  // key 2
  ASSERT_EQ(sliced[1].second.size(), 2);  // keys 8, 9
  EXPECT_EQ(sliced[0].second[0], 2);
  EXPECT_EQ(sliced[1].second[0], 8);
  EXPECT_EQ(sliced[1].second[1], 9);
}

TEST_F(TestRangePartitionManager, SliceKVs) {
  RangePartitionManager pm({0, 1, 2}, {{0, 4}, {4, 8}, {8, 10}});
  third_party::SArray<Key> keys({2, 5, 9});
  third_party::SArray<double> vals({.2, .5, .9});
  std::vector<std::pair<int, AbstractPartitionManager::KVPairs>> sliced;
  pm.Slice(std::make_pair(keys, vals), &sliced);

  ASSERT_EQ(sliced.size(), 3);  // 3 slices for 3 servers
  EXPECT_EQ(sliced[0].first, 0);
  EXPECT_EQ(sliced[1].first, 1);
  EXPECT_EQ(sliced[2].first, 2);
  ASSERT_EQ(sliced[0].second.first.size(), 1);  // key 2
  EXPECT_EQ(sliced[0].second.first[0], 2);
  ASSERT_EQ(sliced[0].second.second.size(), 1);  // value .2
  EXPECT_DOUBLE_EQ(sliced[0].second.second[0], .2);
  ASSERT_EQ(sliced[1].second.first.size(), 1);  // key 5
  EXPECT_EQ(sliced[1].second.first[0], 5);
  ASSERT_EQ(sliced[1].second.second.size(), 1);  // value .5
  EXPECT_DOUBLE_EQ(sliced[1].second.second[0], .5);
  ASSERT_EQ(sliced[2].second.first.size(), 1);  // key 9
  EXPECT_EQ(sliced[2].second.first[0], 9);
  ASSERT_EQ(sliced[2].second.second.size(), 1);  // value .9
  EXPECT_DOUBLE_EQ(sliced[2].second.second[0], .9);
}

}  // namespace csci5570
