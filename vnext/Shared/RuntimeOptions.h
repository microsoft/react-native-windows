// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

/// <summary>
/// Sets a global boolean value identified by an arbitrary string.
/// </summary>
/// <param name="name">Global key</param>
void __cdecl Microsoft_React_SetRuntimeOptionBool(const char *name, bool value) noexcept;

/// <summary>
/// Sets a global signed integer value identified by an arbitrary string.
/// </summary>
/// <param name="name">Global boolean key</param>
void __cdecl Microsoft_React_SetRuntimeOptionInt(const char *name, int32_t value) noexcept;

/// <summary>
/// Sets a global signed integer value identified by an arbitrary string.
/// </summary>
/// <param name="name">Global string key</param>
void __cdecl Microsoft_React_SetRuntimeOptionString(const char *name, const char *value) noexcept;

/// <summary>
/// Retrieves a global boolean value for the given key.
/// </summary>
/// <param name="name">Global boolean key</param>
/// <returns>Value stored for the given key, or false if the entry doesn't exist (default)</returns>
const bool __cdecl Microsoft_React_GetRuntimeOptionBool(const char *name) noexcept;

/// <summary>
/// Retrieves a global boolean value for the given key.
/// </summary>
/// <param name="name">Global key</param>
/// <returns>Value stored for the given key, or 0 if the entry doesn't exist (default)</returns>
const int32_t __cdecl Microsoft_React_GetRuntimeOptionInt(const char *name) noexcept;

/// <param name="buffer">String contents. nullptr if none found</param>
/// <param name="length">String length. 0 if none found</param>
/// <param name="state">Pointer used to pass or retrieve arbitrary data</param>
typedef void(__cdecl *Microsoft_React_GetStringCallback)(const char *buffer, size_t length, void *state);

/// <summary>
/// Retrieves a global string value for the given key.
/// </summary>
/// <param name="name">Global key</param>
/// <param name="callBack">Handler used to access the obtained string</param>
/// <param name="state">Arbitrary data to pass on to or retrieve from callBack</param>
void Microsoft_React_GetRuntimeOptionString(const char *name, Microsoft_React_GetStringCallback callBack, void *state);
