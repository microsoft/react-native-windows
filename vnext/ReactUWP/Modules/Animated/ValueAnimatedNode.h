// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <folly/dynamic.h>
#include "AnimatedNode.h"

namespace winrt {
using namespace winrt::Windows::UI::Composition;
}

namespace react {
namespace uwp {
class ValueAnimatedNode : public AnimatedNode {
 public:
  ValueAnimatedNode(
      int64_t tag,
      const folly::dynamic &config,
      const std::shared_ptr<NativeAnimatedNodeManager> &manager);
  ValueAnimatedNode(
      int64_t tag,
      const std::shared_ptr<NativeAnimatedNodeManager> &manager);
  double Value();
  double RawValue();
  void RawValue(double value);
  double Offset();
  void Offset(double offset);
  void FlattenOffset();
  void ExtractOffset();
  winrt::CompositionPropertySet PropertySet() {
    return m_propertySet;
  };

  void AddDependentPropsNode(int64_t propsNodeTag);
  void RemoveDependentPropsNode(int64_t propsNodeTag);
  void AddActiveAnimation(int64_t animationTag);
  void RemoveActiveAnimation(int64_t animationTag);
  void AddActiveTrackingNode(int64_t trackingNodeTag);
  void RemoveActiveTrackingNode(int64_t trackingNodeTag);

  static constexpr std::wstring_view s_valueName{L"v"};
  static constexpr std::wstring_view s_offsetName{L"o"};

 protected:
  winrt::CompositionPropertySet m_propertySet{nullptr};

  static constexpr std::string_view s_inputName{"input"};

  static constexpr std::string_view s_jsValueName{"value"};
  static constexpr std::string_view s_jsOffsetName{"offset"};

 private:
  void UpdateTrackingNodes();
  std::unordered_set<int64_t> m_dependentPropsNodes{};
  std::unordered_set<int64_t> m_activeAnimations{};
  std::unordered_set<int64_t> m_activeTrackingNodes{};
};
} // namespace uwp
} // namespace react
