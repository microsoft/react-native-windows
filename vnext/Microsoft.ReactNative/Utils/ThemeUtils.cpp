// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <Utils/ThemeUtils.h>

#include <winuser.h>

namespace Microsoft::ReactNative {

bool IsColorLight(const winrt::Windows::UI::Color &clr) noexcept {
  return (((5 * clr.G) + (2 * clr.R) + clr.B) > (8 * 128));
}

bool IsInHighContrastWin32() noexcept {
  BOOL fResult;
  HIGHCONTRAST info;

  fResult = SystemParametersInfo(
      SPI_GETHIGHCONTRAST, // Get contrast information
      0, // Not used
      &info, // Holds contrast information
      0); // Not used

  // Double it.
  if (fResult) {
    return !!(info.dwFlags & HCF_HIGHCONTRASTON);
  }

  return false;
}

} // namespace Microsoft::ReactNative
