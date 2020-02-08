// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactPromise.h"
#include "JSValueWriter.h"

namespace winrt::Microsoft::ReactNative {

static const char *ErrorDefaultCode = "EUNSPECIFIED";
static const char *ErrorDefaultMessage = "Error not specified.";

// Keys for m_reject's Error object
static const char *ErrorMapKeyCode = "code";
static const char *ErrorMapKeyMessage = "message";
static const char *ErrorMapKeyUserInfo = "userInfo";

ReactPromiseBase::ReactPromiseBase(
    IJSValueWriter const &writer,
    MethodResultCallback const &resolve,
    MethodResultCallback const &reject) noexcept
    : m_writer{writer}, m_resolve{resolve}, m_reject{reject} {}

// Reject the ReactPromise and report an error.
void ReactPromiseBase::Reject(ReactError &&error) noexcept {
  if (m_reject == nullptr) {
    Clear();
    return;
  }

  JSValueObject errorInfo;
  errorInfo.emplace(ErrorMapKeyCode, !error.Code.empty() ? error.Code : ErrorDefaultCode);
  errorInfo.emplace(ErrorMapKeyMessage, !error.Message.empty() ? error.Message : ErrorDefaultMessage);

  // For consistency with iOS ensure userInfo key exists, even if we null it.
  // iOS: /React/Base/RCTUtils.m -> RCTJSErrorFromCodeMessageAndNSError
  errorInfo.emplace(ErrorMapKeyUserInfo, !error.UserInfo.empty() ? JSValue(std::move(error.UserInfo)) : nullptr);

  WriteArgs(m_writer, errorInfo);
  m_reject(m_writer);
  Clear();
}

void ReactPromiseBase::Reject(const char *errorMessage) noexcept {
  if (m_reject == nullptr) {
    Clear();
    return;
  }

  m_writer.WriteArrayBegin();
  m_writer.WriteObjectBegin();

  WriteProperty(m_writer, ErrorMapKeyCode, ErrorDefaultCode);
  WriteProperty(m_writer, ErrorMapKeyMessage, errorMessage);

  // For consistency with iOS ensure userInfo key exists, even if we null it.
  // iOS: /React/Base/RCTUtils.m -> RCTJSErrorFromCodeMessageAndNSError
  WriteProperty(m_writer, ErrorMapKeyUserInfo, nullptr);

  m_writer.WriteObjectEnd();
  m_writer.WriteArrayEnd();
  m_reject(m_writer);
  Clear();
}

void ReactPromiseBase::Reject(const wchar_t *errorMessage) noexcept {
  if (m_reject == nullptr) {
    Clear();
    return;
  }

  m_writer.WriteArrayBegin();
  m_writer.WriteObjectBegin();

  WriteProperty(m_writer, ErrorMapKeyCode, ErrorDefaultCode);
  WriteProperty(m_writer, ErrorMapKeyMessage, errorMessage);

  // For consistency with iOS ensure userInfo key exists, even if we null it.
  // iOS: /React/Base/RCTUtils.m -> RCTJSErrorFromCodeMessageAndNSError
  WriteProperty(m_writer, ErrorMapKeyUserInfo, nullptr);

  m_writer.WriteObjectEnd();
  m_writer.WriteArrayEnd();
  m_reject(m_writer);
  Clear();
}

void ReactPromiseBase::Clear() noexcept {
  m_resolve = nullptr;
  m_reject = nullptr;
  m_writer = nullptr;
}

// Successfully resolve the ReactPromise<void>.
void ReactPromise<void>::Resolve() noexcept {
  if (m_resolve) {
    WriteArgs(m_writer, nullptr);
    m_resolve(m_writer);
    Clear();
  }
}

} // namespace winrt::Microsoft::ReactNative
