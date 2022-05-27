// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/// <summary>
/// Sets a global boolean value identified by an arbitrary string.
/// </summary>
/// <param name="name">Global key</param>
void __cdecl MicrosoftReactSetRuntimeOptionBool(const char *name, bool value) noexcept;

/// <summary>
/// Sets a global signed integer value identified by an arbitrary string.
/// </summary>
/// <param name="name">Global boolean key</param>
void __cdecl MicrosoftReactSetRuntimeOptionInt(const char *name, int32_t value) noexcept;

/// <summary>
/// Sets a global signed integer value identified by an arbitrary string.
/// </summary>
/// <param name="name">Global string key</param>
void __cdecl MicrosoftReactSetRuntimeOptionString(const char *name, const char *value) noexcept;

/// <summary>
/// Retrieves a global boolean value for the given key.
/// </summary>
/// <param name="name">Global boolean key</param>
/// <returns>Value stored for the given key, or false if the entry doesn't exist (default)</returns>
bool __cdecl MicrosoftReactGetRuntimeOptionBool(const char *name) noexcept;

/// <summary>
/// Retrieves a global boolean value for the given key.
/// </summary>
/// <param name="name">Global key</param>
/// <returns>Value stored for the given key, or 0 if the entry doesn't exist (default)</returns>
int32_t __cdecl MicrosoftReactGetRuntimeOptionInt(const char *name) noexcept;

/// <param name="buffer">String contents. nullptr if none found</param>
/// <param name="length">String length. 0 if none found</param>
/// <param name="state">Pointer used to pass or retrieve arbitrary data</param>
typedef void(__cdecl *MicrosoftReactGetStringCallback)(const char *buffer, size_t length, void *state);

/// <summary>
/// Retrieves a global string value for the given key.
/// </summary>
/// <param name="name">Global key</param>
/// <param name="callBack">Handler used to access the obtained string</param>
/// <param name="state">Arbitrary data to pass on to or retrieve from callBack</param>
void __cdecl MicrosoftReactGetRuntimeOptionString(
    const char *name,
    MicrosoftReactGetStringCallback callBack,
    void *state);

#ifdef __cplusplus
}
#endif
