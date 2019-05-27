// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "XamlView.h"
#include <memory>
#include <NativeModuleProvider.h>
#include "ViewManagerProvider.h"
#include <ReactWindowsCore/ReactWindowsAPI.h>

namespace react { namespace uwp {

struct IReactInstance;
struct IXamlRootView;

struct IReactInstanceCreator
{
  virtual std::shared_ptr<IReactInstance> getInstance() = 0;
  virtual void markAsNeedsReload() = 0;
};

using ReactInstanceCreator = std::shared_ptr<IReactInstanceCreator>;

REACTWINDOWS_API_(std::shared_ptr<IReactInstance>) CreateReactInstance(const std::shared_ptr<facebook::react::NativeModuleProvider>& moduleProvider,
                                                    const std::shared_ptr<ViewManagerProvider>& viewManagerProvider = nullptr);
REACTWINDOWS_API_(std::shared_ptr<IXamlRootView>) CreateReactRootView(XamlView parentView,
                                                   const wchar_t* pJsComponentName,
                                                   const ReactInstanceCreator& instanceCreator
                                                   );

// The method return the instance pointer directly, the caller own the lifetime
REACTWINDOWS_API_(IReactInstance*) UnSafeCreateReactInstance(const std::shared_ptr<facebook::react::NativeModuleProvider>& moduleProvider,
                                          const std::shared_ptr<ViewManagerProvider>& viewManagerProvider = nullptr);

REACTWINDOWS_API_(std::shared_ptr<facebook::react::MessageQueueThread>) CreateWorkerMessageQueue();

} }
