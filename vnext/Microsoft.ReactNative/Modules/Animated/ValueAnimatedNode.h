// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <JSValue.h>
#include <UI.Composition.h>
#include <unordered_set>
#include "AnimatedNode.h"

namespace winrt {
using namespace comp;
}

namespace Microsoft::ReactNative {
typedef std::function<void(double)> ValueListenerCallback;

class ValueAnimatedNode : public AnimatedNode {
 public:
  ValueAnimatedNode(
      int64_t tag,
      const winrt::Microsoft::ReactNative::JSValueObject &config,
      const std::shared_ptr<NativeAnimatedNodeManager> &manager);
  double Value();
  double RawValue();
  void RawValue(double value);
  double Offset();
  void Offset(double offset);
  void FlattenOffset();
  void ExtractOffset();
  void OnValueUpdate();
  void ValueListener(const ValueListenerCallback &callback);

  comp::CompositionPropertySet PropertySet() {
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
  comp::CompositionPropertySet m_propertySet{nullptr};
  static constexpr std::string_view s_inputName{"input"};

  static constexpr std::string_view s_jsValueName{"value"};
  static constexpr std::string_view s_jsOffsetName{"offset"};

 private:
  void UpdateTrackingNodes();
  std::unordered_set<int64_t> m_dependentPropsNodes{};
  std::unordered_set<int64_t> m_activeAnimations{};
  std::unordered_set<int64_t> m_activeTrackingNodes{};
  double m_value{0.0};
  double m_offset{0.0};
  ValueListenerCallback m_valueListener{};
};
} // namespace Microsoft::ReactNative
