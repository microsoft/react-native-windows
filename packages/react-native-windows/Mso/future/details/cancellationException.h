// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_FUTURE_DETAILS_CANCELLATIONEXCEPTION_H
#define MSO_FUTURE_DETAILS_CANCELLATIONEXCEPTION_H

#include <sal.h>
#include <exception>
#include <string>

namespace Mso::Async {

struct CancellationException : std::exception {
  explicit CancellationException(_In_z_ const char *message) noexcept : m_message(message) {}

  CancellationException() noexcept {}

  const char *what() const noexcept override {
    return m_message.c_str();
  }

 private:
  std::string m_message;
};

} // namespace Mso::Async

#endif // MSO_FUTURE_DETAILS_CANCELLATIONEXCEPTION_H
