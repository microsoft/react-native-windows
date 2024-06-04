// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <ReactPropertyBag.h>

namespace Microsoft::React {

struct IWebSocketModuleContentHandler;
struct IBlobPersistor;
struct IWebSocketModuleProxy;
struct IHttpModuleProxy;

namespace Networking {
struct IBlobResource;
}

const winrt::Microsoft::ReactNative::ReactPropertyId<
    winrt::Microsoft::ReactNative::ReactNonAbiValue<std::weak_ptr<IWebSocketModuleContentHandler>>>
BlobModuleContentHandlerPropertyId() noexcept;

const winrt::Microsoft::ReactNative::ReactPropertyId<
    winrt::Microsoft::ReactNative::ReactNonAbiValue<std::weak_ptr<IBlobPersistor>>>
BlobModulePersistorPropertyId() noexcept;

const winrt::Microsoft::ReactNative::ReactPropertyId<
    winrt::Microsoft::ReactNative::ReactNonAbiValue<std::weak_ptr<IWebSocketModuleProxy>>>
WebSocketModuleProxyPropertyId() noexcept;

const winrt::Microsoft::ReactNative::ReactPropertyId<
    winrt::Microsoft::ReactNative::ReactNonAbiValue<std::weak_ptr<IHttpModuleProxy>>>
HttpModuleProxyPropertyId() noexcept;

const winrt::Microsoft::ReactNative::ReactPropertyId<
    winrt::Microsoft::ReactNative::ReactNonAbiValue<std::weak_ptr<Networking::IBlobResource>>>
BlobResourcePropertyId() noexcept;

const winrt::Microsoft::ReactNative::ReactPropertyId<winrt::hstring> DefaultUserAgentPropertyId() noexcept;

} // namespace Microsoft::React
