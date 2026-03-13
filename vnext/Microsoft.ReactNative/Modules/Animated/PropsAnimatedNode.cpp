// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <Utils/Helpers.h>
#include "NativeAnimatedNodeManager.h"
#include "PropsAnimatedNode.h"
#include "StyleAnimatedNode.h"

#include <Fabric/Composition/CompositionContextHelper.h>
#include <Fabric/Composition/CompositionUIService.h>
#include <Fabric/FabricUIManagerModule.h>

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
        } else if (valueNode->IsColorValue()) {
          const auto value = valueNode->Value();
          int32_t color;
          memcpy(&color, &value, sizeof(int32_t));
          m_props[entry.first] = color;
        } else {
          m_props[entry.first] = valueNode->Value();
        }
      } else if (const auto &colorNode = manager->GetColorAnimatedNode(entry.second)) {
        if (m_useComposition) {
          const auto &facade = StringToFacadeType(entry.first);
          if (facade != FacadeType::None) {
            MakeAnimation(entry.second, facade);
          }
        } else if (valueNode->IsColorValue()) {
          const auto value = valueNode->Value();
          int32_t color;
          memcpy(&color, &value, sizeof(int32_t));
          m_props[entry.first] = color;
        } else {
          m_props[entry.first] = colorNode->GetColor();
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

void PropsAnimatedNode::StartAnimations() {
  assert(m_useComposition);
  if (m_expressionAnimations.size()) {
    AnimationView view = GetAnimationView();
    if (view) {
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
          if (view.m_componentView) {
            auto visual =
                winrt::Microsoft::ReactNative::Composition::Experimental::CompositionContextHelper::InnerVisual(
                    view.m_componentView
                        .as<winrt::Microsoft::ReactNative::Composition::implementation::ComponentView>()
                        ->Visual());
            visual.RotationAxis(m_rotationAxis);
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

void PropsAnimatedNode::CommitProps() {}

PropsAnimatedNode::AnimationView PropsAnimatedNode::GetAnimationView() {
  if (auto fabricuiManager = FabricUIManager::FromProperties(m_context.Properties())) {
    auto componentView = fabricuiManager->GetViewRegistry().findComponentViewWithTag(
        static_cast<facebook::react::Tag>(m_connectedViewTag));
    if (componentView) {
      return {componentView};
    }
  }
  return {nullptr};
}

void PropsAnimatedNode::StartAnimation(
    const AnimationView &view,
    const comp::CompositionAnimation &animation) noexcept {
  if (view.m_componentView) {
    auto baseComponentView =
        view.m_componentView.as<winrt::Microsoft::ReactNative::Composition::implementation::ComponentView>();
    auto visual = winrt::Microsoft::ReactNative::Composition::Experimental::CompositionContextHelper::InnerVisual(
        baseComponentView->Visual());
    if (visual) {
      auto targetProp = animation.Target();
      if (targetProp == L"Rotation") {
        targetProp = L"RotationAngleInDegrees";
      } else if (targetProp == L"Transform") {
        baseComponentView->EnsureCenterPointPropertySet().StartAnimation(L"transform", animation);
        return;
      } else if (targetProp == L"Translation") {
        baseComponentView->EnsureTransformMatrixFacade();
        baseComponentView->EnsureCenterPointPropertySet().StartAnimation(L"translation", animation);

        return;
      }
      visual.StartAnimation(targetProp, animation);
    }
  }
}

comp::CompositionPropertySet PropsAnimatedNode::EnsureCenterPointPropertySet(const AnimationView &view) noexcept {
  if (view.m_componentView) {
    return view.m_componentView.as<winrt::Microsoft::ReactNative::Composition::implementation::ComponentView>()
        ->EnsureCenterPointPropertySet();
  }
  return nullptr;
}

} // namespace Microsoft::ReactNative
