// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// pch.cpp
// Include the standard header and generate the precompiled header.
//

#include "pch.h"

namespace folly {
const char *const dynamic::TypeInfo<dynamic::ObjectImpl>::name = "object";
}
