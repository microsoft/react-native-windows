// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.Foundation.h>
#include <ReactWindowsCore/ReactWindowsAPI.h>

namespace react {
namespace uwp {

  REACTWINDOWS_API_(bool) IsEmptyRect(winrt::Windows::Foundation::Rect rect);
  REACTWINDOWS_API_(winrt::Windows::Foundation::Rect) CreateEmptyRect();
  REACTWINDOWS_API_(winrt::Windows::Foundation::Rect) UnionRect(winrt::Windows::Foundation::Rect rect1, _In_ winrt::Windows::Foundation::Rect rect2);

  } // namespace uwp
} // namespace react
