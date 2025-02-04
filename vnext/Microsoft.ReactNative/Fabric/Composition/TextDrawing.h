
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Fabric/Composition/Theme.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <react/renderer/attributedstring/AttributedString.h>
#include <winrt/Windows.UI.Composition.h>
#include "CompositionHelpers.h"

namespace winrt::Microsoft::ReactNative::Composition {

void RenderText(
    ID2D1RenderTarget &deviceContext,
    ::IDWriteTextLayout &textLayout,
    const facebook::react::AttributedString &attributedString,
    const facebook::react::TextAttributes &textAttributes,
    facebook::react::Point offset,
    float pointScaleFactor,
    winrt::Microsoft::ReactNative::Composition::implementation::Theme &theme) noexcept;

} // namespace winrt::Microsoft::ReactNative::Composition
