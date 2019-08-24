// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <stdarg.h>
#include <stdio.h>

/// <summary>
/// Allocating swprintf which allocates the buffer with the given format and
/// parameters.
/// </summary>
int _aswprintf(wchar_t **ret, const wchar_t *format, ...);

/// <summary>
/// Allocating sprintf which allocates the buffer with the given format and
/// parameters.
/// </summary>
int _asprintf(char **ret, const char *format, ...);
