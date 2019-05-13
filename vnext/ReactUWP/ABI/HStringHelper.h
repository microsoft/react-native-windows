// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <wrl.h>
#include <string>

namespace ABI {
namespace react {
namespace uwp {

std::string HSTRINGToString(HSTRING hstring);
HSTRING StringToHSTRING(const std::string& str);

}
}
}
