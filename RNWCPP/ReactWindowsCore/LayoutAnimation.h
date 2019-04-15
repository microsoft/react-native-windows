// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace folly {

struct dynamic;

} // namespace folly

namespace facebook { namespace react {

class LayoutAnimation
{
public:

  enum class AnimationType
  {
    Decay,
    EaseIn,
    EaseInEaseOut,
    EaseOut,
    Keyboard,
    Linear,
    None,
    Spring,
  };

  enum class AnimatableProperty
  {
    None,
    Opacity,
    ScaleX,
    ScaleY,
    ScaleXY,
  };

  struct LayoutCreateAnimationProperties
  {
    AnimationType animationType;
    AnimatableProperty animatedProp;
  };

  struct LayoutUpdateAnimationProperties
  {
    AnimationType animationType;
  };

  struct LayoutDeleteAnimationProperties
  {
    AnimationType animationType;
    AnimatableProperty animatedProp;
  };

  struct LayoutAnimationProperties
  {
    float duration;
    LayoutCreateAnimationProperties createAnimationProps;
    LayoutDeleteAnimationProperties deleteAnimationProps;
    LayoutUpdateAnimationProperties updateAnimationProps;
  };

  LayoutAnimation(folly::dynamic&& config);
  const LayoutAnimationProperties& Properties()
  {
    return m_props;
  }
  AnimationType Type() const
  {
    return m_type;
  }

private:
  LayoutAnimationProperties m_props;
  AnimationType m_type;
};

}} // namespace facebook::react
