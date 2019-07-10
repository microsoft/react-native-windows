// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <Utils\XamlDirectInstance.h>
#include "pch.h"

XD::IXamlDirect XamlDirectInstance::m_xamlDirectInstance = NULL;

XD::IXamlDirect XamlDirectInstance::GetInstance() {
  if (m_xamlDirectInstance == NULL) {
    m_xamlDirectInstance = XD::XamlDirect::GetDefault();
  }
  return m_xamlDirectInstance;
}
