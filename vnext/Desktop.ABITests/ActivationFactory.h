// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "pch.h"
#include <Windows.h>
#include <winrt/base.h>

extern int32_t __stdcall WINRT_RoGetActivationFactory(void *classId, winrt::guid const &iid, void **factory) noexcept;
