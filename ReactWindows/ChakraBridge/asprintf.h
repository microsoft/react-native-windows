#pragma once

#include <stdio.h>
#include <stdarg.h>

/// <summary>
/// Allocating swprintf which allocates the buffer with the given format and parameters.
/// </summary>
int _aswprintf(wchar_t **ret, const wchar_t *format, ...);

/// <summary>
/// Allocating sprintf which allocates the buffer with the given format and parameters.
/// </summary>
int _asprintf(char **ret, const char *format, ...);
