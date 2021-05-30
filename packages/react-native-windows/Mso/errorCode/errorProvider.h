// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_ERRORCODE_ERRORPROVIDER_H
#define MSO_ERRORCODE_ERRORPROVIDER_H

#include "errorCode/errorCode.h"
#include "errorCode/maybe.h"
#include "guid/msoGuid.h"
#include "memoryApi/memoryApi.h"

#if MS_TARGET_APPLE // VSO#2909557
#pragma pack(push, 8)
#else
#pragma pack(push, _CRT_PACKING)
#endif

namespace Mso {

// Default implementation of IErrorProvider for error data T
template <class T, class GuidToken>
class ErrorProvider final : public IErrorProvider {
 public:
  using ErrorInfoType = T;

  ErrorCode MakeErrorCode(T &&errorInfo) const noexcept;
  ErrorCode MakeErrorCode(const T &errorInfo) const noexcept;

  template <class TValue, class... TArgs>
  Maybe<TValue> MakeMaybe(TArgs &&... errorArgs) const noexcept;

  // TODO: deprecate this method in favor of TryGetErrorInfo
  bool IsOwnedErrorCode(const ErrorCode &errorCode) const noexcept;
  // TODO: deprecate this method in favor of TryGetErrorInfo
  const T &GetErrorInfo(const ErrorCode &errorCode) const noexcept;

  const T *TryGetErrorInfo(const ErrorCode &errorCode, bool shouldHandle = false) const noexcept;

 private: // IErrorProvider implementation. They must be private because these methods are not exported.
  bool IsOfType(const GUID &type) const noexcept override;
  void Destroy(const ErrorCodeState &state) const noexcept override;

  // By default we throw ErrorCodeException. Provide a specialization for this method to throw a different exception.
  void Throw(const ErrorCode &errorCode, bool shouldHandle) const override;

  // By default returns generic error message. Provide a specialization for this method to return specific message.
  Mso::CntPtr<Mso::IErrorString> ToString(const ErrorCode &errorCode) const noexcept override;

  // By default we call ShipAssert. Provide a specialization for this method to react to unhandled errors differently.
  void OnUnhandledError(const ErrorCodeState &state) const noexcept;
};

namespace Details {

template <class TErrorInfo>
class ErrorCodeInfo final : public ErrorCodeState {
 public:
  const TErrorInfo &ErrorInfo() const noexcept;

  ErrorCodeInfo(const IErrorProvider &errorProvider, TErrorInfo &&errorInfo) noexcept;
  ErrorCodeInfo(const IErrorProvider &errorProvider, const TErrorInfo &errorInfo) noexcept;

