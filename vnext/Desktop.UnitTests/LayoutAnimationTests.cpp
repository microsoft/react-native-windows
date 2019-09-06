#include <CppUnitTest.h>
#include <LayoutAnimation.h>
#include <folly/dynamic.h>
#include <variant>

using namespace facebook::react;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace {

constexpr auto c_typeSz = "type";
constexpr auto c_propertySz = "property";
constexpr auto c_noneSz = "none";

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

} // namespace

namespace Microsoft::VisualStudio::CppUnitTestFramework {

template <>
std::wstring ToString<enum facebook::react::LayoutAnimation::AnimationType>(
    const enum facebook::react::LayoutAnimation::AnimationType &t) {
  switch (t) {
    case facebook::react::LayoutAnimation::AnimationType::Linear:
      return L"Linear";
    case facebook::react::LayoutAnimation::AnimationType::EaseIn:
      return L"EaseIn";
    case facebook::react::LayoutAnimation::AnimationType::EaseInEaseOut:
      return L"EaseInEaseOut";
    case facebook::react::LayoutAnimation::AnimationType::EaseOut:
      return L"EaseOut";
    case facebook::react::LayoutAnimation::AnimationType::Spring:
      return L"Spring";
    case facebook::react::LayoutAnimation::AnimationType::Keyboard:
      return L"Keyboard";
  }

  return L"None";
}

template <>
std::wstring
ToString<enum facebook::react::LayoutAnimation::AnimatableProperty>(
    const enum facebook::react::LayoutAnimation::AnimatableProperty &t) {
  switch (t) {
    case facebook::react::LayoutAnimation::AnimatableProperty::Opacity:
      return L"Opacity";
    case facebook::react::LayoutAnimation::AnimatableProperty::ScaleX:
      return L"ScaleX";
    case facebook::react::LayoutAnimation::AnimatableProperty::ScaleXY:
      return L"ScaleXY";
    case facebook::react::LayoutAnimation::AnimatableProperty::ScaleY:
      return L"ScaleY";
  }

  return L"None";
}

} // namespace Microsoft::VisualStudio::CppUnitTestFramework

