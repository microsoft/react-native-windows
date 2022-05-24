// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#ifndef MICROSOFT_REACTNATIVE_JSI_NODEAPIJSIRUNTIME
#define MICROSOFT_REACTNATIVE_JSI_NODEAPIJSIRUNTIME

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

template <typename T>
struct NativeObjectWrapper;

template <typename T>
struct NativeObjectWrapper<std::unique_ptr<T>> {
  static napi_ext_native_data Wrap(std::unique_ptr<T> &&obj) noexcept {
    napi_ext_native_data nativeData{};
    nativeData.data = obj.release();
    nativeData.finalize_cb = [](napi_env /*env*/, void *data, void * /*finalizeHint*/) {
      std::unique_ptr<T> obj{reinterpret_cast<T *>(data)};
    };
    return nativeData;
  }

  static T *Unwrap(napi_ext_native_data &nativeData) noexcept {
    return reinterpret_cast<T *>(nativeData.data);
  }
};

template <typename T>
struct NativeObjectWrapper<std::shared_ptr<T>> {
  static napi_ext_native_data Wrap(std::shared_ptr<T> &&obj) noexcept {
    static_assert(
        sizeof(SharedPtrHolder) == sizeof(std::shared_ptr<T>), "std::shared_ptr expected to have size of two pointers");
    SharedPtrHolder ptrHolder;
    new (std::addressof(ptrHolder)) std::shared_ptr(std::move(obj));
    napi_ext_native_data nativeData{};
    nativeData.data = ptrHolder.ptr1;
    nativeData.finalize_hint = ptrHolder.ptr2;
    nativeData.finalize_cb = [](napi_env /*env*/, void *data, void *finalizeHint) {
      SharedPtrHolder ptrHolder{data, finalizeHint};
      std::shared_ptr<T> obj(std::move(*reinterpret_cast<std::shared_ptr<T> *>(std::addressof(ptrHolder))));
    };
    return nativeData;
  }

  static std::shared_ptr<T> Unwrap(napi_ext_native_data &nativeData) noexcept {
    SharedPtrHolder ptrHolder{nativeData.data, nativeData.finalize_hint};
    return *reinterpret_cast<std::shared_ptr<T> *>(std::addressof(ptrHolder));
  }

 private:
  struct SharedPtrHolder {
    void *ptr1;
    void *ptr2;
  };
};

} // namespace Microsoft::JSI

#endif // MICROSOFT_REACTNATIVE_JSI_NODEAPIJSIRUNTIME