 private:
  TErrorInfo m_errorInfo;
};

} // namespace Details

//============================================================================================
// ErrorProvider implementation
//============================================================================================

template <class T, class GuidToken>
ErrorCode ErrorProvider<T, GuidToken>::MakeErrorCode(T &&errorInfo) const noexcept {
  using ErrorInfo = Details::ErrorCodeInfo<T>;
  void *memory = Mso::Memory::FailFast::AllocateEx(sizeof(ErrorInfo), Mso::Memory::AllocFlags::ShutdownLeak);
  return ErrorCode(Mso::CntPtr<ErrorCodeState>{::new (memory) ErrorInfo(*this, std::move(errorInfo)), AttachTag});
}

template <class T, class GuidToken>
ErrorCode ErrorProvider<T, GuidToken>::MakeErrorCode(const T &errorInfo) const noexcept {
  using ErrorInfo = Details::ErrorCodeInfo<T>;
  void *memory = Mso::Memory::FailFast::AllocateEx(sizeof(ErrorInfo), Mso::Memory::AllocFlags::ShutdownLeak);
  return ErrorCode(Mso::CntPtr<ErrorCodeState>{::new (memory) ErrorInfo(*this, errorInfo), AttachTag});
}

template <class T, class GuidToken>
template <class TValue, class... TArgs>
Maybe<TValue> ErrorProvider<T, GuidToken>::MakeMaybe(TArgs &&... errorArgs) const noexcept {
  return Mso::Maybe<TValue>(MakeErrorCode(T(std::forward<TArgs>(errorArgs)...)));
}

template <class T, class GuidToken>
bool ErrorProvider<T, GuidToken>::IsOwnedErrorCode(const ErrorCode &errorCode) const noexcept {
  return (TryGetErrorInfo(errorCode, /*shouldHandle:*/ false) != nullptr);
}

template <class T, class GuidToken>
const T &ErrorProvider<T, GuidToken>::GetErrorInfo(const ErrorCode &errorCode) const noexcept {
  const T *errorInfo = TryGetErrorInfo(errorCode, /*shouldHandle:*/ false);
  VerifyElseCrashSzTag(errorInfo, "Cannot get error info from error code.", 0x0130f523 /* tag_bmpu9 */);
  return *errorInfo;
}

template <class T, class GuidToken>
const T *ErrorProvider<T, GuidToken>::TryGetErrorInfo(const ErrorCode &errorCode, bool shouldHandle) const noexcept {
  const ErrorCodeState *state = GetErrorCodeState(errorCode);
  if (state) {
    const IErrorProvider &errorProvider = state->ErrorProvider();
    if (&errorProvider == this || errorProvider.IsOfType(__uuidof(GuidToken))) {
      if (shouldHandle) {
        state->Handle();
      }

      return &static_cast<const Details::ErrorCodeInfo<T> *>(state)->ErrorInfo();
    }
  }

  return nullptr;
}

template <class T, class GuidToken>
void ErrorProvider<T, GuidToken>::Destroy(const ErrorCodeState &state) const noexcept {
  if (!state.IsHandled()) {
    OnUnhandledError(state);
  }

  static_cast<const Details::ErrorCodeInfo<T> &>(state).~ErrorCodeInfo<T>();
  Mso::Memory::Free(const_cast<ErrorCodeState *>(&state));
}

template <class T, class GuidToken>
void ErrorProvider<T, GuidToken>::Throw(const ErrorCode &errorCode, bool shouldHandle) const {
  if (TryGetErrorInfo(errorCode, shouldHandle)) {
    throw ErrorCodeException(errorCode);
  }
}

template <class T, class GuidToken>
void ErrorProvider<T, GuidToken>::OnUnhandledError(const ErrorCodeState & /*state*/) const noexcept {
  UNREFERENCED_OACR(this);
  // TODO: Uncomment
  // ShipAssertSzTag(false, "Error was unhandled before destruction", UNTAGGED);
}

template <class T, class GuidToken>
bool ErrorProvider<T, GuidToken>::IsOfType(const GUID &type) const noexcept {
  return (type == __uuidof(GuidToken));
}

// Default implementation
template <class T, class GuidToken>
Mso::CntPtr<Mso::IErrorString> ErrorProvider<T, GuidToken>::ToString(const ErrorCode & /*errorCode*/) const noexcept {
  return Mso::MakeErrorString("Error"); // Default error message for all providers
}

//============================================================================================
// ErrorCodeInfo implementation
//============================================================================================

namespace Details {

template <class TErrorInfo>
inline const TErrorInfo &ErrorCodeInfo<TErrorInfo>::ErrorInfo() const noexcept {
  return m_errorInfo;
}

template <class TErrorInfo>
inline ErrorCodeInfo<TErrorInfo>::ErrorCodeInfo(const IErrorProvider &errorProvider, TErrorInfo &&errorInfo) noexcept
    : ErrorCodeState(errorProvider), m_errorInfo(std::move(errorInfo)) {}

template <class TErrorInfo>
inline ErrorCodeInfo<TErrorInfo>::ErrorCodeInfo(
    const IErrorProvider &errorProvider,
    const TErrorInfo &errorInfo) noexcept
    : ErrorCodeState(errorProvider), m_errorInfo(errorInfo) {}

} // namespace Details
} // namespace Mso

#pragma pack(pop)

#endif // MSO_ERRORCODE_ERRORPROVIDER_H
