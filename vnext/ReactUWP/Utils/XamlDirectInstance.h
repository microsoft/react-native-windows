// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.UI.Xaml.Core.Direct.h>

namespace XD {
using namespace winrt::Windows::UI::Xaml::Core::Direct;
}

class XamlDirectInstance {
 public:
  static XD::IXamlDirect GetInstance();

 private:
  static XD::IXamlDirect m_xamlDirectInstance;
  XamlDirectInstance();
};
