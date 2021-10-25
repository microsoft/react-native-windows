// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <hermes/hermes.h>

//! We do not package hermes.dll for projects that do not require it. We cannot
//! use pure delay-loading to achieve this, since WACK will detect the
//! non-present DLL. Functions in this namespace shim to the Hermes DLL via
//! GetProcAddress.
namespace Microsoft::ReactNative::HermesShim {

std::unique_ptr<facebook::hermes::HermesRuntime> makeHermesRuntime(const hermes::vm::RuntimeConfig &runtimeConfig);
void enableSamplingProfiler();
void disableSamplingProfiler();
void dumpSampledTraceToFile(const std::string &fileName);

} // namespace Microsoft::ReactNative::HermesShim
