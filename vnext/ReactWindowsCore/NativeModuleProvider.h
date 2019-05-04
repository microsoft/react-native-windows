// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// public header

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace facebook { namespace react { class MessageQueueThread;  } }
namespace facebook { namespace xplat { namespace module { class CxxModule; } } }

namespace facebook { namespace react {

using NativeModuleDescription = std::tuple<std::string,
                                           std::function<std::unique_ptr<facebook::xplat::module::CxxModule>()>, // CxxModule::Provider
                                           std::shared_ptr<facebook::react::MessageQueueThread>>;

struct NativeModuleProvider
{
  virtual std::vector<NativeModuleDescription> GetModules(const std::shared_ptr<facebook::react::MessageQueueThread>& defaultQueueThread) = 0;
};

} }
