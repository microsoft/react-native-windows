
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <dwrite.h>

namespace Microsoft::ReactNative {

winrt::com_ptr<::IDWriteFactory> DWriteFactory() noexcept;

// Font collection that merges the system font set with every font file bundled in the
// application's Assets\ and Assets\Fonts\ directories (*.ttf / *.otf), so app-bundled
// font families resolve during text layout exactly like installed fonts. Returns
// nullptr when the app bundles no fonts or when the collection cannot be built;
// callers should treat nullptr as "use the system font collection".
//
// The collection - including the directory enumeration used to find the bundled font
// files - is built exactly once per process, on first use, and is then owned for the
// lifetime of the process. Initialization is thread-safe: concurrent first callers
// resolve to the same instance.
//
// Returns a NON-OWNING raw pointer on purpose. GetTextLayout() calls this on every
// text measure, so handing back a com_ptr by value would put an AddRef/Release pair
// on that path for a pointer whose lifetime is already static. Callers must not
// release it; take a com_ptr copy if they need to extend a reference.
::IDWriteFontCollection *DWriteAppFontCollection() noexcept;

} // namespace Microsoft::ReactNative
