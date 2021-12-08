// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <JSValue.h>
#include <cstdint>
#include <memory>
#include <vector>

namespace Microsoft::ReactNative {
class NativeAnimatedNodeManager;
class AnimatedNode {
 public:
  AnimatedNode(
      int64_t tag,
      const winrt::Microsoft::ReactNative::JSValueObject &config,
      const std::shared_ptr<NativeAnimatedNodeManager> &manager);
  int64_t Tag();
  void AddChild(int64_t animatedNode);
  void RemoveChild(int64_t animatedNode);

  std::vector<int64_t> &Children() {
    return m_children;
  }

  virtual bool UseComposition() const noexcept {
    return m_useComposition;
  }

  virtual void Update(){};
  virtual void OnDetachedFromNode(int64_t /*animatedNodeTag*/){};
  virtual void OnAttachToNode(int64_t /*animatedNodeTag*/){};
  virtual ~AnimatedNode() = default;

 protected:
  AnimatedNode *GetChildNode(int64_t tag);
  int64_t m_tag{0};
  const std::weak_ptr<NativeAnimatedNodeManager> m_manager;
  std::vector<int64_t> m_children{};
  bool m_useComposition{false};

  bool HasCompatibleAnimationDriver(int64_t tag);

  static constexpr std::string_view s_platformConfigName{"platformConfig"};
  static constexpr std::string_view s_useCompositionName{"useComposition"};
};
} // namespace Microsoft::ReactNative
