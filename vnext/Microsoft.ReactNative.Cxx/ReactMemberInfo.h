// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#ifndef MICROSOFT_REACTNATIVE_REACTMEMBERINFO
#define MICROSOFT_REACTNATIVE_REACTMEMBERINFO

#include "winrt/Microsoft.ReactNative.h"

namespace winrt::Microsoft::ReactNative {

template <int I>
using ReactMemberId = std::integral_constant<int, I>;

} // namespace winrt::Microsoft::ReactNative

#endif // MICROSOFT_REACTNATIVE_REACTMEMBERINFO
