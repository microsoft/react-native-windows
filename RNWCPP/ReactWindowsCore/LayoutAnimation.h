#pragma once

namespace folly {

struct dynamic;

} // namespace folly

namespace facebook { namespace react {

#pragma region Animation-specific properties

struct SpringAnimationProperties
{
	float springDamping;
  float initialVelocity;
};

#pragma endregion

class LayoutAnimation
{
public:

enum class AnimationType
{
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

struct LayoutAnimationProperties
{
  float duration = 0.0f;
  float delay = 0.0f;
  AnimationType animationType;
  AnimatableProperty animatedProp;

  union // Animation-specific properties
  {
    SpringAnimationProperties springAnimationProperties;
  };
};

struct LayoutAnimations
{
  LayoutAnimationProperties createAnimationProps;
  LayoutAnimationProperties deleteAnimationProps;
  LayoutAnimationProperties updateAnimationProps;
};

	LayoutAnimation(folly::dynamic config);
	const LayoutAnimations& Properties() { return m_props; }

private:
	LayoutAnimations m_props;
};

}} // namespace facebook::react
