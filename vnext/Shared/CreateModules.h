// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

// Windows API
#include <winrt/Windows.Foundation.h>

// Standard Library
#include <memory>

// Forward declarations. Desktop projects can not access <React.h>
namespace winrt::Microsoft::ReactNative {
struct ReactContext;
struct ReactModuleProvider;
} // namespace winrt::Microsoft::ReactNative

namespace Mso::React {
struct IReactContext;
}

namespace Microsoft::React {

#pragma region TurboModules

extern const wchar_t *GetBlobTurboModuleName() noexcept;
extern const winrt::Microsoft::ReactNative::ReactModuleProvider &GetBlobModuleProvider() noexcept;

extern const wchar_t *GetHttpTurboModuleName() noexcept;
extern const winrt::Microsoft::ReactNative::ReactModuleProvider &GetHttpModuleProvider() noexcept;

extern const wchar_t *GetFileReaderTurboModuleName() noexcept;
extern const winrt::Microsoft::ReactNative::ReactModuleProvider &GetFileReaderModuleProvider() noexcept;

extern const wchar_t *GetWebSocketTurboModuleName() noexcept;
extern const winrt::Microsoft::ReactNative::ReactModuleProvider &GetWebSocketModuleProvider() noexcept;

#pragma endregion TurboModules

} // namespace Microsoft::React
