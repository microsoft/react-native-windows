// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "JSBundle.h"

namespace Mso::React {

LIBLET_PUBLICAPI Mso::CntPtr<IJSBundle> MakeFileJSBundle(std::string && /*id*/, std::string && /*fileName*/) noexcept {
  // TODO: implement
  return {nullptr};
}

} // namespace Mso::React
