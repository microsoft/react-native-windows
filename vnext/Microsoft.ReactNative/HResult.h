#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

namespace winrt {
inline constexpr hresult error_null_argument{static_cast<hresult>(0x80004003)}; // ArgumentNullException
inline constexpr hresult error_invalid_operation{static_cast<hresult>(0x80131509)}; // InvalidOperationException

struct hresult_null_argument : hresult_error {
  hresult_null_argument() noexcept : hresult_error(error_null_argument) {}
  hresult_null_argument(param::hstring const &message) noexcept : hresult_error(error_null_argument, message) {}
  hresult_null_argument(take_ownership_from_abi_t) noexcept
      : hresult_error(error_null_argument, take_ownership_from_abi) {}
};

struct hresult_invalid_operation : hresult_error {
  hresult_invalid_operation() noexcept : hresult_error(error_invalid_operation) {}
  hresult_invalid_operation(param::hstring const &message) noexcept : hresult_error(error_invalid_operation, message) {}
  hresult_invalid_operation(take_ownership_from_abi_t) noexcept
      : hresult_error(error_invalid_operation, take_ownership_from_abi) {}
};

} // namespace winrt
