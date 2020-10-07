// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <React.h>
#include <cxxreact/CxxModule.h>
#include <smartPtr/cntPtr.h>

#include <memory>

namespace facebook {
namespace react {

// This method is to create a unique_ptr of native timing module.
// @param A MessageQueueThread on which this native module lives.
// @return Native timing module
std::unique_ptr<facebook::xplat::module::CxxModule> CreateTimingModule(
    const std::shared_ptr<facebook::react::MessageQueueThread> &nativeThread) noexcept;

} // namespace react
} // namespace facebook

namespace Microsoft::React {

// This method is to create a unique_ptr of a native WebSocket module.
// @param An IReactContext smart pointer to provide runtime configuration values.
// @return Native WebSocket module.
std::unique_ptr<facebook::xplat::module::CxxModule> CreateWebSocketModule(
    Mso::CntPtr<Mso::React::IReactContext> &&context) noexcept;

} // namespace Microsoft::React
