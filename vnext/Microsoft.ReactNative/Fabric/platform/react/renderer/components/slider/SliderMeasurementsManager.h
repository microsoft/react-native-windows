#pragma once

#include <react/renderer/core/ConcreteComponentDescriptor.h>
#include <react/renderer/core/LayoutConstraints.h>
#include <react/utils/ContextContainer.h>
#include <UI.Xaml.Controls.h>
#include <CppWinrtIncludes.h>

namespace facebook::react {

/**
 * Class that manages slider measurements across platforms.
 * On iOS it is a noop, since the height is passed in from JS on iOS only.
 */
class SliderMeasurementsManager {
 public:
  SliderMeasurementsManager(const ContextContainer::Shared &contextContainer) : m_contextContainer(contextContainer) {}

  static inline bool shouldMeasureSlider() {
    return true;
  }

  Size measure(SurfaceId surfaceId, LayoutConstraints layoutConstraints) const;

 private:
  ContextContainer::Shared m_contextContainer;
  mutable xaml::Controls::Slider m_slider{nullptr};
};

} // namespace facebook::react
