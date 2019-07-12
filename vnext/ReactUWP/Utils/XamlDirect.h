// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.UI.Xaml.h>
#include "winrt/Microsoft.UI.Xaml.Core.Direct.h"

typedef winrt::Microsoft::UI::Xaml::Core::Direct::XamlEventIndex XDEventIndex;
typedef winrt::Microsoft::UI::Xaml::Core::Direct::XamlPropertyIndex
    XDPropertyIndex;
typedef winrt::Microsoft::UI::Xaml::Core::Direct::XamlTypeIndex XDTypeIndex;

static winrt::Microsoft::UI::Xaml::Core::Direct::IXamlDirect GetXamlDirect() {
  thread_local static winrt::Microsoft::UI::Xaml::Core::Direct::IXamlDirect
      xamlDirect;
  if (xamlDirect == nullptr) {
    xamlDirect =
        winrt::Microsoft::UI::Xaml::Core::Direct::XamlDirect::GetDefault();
  }
  return xamlDirect;
}
