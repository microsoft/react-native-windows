#pragma once

#include <memory>
#include <ViewManager.h>
#include <IReactInstance.h>

namespace react_native_windows_extended {

std::unique_ptr<facebook::react::IViewManager> CreateCustomViewManager(
    const std::shared_ptr<react::uwp::IReactInstance> &instance);

}
