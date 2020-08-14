// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <string>

namespace Microsoft::React {

/// <summary>
/// Sets a global boolean value identified by an arbitrary string.
/// </summary>
/// <param name="name">Global key</param>
void __cdecl SetRuntimeOptionBool(std::string &&name, bool value) noexcept;

/// <summary>
/// Sets a global signed integer value identified by an arbitrary string.
/// </summary>
/// <param name="name">Global boolean key</param>
void __cdecl SetRuntimeOptionInt(std::string &&name, std::int32_t value) noexcept;

/// <summary>
/// Retrieves a global boolean value for the given key.
/// </summary>
/// <param name="name">Global boolean key</param>
/// <returns>Value stored for the given key, or false if the entry doesn't exist (default)</returns>
const bool __cdecl GetRuntimeOptionBool(const std::string &name) noexcept;

/// <summary>
/// Retrieves a global boolean value for the given key.
/// </summary>
/// <param name="name">Global key</param>
/// <returns>Value stored for the given key, or 0 if the entry doesn't exist (default)</returns>
const std::int32_t __cdecl GetRuntimeOptionInt(const std::string &name) noexcept;

} // namespace Microsoft::React
