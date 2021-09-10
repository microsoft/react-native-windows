// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

// JSI
#include <jsi/jsi.h>
#include <js_native_api_types.h>

// Standard Library
#include <memory>

namespace Microsoft::JSI {

///
// NapiJsiRuntime factory function.
///
std::unique_ptr<facebook::jsi::Runtime> __cdecl MakeNapiJsiRuntime2(napi_env env) noexcept;

}
