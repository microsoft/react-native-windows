// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <IReactInstance.h>
#include <folly/dynamic.h>
#include "AnimatedNode.h"

#include "FacadeType.h"

namespace react {
namespace uwp {
struct ShadowNodeBase;
class PropsAnimatedNode : public AnimatedNode {
 public:
  PropsAnimatedNode(
      int64_t tag,
      const folly::dynamic &config,
      const std::weak_ptr<IReactInstance> &instance,
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
  ShadowNodeBase *GetShadowNodeBase();
  winrt::UIElement GetUIElement();

  std::weak_ptr<IReactInstance> m_instance{};
  std::map<std::string, int64_t> m_propMapping{};
  folly::dynamic m_propMap{};

  int64_t m_connectedViewTag{s_connectedViewTagUnset};
  std::unordered_map<
      int64_t,
      winrt::Windows::UI::Composition::CompositionAnimation>
      m_expressionAnimations{};
  std::vector<int64_t> m_suspendedExpressionAnimationTags{};
  winrt::Windows::UI::Composition::ExpressionAnimation m_centerPointAnimation{
      nullptr};
  winrt::Numerics::float3 m_rotationAxis{0, 0, 1};
  bool m_needsCenterPointAnimation{false};
  winrt::CompositionPropertySet m_subchannelPropertySet{nullptr};
  winrt::CompositionAnimation m_translationCombined{nullptr};
  winrt::CompositionAnimation m_scaleCombined{nullptr};

  static constexpr int64_t s_connectedViewTagUnset{-1};
};
} // namespace uwp
} // namespace react
