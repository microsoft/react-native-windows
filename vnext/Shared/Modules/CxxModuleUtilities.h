// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

// Folly
#include <folly/dynamic.h>

// React Native
#include <cxxreact/Instance.h>

// Standard Library
#include <memory>
#include <string>

namespace Microsoft::React::Modules {

void SendEvent(
    std::weak_ptr<facebook::react::Instance> weakReactInstance,
    std::string &&eventName,
    folly::dynamic &&args);

} // namespace Microsoft::React::Modules
