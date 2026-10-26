// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

// #include <tom.h>
// #include <textserv.h>
struct ITextDocument2;
class ITextServices2;
class ITextHost;

namespace Microsoft::ReactNative::Composition::Experimental {

// This interface will be removed once we complete internal transitions to ContentIsland, and new APIs for hosting
// popups/new windows from a ContentIsland
struct __declspec(uuid("2A60D68A-57B8-43CA-B1BA-2FAF0FCB344B")) IComponentViewInterop : IUnknown {
  virtual HWND GetHwndForParenting() noexcept = 0;
};

struct __declspec(uuid("ABA4F071-1C26-4D56-A608-75361ED21545")) ITextInputComponentInterop : IUnknown {
  virtual HRESULT GetTextDocument(ITextDocument2 **textDocument) noexcept = 0;
  virtual HRESULT GetTextServices(ITextServices2 **textServices) noexcept = 0;
  virtual HRESULT GetTextHost(ITextHost **textHost) noexcept = 0;
};

} // namespace Microsoft::ReactNative::Composition::Experimental
