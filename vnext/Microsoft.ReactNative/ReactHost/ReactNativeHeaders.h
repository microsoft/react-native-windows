// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#if !MS_TARGET_APPLE
// #include <msoFolly/MsoFolly.h>
#endif

#pragma warning(push)

#pragma warning(disable : 4068)
#pragma warning(disable : 4100)
#pragma warning(disable : 4324) // structure was padded due to alignment specifier

#pragma push_macro("ERROR")
#pragma push_macro("Check")
#pragma push_macro("_WIN32")

// Header files that produce the warnings above when included go here.

#undef ERROR
#undef Check

#if defined(MS_TARGET_APPLE) || (MS_TARGET_ANDROID)
#undef _WIN32
#else
// Following are Windows specific headers
#include <InstanceManager.h>
#endif

#include <folly/Json.h>

#pragma pop_macro("_WIN32")
#pragma pop_macro("Check")
#pragma pop_macro("ERROR")

#pragma warning(pop)
