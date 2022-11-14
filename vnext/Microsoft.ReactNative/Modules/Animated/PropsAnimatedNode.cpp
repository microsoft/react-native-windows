// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <Modules/NativeUIManager.h>
#include <Modules/PaperUIManagerModule.h>
#include <Utils/Helpers.h>
#include <Views/ShadowNodeBase.h>
#include <Views/XamlFeatures.h>
#include "NativeAnimatedNodeManager.h"
#include "PropsAnimatedNode.h"
#include "StyleAnimatedNode.h"

#ifdef USE_FABRIC
#include <Fabric/Composition/CompositionContextHelper.h>
#include <Fabric/Composition/CompositionUIService.h>
#include <Fabric/FabricUIManagerModule.h>
#endif

namespace Microsoft::ReactNative {
PropsAnimatedNode::PropsAnimatedNode(
    int64_t tag,
    const winrt::Microsoft::ReactNative::JSValueObject &config,
    const winrt::Microsoft::ReactNative::ReactContext &context,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager)
    : AnimatedNode(tag, config, manager), m_context(context) {
  for (const auto &entry : config["props"].AsObject()) {
    const auto inputTag = entry.second.AsInt64();
    m_propMapping.insert({entry.first, inputTag});
  }

  if (m_useComposition) {
    auto compositor = manager->Compositor();
    m_subchannelPropertySet = compositor.CreatePropertySet();
    m_subchannelPropertySet.InsertScalar(L"TranslationX", 0.0f);
    m_subchannelPropertySet.InsertScalar(L"TranslationY", 0.0f);
    m_subchannelPropertySet.InsertScalar(L"ScaleX", 1.0f);
    m_subchannelPropertySet.InsertScalar(L"ScaleY", 1.0f);

    m_translationCombined =
        compositor.CreateExpressionAnimation(L"Vector3(subchannels.TranslationX, subchannels.TranslationY, 0.0)");
    m_translationCombined.SetReferenceParameter(L"subchannels", m_subchannelPropertySet);
    m_translationCombined.Target(L"Translation");

    m_scaleCombined = compositor.CreateExpressionAnimation(L"Vector3(subchannels.ScaleX, subchannels.ScaleY, 1.0)");
    m_scaleCombined.SetReferenceParameter(L"subchannels", m_subchannelPropertySet);
    m_scaleCombined.Target(L"Scale");
  }
}

void PropsAnimatedNode::ConnectToView(int64_t viewTag) {
  if (m_connectedViewTag != s_connectedViewTagUnset) {
    throw std::invalid_argument(
        "Animated node " + std::to_string(m_tag) + " has already been attached to a view already exists.");
    return;
  }
  m_connectedViewTag = viewTag;

  if (m_useComposition) {
    UpdateView();
  }
}

void PropsAnimatedNode::DisconnectFromView(int64_t viewTag) {
  if (m_connectedViewTag == s_connectedViewTagUnset) {
    return;
  } else if (m_connectedViewTag != viewTag) {
    throw std::invalid_argument(
        "Attempting to disconnect view that has not been connected with the given animated node.");
    return;
  }

  if (m_useComposition) {
    std::vector<int64_t> keys{};
    for (const auto &anim : m_expressionAnimations) {
      keys.push_back(anim.first);
    }
    for (const auto &key : keys) {
      DisposeCompletedAnimation(key);
    }

    if (m_centerPointAnimation) {
      if (const auto target = GetUIElement()) {
        target.StopAnimation(m_centerPointAnimation);
      }
      m_centerPointAnimation = nullptr;
    }
    m_needsCenterPointAnimation = false;
  }

  m_connectedViewTag = s_connectedViewTagUnset;
}

void PropsAnimatedNode::RestoreDefaultValues() {
  if (!m_useComposition) {
    for (const auto &entry : m_props) {
      m_props[entry.first] = nullptr;
    }

    CommitProps();
  }
}

void PropsAnimatedNode::UpdateView() {
  if (m_connectedViewTag == s_connectedViewTagUnset) {
    return;
  }

  if (const auto manager = std::shared_ptr<NativeAnimatedNodeManager>(m_manager)) {
    for (const auto &entry : m_propMapping) {
      if (const auto &styleNode = manager->GetStyleAnimatedNode(entry.second)) {
        if (m_useComposition) {
          for (const auto &styleEntry : styleNode->GetMapping()) {
            MakeAnimation(styleEntry.second, styleEntry.first);
          }
        } else {
          styleNode->CollectViewUpdates(m_props);
        }
      } else if (const auto &valueNode = manager->GetValueAnimatedNode(entry.second)) {
        if (m_useComposition) {
          const auto &facade = StringToFacadeType(entry.first);
          if (facade != FacadeType::None) {
            MakeAnimation(entry.second, facade);
          }
        } else {
          m_props[entry.first] = valueNode->Value();
        }
      }
    }
  }

  if (m_useComposition) {
    StartAnimations();
  } else {
    CommitProps();
  }
}

static void EnsureUIElementDirtyForRender(xaml::UIElement uiElement) {
  auto compositeMode = uiElement.CompositeMode();
  switch (compositeMode) {
    case xaml::Media::ElementCompositeMode::SourceOver:
    case xaml::Media::ElementCompositeMode::MinBlend:
      uiElement.CompositeMode(xaml::Media::ElementCompositeMode::Inherit);
      break;
    default:
      uiElement.CompositeMode(xaml::Media::ElementCompositeMode::SourceOver);
      break;
  }
  uiElement.CompositeMode(compositeMode);
}

void PropsAnimatedNode::StartAnimations() {
  assert(m_useComposition);
  if (m_expressionAnimations.size()) {
    AnimationView view = GetAnimationView();
    if (view) {
      // Work around for https://github.com/microsoft/microsoft-ui-xaml/issues/2511
      if (view.m_element) {
        EnsureUIElementDirtyForRender(view.m_element);
      }
      for (const auto &anim : m_expressionAnimations) {
        if (anim.second.Target() == L"Translation.X") {
          m_subchannelPropertySet.StartAnimation(L"TranslationX", anim.second);
          StartAnimation(view, m_translationCombined);
        } else if (anim.second.Target() == L"Translation.Y") {
          m_subchannelPropertySet.StartAnimation(L"TranslationY", anim.second);
          StartAnimation(view, m_translationCombined);
        } else if (anim.second.Target() == L"Scale.X") {
          m_subchannelPropertySet.StartAnimation(L"ScaleX", anim.second);
          StartAnimation(view, m_scaleCombined);
        } else if (anim.second.Target() == L"Scale.Y") {
          m_subchannelPropertySet.StartAnimation(L"ScaleY", anim.second);
          StartAnimation(view, m_scaleCombined);
        } else if (anim.second.Target() == L"Rotation") {
          if (view.m_element) {
            view.m_element.RotationAxis(m_rotationAxis);
#ifdef USE_FABRIC
          } else {
            auto visual =
                winrt::Microsoft::ReactNative::Composition::implementation::CompositionContextHelper::InnerVisual(
                    view.m_componentView->Visual());
            visual.RotationAxis(m_rotationAxis);
#endif
          }
          StartAnimation(view, anim.second);
        } else {
          StartAnimation(view, anim.second);
        }
      }
      if (m_needsCenterPointAnimation) {
        if (!m_centerPointAnimation) {
          if (const auto manager = m_manager.lock()) {
            m_centerPointAnimation = manager->Compositor().CreateExpressionAnimation();
            m_centerPointAnimation.Target(L"CenterPoint");
            m_centerPointAnimation.SetReferenceParameter(L"centerPointPropertySet", EnsureCenterPointPropertySet(view));
            m_centerPointAnimation.Expression(L"centerPointPropertySet.center");
          }
        }

        StartAnimation(view, m_centerPointAnimation);
      }
    } else {
      if (const auto manager = m_manager.lock()) {
        manager->AddDelayedPropsNode(Tag(), m_context);
      }
    }
  }
}

void PropsAnimatedNode::DisposeCompletedAnimation(int64_t valueTag) {
  assert(m_useComposition);
  /*
  if (m_expressionAnimations.count(valueTag)) {
    if (const auto target = GetUIElement()) {
      // We should start and stop the expression animations if there are
      // no active animations. Suspending the active expression animations
      // while they are not in use causes subsequent key frame animations
      // which target the providing property set to never fire their completed
      // events. I can't explain this and for now and commenting out the code.
      // Fixing this will prevent memory bloat as the current solutions never
      // stops the expression animations that are built as a part of this
      // animation solution. Tracked by issue #3280.
      target.StopAnimation(m_expressionAnimations.at(valueTag));
      m_suspendedExpressionAnimationTags.push_back(valueTag);
    }
  }
  */
}

void PropsAnimatedNode::ResumeSuspendedAnimations(int64_t valueTag) {
  assert(m_useComposition);
  /*
  const auto iterator =
      std::find(m_suspendedExpressionAnimationTags.begin(), m_suspendedExpressionAnimationTags.end(), valueTag);
  if (iterator != m_suspendedExpressionAnimationTags.end()) {
    if (const auto target = GetUIElement()) {
      // See comment above, tracked by issue #3280
      target.StartAnimation(m_expressionAnimations.at(valueTag));
      m_suspendedExpressionAnimationTags.erase(iterator);
    }
  }
  */
}

void PropsAnimatedNode::MakeAnimation(int64_t valueNodeTag, FacadeType facadeType) {
  if (const auto manager = m_manager.lock()) {
    if (const auto valueNode = manager->GetValueAnimatedNode(valueNodeTag)) {
      const auto animation = manager->Compositor().CreateExpressionAnimation();
      animation.SetReferenceParameter(L"ValuePropSet", valueNode->PropertySet());
      animation.Expression(
          static_cast<winrt::hstring>(L"ValuePropSet.") + ValueAnimatedNode::s_valueName + L" + ValuePropSet." +
          ValueAnimatedNode::s_offsetName);
      switch (facadeType) {
        case FacadeType::Opacity:
          animation.Target(L"Opacity");
          break;
        case FacadeType::Rotation:
          m_rotationAxis = {0, 0, 1};
          animation.Expression(
              static_cast<winrt::hstring>(L"(ValuePropSet.") + ValueAnimatedNode::s_valueName + L" + ValuePropSet." +
              ValueAnimatedNode::s_offsetName + L") * 180 / PI");
          animation.Target(L"Rotation");
          m_needsCenterPointAnimation = true;
          break;
        case FacadeType::RotationX:
          animation.Expression(
              static_cast<winrt::hstring>(L"(ValuePropSet.") + ValueAnimatedNode::s_valueName + L" + ValuePropSet." +
              ValueAnimatedNode::s_offsetName + L") * 180 / PI");
          m_rotationAxis = {1, 0, 0};
          animation.Target(L"Rotation");
          m_needsCenterPointAnimation = true;
          break;
        case FacadeType::RotationY:
          animation.Expression(
              static_cast<winrt::hstring>(L"(ValuePropSet.") + ValueAnimatedNode::s_valueName + L" + ValuePropSet." +
              ValueAnimatedNode::s_offsetName + L") * 180 / PI");
          m_rotationAxis = {0, 1, 0};
          animation.Target(L"Rotation");
          m_needsCenterPointAnimation = true;
          break;
        case FacadeType::Scale:
          animation.Expression(
              static_cast<winrt::hstring>(L"Vector3(ValuePropSet.") + ValueAnimatedNode::s_valueName +
              L" + ValuePropSet." + ValueAnimatedNode::s_offsetName + L", ValuePropSet." +
              ValueAnimatedNode::s_valueName + L" + ValuePropSet." + ValueAnimatedNode::s_offsetName + L", 0)");
          animation.Target(L"Scale");
          m_needsCenterPointAnimation = true;
          break;
        case FacadeType::ScaleX:
          animation.Target(L"Scale.X");
          m_needsCenterPointAnimation = true;
          break;
        case FacadeType::ScaleY:
          animation.Target(L"Scale.Y");
          m_needsCenterPointAnimation = true;
          break;
        case FacadeType::TranslateX:
          animation.Target(L"Translation.X");
          break;
        case FacadeType::TranslateY:
          animation.Target(L"Translation.Y");
          break;
        case FacadeType::Perspective:
          // TODO: implement perspective animations, tracked by issue #2680
          return;
        case FacadeType::Progress:
          // TODO: implement progress animations, tracked by issue #3283
          return;
        default:
          assert(false);
      }
      m_expressionAnimations.insert({valueNode->Tag(), animation});
      valueNode->AddDependentPropsNode(Tag());
    }
  }
}

Microsoft::ReactNative::ShadowNodeBase *PropsAnimatedNode::GetShadowNodeBase() {
  if (const auto uiManager = Microsoft::ReactNative::GetNativeUIManager(m_context).lock()) {
    if (const auto nativeUIManagerHost = uiManager->getHost()) {
      return static_cast<Microsoft::ReactNative::ShadowNodeBase *>(
          nativeUIManagerHost->FindShadowNodeForTag(m_connectedViewTag));
    }
  }
  return nullptr;
}

xaml::UIElement PropsAnimatedNode::GetUIElement() {
  if (IsRS5OrHigher()) {
    if (const auto shadowNodeBase = GetShadowNodeBase()) {
      if (const auto shadowNodeView = shadowNodeBase->GetView()) {
        return shadowNodeView.as<xaml::UIElement>();
      }
    }
  }
  return nullptr;
}

void PropsAnimatedNode::CommitProps() {
  if (const auto node = GetShadowNodeBase()) {
    if (!node->m_zombie) {
      node->updateProperties(m_props);
    }
  }
}

PropsAnimatedNode::AnimationView PropsAnimatedNode::GetAnimationView() {
#ifdef USE_FABRIC
  if (auto fabricuiManager = FabricUIManager::FromProperties(m_context.Properties())) {
    auto componentView = fabricuiManager->GetViewRegistry().findComponentViewWithTag(
        static_cast<facebook::react::Tag>(m_connectedViewTag));
    if (componentView) {
      return {nullptr, std::static_pointer_cast<CompositionBaseComponentView>(componentView)};
    }
  }
#endif
  if (IsRS5OrHigher()) {
    if (const auto shadowNodeBase = GetShadowNodeBase()) {
      if (const auto shadowNodeView = shadowNodeBase->GetView()) {
#ifdef USE_FABRIC
        return {shadowNodeView.as<xaml::UIElement>(), nullptr};
#else
        return {shadowNodeView.as<xaml::UIElement>()};
#endif
      }
    }
  }

#ifdef USE_FABRIC
  return {nullptr, nullptr};
#else
  return {nullptr};
#endif
}

void PropsAnimatedNode::StartAnimation(
    const AnimationView &view,
    const comp::CompositionAnimation &animation) noexcept {
  if (view.m_element) {
    view.m_element.StartAnimation(animation);
#ifdef USE_FABRIC
  } else if (view.m_componentView) {
    auto visual = winrt::Microsoft::ReactNative::Composition::implementation::CompositionContextHelper::InnerVisual(
        view.m_componentView->Visual());
    if (visual) {
      auto targetProp = animation.Target();
      if (targetProp == L"Rotation") {
        targetProp = L"RotationAngleInDegrees";
      } else if (targetProp == L"Transform") {
        view.m_componentView->EnsureCenterPointPropertySet().StartAnimation(L"transform", animation);
        return;
      } else if (targetProp == L"Translation") {
        view.m_componentView->EnsureTransformMatrixFacade();
        view.m_componentView->EnsureCenterPointPropertySet().StartAnimation(L"translation", animation);

        return;
      }
      visual.StartAnimation(targetProp, animation);
    }
#endif
  }
}

comp::CompositionPropertySet PropsAnimatedNode::EnsureCenterPointPropertySet(const AnimationView &view) noexcept {
  if (view.m_element) {
    return GetShadowNodeBase()->EnsureTransformPS();
#ifdef USE_FABRIC
  } else if (view.m_componentView) {
    return view.m_componentView->EnsureCenterPointPropertySet();
#endif
  }
  return nullptr;
}

} // namespace Microsoft::ReactNative
