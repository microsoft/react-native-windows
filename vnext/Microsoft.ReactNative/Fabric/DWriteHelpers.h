
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <dwrite.h>

namespace Microsoft::ReactNative {

winrt::com_ptr<::IDWriteFactory> DWriteFactory() noexcept;

// Font collection that merges the system font set with every font file bundled in the
// application's Assets\ and Assets\Fonts\ directories (*.ttf / *.otf), so app-bundled
// font families resolve during text layout exactly like installed fonts. Built once on
// first use. Returns nullptr when the app bundles no fonts or when the collection
// cannot be built; callers should treat nullptr as "use the system font collection".
winrt::com_ptr<::IDWriteFontCollection> DWriteAppFontCollection() noexcept;

} // namespace Microsoft::ReactNative
