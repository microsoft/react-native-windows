// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

// We include all Folly header files here to be able to disable their warnings in one place.

#pragma warning(push)
#pragma warning(disable : 4100) // Line 109: 'requested_digits': unreferenced formal parameter
#include <double-conversion/double-conversion.h>
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable : 4324) // 'folly::max_align_t': structure was padded due to alignment specifier
#include <folly/lang/Align.h>
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable : 4127) // conditional expression is constant
#include <folly/container/detail/F14Table.h>
#pragma warning(pop)

#include <folly/Memory.h>
#include <folly/Optional.h>
#include <folly/dynamic.h>
#include <folly/json.h>
#include <folly/json/dynamic.h>
