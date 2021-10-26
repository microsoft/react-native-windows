// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <winrt/Microsoft.ReactNative.h>
#include "CppWinRTIncludes.h"

namespace Microsoft::ReactNative {

using XamlView = xaml::DependencyObject;

const int64_t INVALID_TAG = -1;

inline int64_t GetTag(XamlView view) {
  return winrt::Microsoft::ReactNative::XamlUIService::GetReactTag(view);
}

inline void SetTag(XamlView view, int64_t tag) {
  winrt::Microsoft::ReactNative::XamlUIService::SetReactTag(view, tag);
}

inline bool IsValidTag(int64_t value) {
  return value != INVALID_TAG;
}

inline bool TryGetTag(XamlView view, int64_t &value) {
  int64_t tag = GetTag(view);
  if (IsValidTag(tag)) {
    value = tag;
    return true;
  }
  return false;
}

// Walk up the XAML element tree to find the closest React element
// Searches itself and its parent to get a valid XamlView.
bool TryGetClosestTag(XamlView view, int64_t &value);

xaml::XamlRoot TryGetXamlRoot(const XamlView &view);
comp::Compositor GetCompositor(const XamlView &view);
void SetCompositor(const comp::Compositor &compositor);
comp::Compositor GetCompositor();

} // namespace Microsoft::ReactNative