namespace Microsoft::React::Test {

// We turn clang format off here because it does not work with some of the
// test macros.
// clang-format off

TEST_CLASS(LayoutAnimationTests) {

/*
  Typical layout animation:

  {"duration":500,"create":{"type":"linear","property":"opacity"},"update":{"type":"linear"},"delete":{"type":"linear","property":"opacity"}}
  {"duration":1000,"update":{"type":"linear"}}

  auto value = folly::dynamic::object("duration", 500)("create",
  folly::dynamic::object("type", "linear")("property", "opacity"))("update",
  folly::dynamic::object("type", "linear"))("delete",
  folly::dynamic::object("type", "linear")("property", "opacity")); auto value =
  folly::dynamic::object("duration", 1000)("update",
  folly::dynamic::object("type", "linear"));
*/

#pragma region LayoutAnimation Config Processing Tests

  TEST_METHOD(LayoutAnimationTests_BasicLayoutAnimationPropertySetTest){
    auto duration = 500.0f;
    auto delay = 0.0f;
    folly::dynamic layoutAnimation = folly::dynamic::object(c_durationSz, duration)(
        c_createSz,
        folly::dynamic::object(c_typeSz, c_linearSz)(c_propertySz, c_scaleXYSz))(
        c_updateSz,
        folly::dynamic::object(c_typeSz, c_easeInSz))(
        c_deleteSz,
        folly::dynamic::object(c_typeSz, c_easeOutSz)(c_propertySz, c_opacitySz));
    LayoutAnimation typedLayoutAnimation(layoutAnimation);
    auto typedProps = typedLayoutAnimation.Properties();

    Assert::AreEqual(duration, typedProps.createAnimationProps.duration);
    Assert::AreEqual(duration, typedProps.updateAnimationProps.duration);
    Assert::AreEqual(duration, typedProps.deleteAnimationProps.duration);
    Assert::AreEqual(
        LayoutAnimation::AnimationType::Linear,
        typedProps.createAnimationProps.animationType);
    Assert::AreEqual(
        LayoutAnimation::AnimationType::EaseIn,
        typedProps.updateAnimationProps.animationType);
    Assert::AreEqual(
        LayoutAnimation::AnimationType::EaseOut,
        typedProps.deleteAnimationProps.animationType);
    Assert::AreEqual(
        LayoutAnimation::AnimatableProperty::ScaleXY,
        typedProps.createAnimationProps.animatedProp);
    Assert::AreEqual(
        LayoutAnimation::AnimatableProperty::None,
        typedProps.updateAnimationProps.animatedProp);
    Assert::AreEqual(
        LayoutAnimation::AnimatableProperty::Opacity,
        typedProps.deleteAnimationProps.animatedProp);
    Assert::AreEqual(delay, typedProps.createAnimationProps.delay);
    Assert::AreEqual(delay, typedProps.updateAnimationProps.delay);
    Assert::AreEqual(delay, typedProps.deleteAnimationProps.delay);
  }

  TEST_METHOD(LayoutAnimationTests_OnlyAnimateCreateTest) {
    folly::dynamic layoutAnimation = folly::dynamic::object(c_durationSz, 500)(
        c_createSz,
        folly::dynamic::object(c_typeSz, c_linearSz)(c_propertySz, c_scaleXYSz));
    LayoutAnimation typedLayoutAnimation(layoutAnimation);
    auto typedProps = typedLayoutAnimation.Properties();

    Assert::AreEqual(
        LayoutAnimation::AnimationType::Linear,
        typedProps.createAnimationProps.animationType);
    Assert::AreEqual(
        LayoutAnimation::AnimationType::None,
        typedProps.updateAnimationProps.animationType);
    Assert::AreEqual(
        LayoutAnimation::AnimationType::None,
        typedProps.deleteAnimationProps.animationType);
  }

  TEST_METHOD(LayoutAnimationTests_OnlyAnimateUpdateTest) {
    folly::dynamic layoutAnimation = folly::dynamic::object(c_durationSz, 500)(
        c_updateSz,
        folly::dynamic::object(c_typeSz, c_linearSz)(c_propertySz, c_scaleXYSz));
    LayoutAnimation typedLayoutAnimation(layoutAnimation);
    auto typedProps = typedLayoutAnimation.Properties();

    Assert::AreEqual(
        LayoutAnimation::AnimationType::None,
        typedProps.createAnimationProps.animationType);
    Assert::AreEqual(
        LayoutAnimation::AnimationType::Linear,
        typedProps.updateAnimationProps.animationType);
    Assert::AreEqual(
        LayoutAnimation::AnimationType::None,
        typedProps.deleteAnimationProps.animationType);
  }

  TEST_METHOD(LayoutAnimationTests_OnlyAnimateDeleteTest) {
    folly::dynamic layoutAnimation = folly::dynamic::object(c_durationSz, 500)(
        c_deleteSz,
        folly::dynamic::object(c_typeSz, c_linearSz)(c_propertySz, c_scaleXYSz));
    LayoutAnimation typedLayoutAnimation(layoutAnimation);
    auto typedProps = typedLayoutAnimation.Properties();

    Assert::AreEqual(
        LayoutAnimation::AnimationType::None,
        typedProps.createAnimationProps.animationType);
    Assert::AreEqual(
        LayoutAnimation::AnimationType::None,
        typedProps.updateAnimationProps.animationType);
    Assert::AreEqual(
        LayoutAnimation::AnimationType::Linear,
        typedProps.deleteAnimationProps.animationType);
  }

  TEST_METHOD(LayoutAnimationTests_CustomizedDurationTest) {
    auto defaultDuration = 500.0f;
    auto customDuration = 50.0f;
    folly::dynamic layoutAnimation = folly::dynamic::object(
        c_durationSz, defaultDuration)(
        c_createSz,
        folly::dynamic::object(c_typeSz, c_linearSz)(c_propertySz, c_scaleXYSz))(
        c_updateSz,
        folly::dynamic::object(c_durationSz, customDuration)(
            c_typeSz, c_easeInSz));
    LayoutAnimation typedLayoutAnimation(layoutAnimation);
    auto typedProps = typedLayoutAnimation.Properties();

    Assert::AreEqual(defaultDuration, typedProps.createAnimationProps.duration);
    Assert::AreEqual(customDuration, typedProps.updateAnimationProps.duration);
  }

  TEST_METHOD(LayoutAnimationTests_MultipleDelaysTest) {
    auto createDelay = 50.0f;
    auto updateDelay = 150.0f;
    auto deleteDelay = 250.0f;
    folly::dynamic layoutAnimation = folly::dynamic::object(c_durationSz, 500)(
        c_createSz,
        folly::dynamic::object(c_delaySz, createDelay)(c_typeSz, c_linearSz)(
            c_propertySz, c_scaleXYSz))(
        c_updateSz,
        folly::dynamic::object(c_delaySz, updateDelay)(c_typeSz, c_easeInSz))(
        c_deleteSz,
        folly::dynamic::object(c_delaySz, deleteDelay)(c_typeSz, c_easeOutSz)(
            c_propertySz, c_opacitySz));
    LayoutAnimation typedLayoutAnimation(layoutAnimation);
    auto typedProps = typedLayoutAnimation.Properties();

    Assert::AreEqual(createDelay, typedProps.createAnimationProps.delay);
    Assert::AreEqual(updateDelay, typedProps.updateAnimationProps.delay);
    Assert::AreEqual(deleteDelay, typedProps.deleteAnimationProps.delay);
  }

  TEST_METHOD(LayoutAnimationTests_SpringPropertiesTest) {
    auto damping = 0.7f;
    auto initialVelocity = 1.0f;
    folly::dynamic layoutAnimation = folly::dynamic::object(c_durationSz, 500)(
        c_createSz,
        folly::dynamic::object(c_typeSz, c_springSz)(c_springDampingSz, damping)(
            c_initialVelocitySz, initialVelocity)(c_propertySz, c_scaleXYSz));
    LayoutAnimation typedLayoutAnimation(layoutAnimation);
    auto typedProps = typedLayoutAnimation.Properties();

    Assert::AreEqual(
        LayoutAnimation::AnimationType::Spring,
        typedProps.createAnimationProps.animationType);
    Assert::AreEqual(
        damping,
        typedProps.createAnimationProps.springAnimationProperties.springDamping);
    Assert::AreEqual(
        initialVelocity,
        typedProps.createAnimationProps.springAnimationProperties
            .initialVelocity);
  }

#pragma endregion
};

} // namespace Microsoft::React::Test

// clang-format on
