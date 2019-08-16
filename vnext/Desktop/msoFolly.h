// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// This file is a helper to allow devmain code to include folly
// TODO: This file should eventually move to devmain
//#ifndef FBXPLATMODULE
//#define FBXPLATMODULE

#include <iostream>
#pragma warning(push)
#pragma warning(disable : 4995)
#pragma warning(disable : 4068)
#pragma warning(disable : 4146)
#pragma warning(disable : 4100)
#pragma warning( \
    disable : 4324) // structure was padded due to alignment specifier
#pragma warning(disable : 4643) // Forward declaring 'T' in namespace std is not
                                // permitted by the C++ Standard.
#pragma push_macro("CHECK")
#pragma push_macro("Check")
#pragma push_macro("max")
#pragma push_macro("min")
#pragma push_macro("OUT")
#pragma push_macro("IN")
#undef Check
#undef CHECK
#undef max
#undef min
#undef OUT
#undef IN
#define FOLLY_MOBILE 1
#include <folly/dynamic.h>
#pragma pop_macro("IN")
#pragma pop_macro("OUT")
#pragma pop_macro("min")
#pragma pop_macro("max")
#pragma pop_macro("Check")
#pragma pop_macro("CHECK")
#pragma warning(pop)

//#endif
