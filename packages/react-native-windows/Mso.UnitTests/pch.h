// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#define NOGDI
#define NOMINMAX

#include <combaseapi.h>
#include <guiddef.h>
#include <intrin.h>
#include <unknwn.h>
#include <windows.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdarg>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <mutex>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#include "activeObject/activeObject.h"
#include "future/future.h"
#include "gtest/gtest.h"
#include "motifCpp/gTestAdapter.h"
#include "motifCpp/testCheck.h"
