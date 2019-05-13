#include "LayoutAnimation.h"

#include <exception>
#include <folly/dynamic.h>

namespace facebook { namespace react {

namespace {

constexpr auto c_defaultDelay = 0.0f;
constexpr auto c_defaultInitialVelocity = 0.0f;
constexpr auto c_defaultSpringDamping = 0.4f;

constexpr auto c_typeSz = "type";
constexpr auto c_propertySz = "property";

constexpr auto c_createSz = "create";
constexpr auto c_updateSz = "update";
constexpr auto c_deleteSz = "delete";

constexpr auto c_opacitySz = "opacity";
constexpr auto c_scaleXSz = "scaleX";
constexpr auto c_scaleXYSz = "scaleXY";
constexpr auto c_scaleYSz = "scaleY";

constexpr auto c_easeInSz = "easeIn";
constexpr auto c_easeInEaseOutSz = "easeInEaseOut";
constexpr auto c_easeOutSz = "easeOut";
constexpr auto c_linearSz = "linear";
constexpr auto c_springSz = "spring";
constexpr auto c_keyboardSz = "keyboard";

constexpr auto c_durationSz = "duration";
constexpr auto c_delaySz = "delay";

constexpr auto c_initialVelocitySz = "initialVelocity";
constexpr auto c_springDampingSz = "springDamping";

LayoutAnimation::AnimationType AnimationTypeFromConfig(folly::dynamic& config)
{
  if (config.isNull())
    return LayoutAnimation::AnimationType::None;

  auto type = config[c_typeSz];
  if (type == c_easeInSz)
    return LayoutAnimation::AnimationType::EaseIn;
  else if (type == c_easeInEaseOutSz)
    return LayoutAnimation::AnimationType::EaseInEaseOut;
  else if (type == c_easeOutSz)
    return LayoutAnimation::AnimationType::EaseOut;
  else if (type == c_linearSz)
    return LayoutAnimation::AnimationType::Linear;
  else if (type == c_springSz)
    return LayoutAnimation::AnimationType::Spring;
  else if (type == c_keyboardSz)
    return LayoutAnimation::AnimationType::Keyboard;
  else
  {
    throw std::exception("Unknown animation type requested for LayoutAnimations.");
  }
}

LayoutAnimation::AnimatableProperty AnimatablePropertyFromConfig(folly::dynamic& config)
{
  if (config.isNull())
    return LayoutAnimation::AnimatableProperty::None;

  auto animProp = config[c_propertySz];
  if (animProp.isNull())
    return LayoutAnimation::AnimatableProperty::None;
  else if (animProp == c_opacitySz)
    return LayoutAnimation::AnimatableProperty::Opacity;
  else if (animProp == c_scaleXSz)
    return LayoutAnimation::AnimatableProperty::ScaleX;
  else if (animProp == c_scaleYSz)
    return LayoutAnimation::AnimatableProperty::ScaleY;
  else if (animProp == c_scaleXYSz)
    return LayoutAnimation::AnimatableProperty::ScaleXY;
  else
  {
    throw std::exception("Unknown property used for LayoutAnimations.");
  }
}

void SetAnimationSpecificProperties(folly::dynamic&& config, LayoutAnimation::LayoutAnimationProperties& props)
{
  if (config.isNull())
  {
    throw std::exception("Cannot set animation-specific properties with null config.");
  }

  if (config[c_typeSz] == c_springSz)
  {
    props.springAnimationProperties.initialVelocity = (config.find(c_initialVelocitySz) != config.items().end()) ? static_cast<float>(config[c_initialVelocitySz].asDouble()) : c_defaultInitialVelocity;
    props.springAnimationProperties.springDamping = (config.find(c_springDampingSz) != config.items().end()) ? static_cast<float>(config[c_springDampingSz].asDouble()) : c_defaultSpringDamping;
  }
}

void SetAnimationProperties(folly::dynamic&& config, LayoutAnimation::LayoutAnimationProperties& props, float defaultDuration)
{
  if (config.isNull())
  {
    props.animationType = LayoutAnimation::AnimationType::None;
    return;
  }

  props.animationType = AnimationTypeFromConfig(config);
  props.animatedProp = AnimatablePropertyFromConfig(config);
  props.delay = (config.find(c_delaySz) != config.items().end()) ? static_cast<float>(config[c_delaySz].asDouble()) : c_defaultDelay;
  props.duration = (config.find(c_durationSz) != config.items().end()) ? static_cast<float>(config[c_durationSz].asDouble()) : defaultDuration;

  if (props.animationType == LayoutAnimation::AnimationType::Spring)
  {
    SetAnimationSpecificProperties(std::move(config), props);
  }
}

} // anonymous namespace

LayoutAnimation::LayoutAnimation(folly::dynamic config)
{
  if (config.isNull())
  {
    throw std::exception("Cannot construct LayoutAnimation with null config.");
  }

  auto duration = config[c_durationSz];

  if (duration.isNull())
  {
    throw std::exception("LayoutAnimation must include default duration.");
  }

  auto defaultDuration = static_cast<float>(duration.asDouble()); // duration provided in ms

  SetAnimationProperties(std::move(config[c_createSz]), m_props.createAnimationProps, defaultDuration);
  SetAnimationProperties(std::move(config[c_updateSz]), m_props.updateAnimationProps, defaultDuration);
  SetAnimationProperties(std::move(config[c_deleteSz]), m_props.deleteAnimationProps, defaultDuration);
}

}} // namespace facebook::react
