// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <winrt/react.uwp.h>

namespace facebook::react {
class Instance;
}

namespace react::uwp {
extern std::shared_ptr<facebook::react::Instance> __cdecl GetReactInstanceFromUwpInstance(
    const winrt::react::uwp::Instance &uwpInstance);
}
