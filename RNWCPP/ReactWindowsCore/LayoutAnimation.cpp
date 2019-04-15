// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "LayoutAnimation.h"

#include <exception>
#include <folly/dynamic.h>

using namespace facebook::react;

namespace {

LayoutAnimation::AnimationType AnimationConfigType(folly::dynamic& config)
{
  if (config.isNull())
    return LayoutAnimation::AnimationType::None;

  auto type = config["type"];
  if (type == "decay")
    return LayoutAnimation::AnimationType::Decay;
  else if (type == "easeIn")
    return LayoutAnimation::AnimationType::EaseIn;
  else if (type == "easeInEaseOut")
    return LayoutAnimation::AnimationType::EaseInEaseOut;
  else if (type == "easeOut")
    return LayoutAnimation::AnimationType::EaseOut;
  else if (type == "linear")
    return LayoutAnimation::AnimationType::Linear;
  else if (type == "spring")
    return LayoutAnimation::AnimationType::Spring;
  else
  {
    throw std::exception("Unknown animation type requested for LayoutAnimations.");
  }
}

LayoutAnimation::AnimatableProperty AnimatablePropertyFromConfig(folly::dynamic& config)
{
  if (config.isNull())
  {
    return LayoutAnimation::AnimatableProperty::None;
  }

  auto animProp = config["property"];

  if (animProp.isNull())
  {
    return LayoutAnimation::AnimatableProperty::None;
  }
  else if (animProp == "opacity")
  {
    return LayoutAnimation::AnimatableProperty::Opacity;
  }
  else
  {
    throw std::exception("Unknown property used for LayoutAnimations.");
  }
}

}

namespace facebook { namespace react {

LayoutAnimation::LayoutAnimation(folly::dynamic&& config)
{
  m_props.duration = static_cast<float>(config["duration"].asDouble() / 1000); // duration provided in ms; converting to seconds
  m_props.createAnimationProps.animationType = AnimationConfigType(config["create"]);
  m_props.deleteAnimationProps.animationType = AnimationConfigType(config["delete"]);
  m_props.updateAnimationProps.animationType = AnimationConfigType(config["update"]);
}

}} // namespace facebook::react
