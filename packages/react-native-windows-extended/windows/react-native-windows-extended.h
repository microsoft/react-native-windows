#pragma once

#include <IReactInstance.h>
#include <ViewManager.h>
#include <memory>

namespace react_native_windows_extended {

std::unique_ptr<facebook::react::IViewManager> CreateCustomViewManager(
    const std::shared_ptr<react::uwp::IReactInstance> &instance);

}
