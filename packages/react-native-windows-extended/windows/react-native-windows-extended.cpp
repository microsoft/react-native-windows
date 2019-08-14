#include "pch.h"
#include "react-native-windows-extended.h"
#include <ViewManager.h>
#include "CustomViewManager.h"

namespace react_native_windows_extended {

std::unique_ptr<facebook::react::IViewManager> CreateCustomViewManager(
    const std::shared_ptr<react::uwp::IReactInstance> &instance) {
  return std::make_unique<CustomFrameworkElementViewManager>(instance);
}

} // namespace react_native_windows_extended
