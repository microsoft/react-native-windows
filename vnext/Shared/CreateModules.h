// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

// React Native
#include <cxxreact/CxxModule.h>
#include <smartPtr/cntPtr.h>

// Windows API
#include <winrt/Windows.Foundation.h>

// Standard Library
#include <memory>

// Forward declarations. Desktop projects can not access <React.h>
namespace Mso::React {
struct IReactContext;
}

namespace facebook {
namespace react {

class MessageQueueThread;

// This method is to create a unique_ptr of native timing module.
// @param A MessageQueueThread on which this native module lives.
// @return Native timing module
extern std::unique_ptr<facebook::xplat::module::CxxModule> CreateTimingModule(
    const std::shared_ptr<facebook::react::MessageQueueThread> &nativeThread) noexcept;

} // namespace react
} // namespace facebook

namespace Microsoft::React {

extern const char *GetHttpModuleName() noexcept;
extern std::unique_ptr<facebook::xplat::module::CxxModule> CreateHttpModule(
    winrt::Windows::Foundation::IInspectable const &inspectableProperties) noexcept;

extern const char *GetWebSocketModuleName() noexcept;
extern std::unique_ptr<facebook::xplat::module::CxxModule> CreateWebSocketModule(
    winrt::Windows::Foundation::IInspectable const &inspectableProperties) noexcept;

extern const char *GetBlobModuleName() noexcept;
extern std::unique_ptr<facebook::xplat::module::CxxModule> CreateBlobModule(
    winrt::Windows::Foundation::IInspectable const &inspectableProperties) noexcept;

extern const char *GetFileReaderModuleName() noexcept;
extern std::unique_ptr<facebook::xplat::module::CxxModule> CreateFileReaderModule(
    winrt::Windows::Foundation::IInspectable const &inspectableProperties) noexcept;

} // namespace Microsoft::React
