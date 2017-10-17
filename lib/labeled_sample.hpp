#pragma once

namespace csci5570 {
namespace lib {

// Consider both sparse and dense feature abstraction
// You may use Eigen::Vector and Eigen::SparseVector template
template <typename Feature, typename Label>
class LabeledSample {
 public:
  Feature x_;
  Label y_;
};  // class LabeledSample

}  // namespace lib
}  // namespace csci5570
