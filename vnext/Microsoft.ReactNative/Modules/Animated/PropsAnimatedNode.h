// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <IReactInstance.h>
#include <React.h>
#include <ReactContext.h>
#include <folly/dynamic.h>
#include "AnimatedNode.h"
#include "FacadeType.h"
#include "JSValue.h"

#ifdef USE_FABRIC
#include <Fabric/Composition/CompositionViewComponentView.h>
#endif

namespace Microsoft::ReactNative {
struct ShadowNodeBase;
}

namespace Microsoft::ReactNative {
class PropsAnimatedNode final : public AnimatedNode {
 public:
  PropsAnimatedNode(
      int64_t tag,
      const winrt::Microsoft::ReactNative::JSValueObject &config,
      const winrt::Microsoft::ReactNative::ReactContext &context,
      const std::shared_ptr<NativeAnimatedNodeManager> &manager);
  void ConnectToView(int64_t viewTag);
  void DisconnectFromView(int64_t viewTag);
  void RestoreDefaultValues();
  void UpdateView();
  void StartAnimations();
  void DisposeCompletedAnimation(int64_t valueTag);
  void ResumeSuspendedAnimations(int64_t valueTag);

 private:
  struct AnimationView {
    xaml::UIElement m_element;
#ifdef USE_FABRIC
    std::shared_ptr<CompositionBaseComponentView> m_componentView;
#endif
    operator bool() const noexcept {
#ifdef USE_FABRIC
      return m_element || m_componentView;
#else
      return m_element != nullptr;
#endif
    }
  };

  void CommitProps();
  void MakeAnimation(int64_t valueNodeTag, FacadeType facadeType);
  Microsoft::ReactNative::ShadowNodeBase *GetShadowNodeBase();
  xaml::UIElement GetUIElement();
  AnimationView GetAnimationView();
  void StartAnimation(const AnimationView &view, const comp::CompositionAnimation &animation) noexcept;
  comp::CompositionPropertySet EnsureCenterPointPropertySet(const AnimationView &view) noexcept;

  winrt::Microsoft::ReactNative::ReactContext m_context;
  std::map<std::string, int64_t> m_propMapping{};
  winrt::Microsoft::ReactNative::JSValueObject m_props{};

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
} // namespace Microsoft::ReactNative
