// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "NapiJsiRuntime.h"
#include "NapiJsiRuntime_detail.h"

using namespace facebook::jsi;

using std::shared_ptr;
using std::string;
using std::unique_ptr;

namespace Microsoft::JSI {

unique_ptr<Runtime> __cdecl MakeNapiJsiRuntime2(napi_env env) noexcept {
  return nullptr;
}
}
