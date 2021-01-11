// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <IReactInstance.h>
#include <React.h>
#include <folly/dynamic.h>
#include "AnimatedNode.h"

#include "FacadeType.h"

namespace Microsoft::ReactNative {
struct ShadowNodeBase;
}

namespace react::uwp {
class PropsAnimatedNode final : public AnimatedNode {
 public:
  PropsAnimatedNode(
      int64_t tag,
      const folly::dynamic &config,
      const Mso::CntPtr<Mso::React::IReactContext> &context,
      const std::shared_ptr<NativeAnimatedNodeManager> &manager);
  void ConnectToView(int64_t viewTag);
  void DisconnectFromView(int64_t viewTag);
  void RestoreDefaultValues();
  void UpdateView();
  void StartAnimations();
  void DisposeCompletedAnimation(int64_t valueTag);
  void ResumeSuspendedAnimations(int64_t valueTag);

 private:
  void MakeAnimation(int64_t valueNodeTag, FacadeType facadeType);
  Microsoft::ReactNative::ShadowNodeBase *GetShadowNodeBase();
  xaml::UIElement GetUIElement();

  Mso::CntPtr<Mso::React::IReactContext> m_context{};
  std::map<std::string, int64_t> m_propMapping{};
  folly::dynamic m_propMap{};

  int64_t m_connectedViewTag{s_connectedViewTagUnset};
  std::unordered_map<int64_t, comp::CompositionAnimation> m_expressionAnimations{};
  // std::vector<int64_t> m_suspendedExpressionAnimationTags{};
  comp::ExpressionAnimation m_centerPointAnimation{nullptr};
  winrt::Numerics::float3 m_rotationAxis{0, 0, 1};
  bool m_needsCenterPointAnimation{false};
  comp::CompositionPropertySet m_subchannelPropertySet{nullptr};
  comp::CompositionAnimation m_translationCombined{nullptr};
  comp::CompositionAnimation m_scaleCombined{nullptr};

  static constexpr int64_t s_connectedViewTagUnset{-1};
};
} // namespace react::uwp
