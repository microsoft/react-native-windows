// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "CppWinRTIncludes.h"

namespace Microsoft::ReactNative {

void TransferProperty(
    const xaml::DependencyObject &oldView,
    const xaml::DependencyObject &newView,
    xaml::DependencyProperty dp);

void TransferProperty(
    const xaml::DependencyObject &oldView,
    const xaml::DependencyObject &newView,
    xaml::DependencyProperty oldViewDP,
    xaml::DependencyProperty newViewDP);

void TransferFrameworkElementProperties(const xaml::DependencyObject &oldView, const xaml::DependencyObject &newView);

} // namespace Microsoft::ReactNative
