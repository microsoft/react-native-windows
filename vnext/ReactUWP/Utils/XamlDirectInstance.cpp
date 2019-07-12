// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <winrt/Windows.UI.Xaml.h>
#include "Utils/XamlDirectInstance.h"

namespace XD {
  using namespace winrt::Windows::UI::Xaml::Core::Direct;
}

XD::IXamlDirect XamlDirectInstance::m_xamlDirect = NULL;

XD::IXamlDirect XamlDirectInstance::GetXamlDirect() {
  if (m_xamlDirect == NULL) {
    m_xamlDirect = XD::XamlDirect::GetDefault();
  }
  return m_xamlDirect;
}

