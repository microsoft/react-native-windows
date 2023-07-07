// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <IBlobPersistor.h>

// React Native Windows
#include <JSValue.h>

// Windows API
#include <winrt/Windows.Foundation.h>

// Standard Library
#include <functional>
#include <memory>
#include <string>

namespace Microsoft::React::Networking {

struct IBlobResource {
  struct BlobCallbacks {
    std::function<void(std::string &&errorText)> OnError;
  };

  struct BlobFieldNames {
    const char *Blob;
    const char *BlobId;
    const char *Offset;
    const char *Size;
    const char *Type;
    const char *Data;
  };

  static std::shared_ptr<IBlobResource> Make(winrt::Windows::Foundation::IInspectable const &inspectableProperties);

  static const BlobFieldNames &FieldNames() noexcept;

  virtual ~IBlobResource() noexcept {}

  virtual void SendOverSocket(std::string &&blobId, int64_t offset, int64_t size, int64_t socketId) noexcept = 0;

  virtual void CreateFromParts(winrt::Microsoft::ReactNative::JSValueArray &&parts, std::string &&blobId) noexcept = 0;

  virtual void Release(std::string &&blobId) noexcept = 0;

  virtual void AddNetworkingHandler() noexcept = 0;

  virtual void AddWebSocketHandler(int64_t id) noexcept = 0;

  virtual void RemoveWebSocketHandler(int64_t id) noexcept = 0;

  virtual BlobCallbacks &Callbacks() noexcept = 0;
};

} // namespace Microsoft::React::Networking
