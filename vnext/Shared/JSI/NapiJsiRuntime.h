// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

// JSI
#include <js_native_ext_api.h>
#include <jsi/jsi.h>

// Standard Library
#include <memory>

namespace Microsoft::JSI {

///
// NodeApiJsiRuntime factory function.
// TODO: Rename as MakeNapiJsiRuntime once code is dropped from V8-JSI.
///
std::unique_ptr<facebook::jsi::Runtime> __cdecl MakeNodeApiJsiRuntime(napi_env env) noexcept;

} // namespace Microsoft::JSI
