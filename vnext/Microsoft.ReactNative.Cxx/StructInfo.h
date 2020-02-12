// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#ifndef MICROSOFT_REACTNATIVE_STRUCTINFO
#define MICROSOFT_REACTNATIVE_STRUCTINFO

#include "winrt/Microsoft.ReactNative.h"

#define REACT_STRUCT(type)                                                           \
  struct type;                                                                       \
  winrt::Microsoft::ReactNative::FieldMap GetStructInfo(type *) noexcept {           \
    winrt::Microsoft::ReactNative::FieldMap fieldMap{};                              \
    winrt::Microsoft::ReactNative::CollectStructFields<type, __COUNTER__>(fieldMap); \
    return fieldMap;                                                                 \
  }

#define REACT_FIELD(field)                                                 \
  template <class TClass>                                                  \
  static void RegisterField(                                               \
      winrt::Microsoft::ReactNative::FieldMap &fieldMap,                   \
      winrt::Microsoft::ReactNative::ReactFieldId<__COUNTER__>) noexcept { \
    fieldMap.emplace(L## #field, &TClass::field);                          \
  }

namespace winrt::Microsoft::ReactNative {

struct FieldInfo;
using FieldMap = std::map<std::wstring, FieldInfo, std::less<>>;
using FieldReaderType =
    void (*)(IJSValueReader const & /*reader*/, void * /*obj*/, const uintptr_t * /*fieldPtrStore*/) noexcept;
using FieldWriterType =
    void (*)(IJSValueWriter const & /*writer*/, const void * /*obj*/, const uintptr_t * /*fieldPtrStore*/) noexcept;

template <class T>
void GetStructInfo(T *) {}

template <class TClass, class TValue>
void FieldReader(IJSValueReader const &reader, void *obj, const uintptr_t *fieldPtrStore) noexcept;

template <class TClass, class TValue>
void FieldWriter(IJSValueWriter const &writer, const void *obj, const uintptr_t *fieldPtrStore) noexcept;

struct FieldInfo {
  template <class TClass, class TValue>
  FieldInfo(TValue TClass::*fieldPtr) noexcept
      : m_fieldReader{FieldReader<TClass, TValue>},
        m_fieldWriter{FieldWriter<TClass, TValue>},
        m_fieldPtrStore{*reinterpret_cast<uintptr_t *>(&fieldPtr)} {
    static_assert(sizeof(m_fieldPtrStore) >= sizeof(fieldPtr));
  }

  void ReadField(IJSValueReader const &reader, void *obj) const noexcept {
    m_fieldReader(reader, obj, &m_fieldPtrStore);
  }

  void WriteField(IJSValueWriter const &writer, const void *obj) const noexcept {
    m_fieldWriter(writer, obj, &m_fieldPtrStore);
  }

 private:
  FieldReaderType m_fieldReader;
  FieldWriterType m_fieldWriter;
  const uintptr_t m_fieldPtrStore;
};

template <class TClass, class TValue>
void FieldReader(IJSValueReader const &reader, void *obj, const uintptr_t *fieldPtrStore) noexcept {
  using FieldPtrType = TValue TClass::*;
  ReadValue(reader, /*out*/ static_cast<TClass *>(obj)->*(*reinterpret_cast<const FieldPtrType *>(fieldPtrStore)));
}

template <class TClass, class TValue>
void FieldWriter(IJSValueWriter const &writer, const void *obj, const uintptr_t *fieldPtrStore) noexcept {
  using FieldPtrType = TValue TClass::*;
  WriteValue(writer, static_cast<const TClass *>(obj)->*(*reinterpret_cast<const FieldPtrType *>(fieldPtrStore)));
}

template <class T>
struct StructInfo {
  static const FieldMap FieldMap;
};

template <class T>
/*static*/ const FieldMap StructInfo<T>::FieldMap = GetStructInfo(static_cast<T *>(nullptr));

template <int I>
using ReactFieldId = std::integral_constant<int, I>;

template <class TClass, int I>
auto HasRegisterField(FieldMap &fieldMap, ReactFieldId<I> id)
    -> decltype(TClass::template RegisterField<TClass>(fieldMap, id), std::true_type{});
template <class TClass>
auto HasRegisterField(...) -> std::false_type;

template <class TClass, int I>
void CollectStructFields(FieldMap &fieldMap) noexcept {
  if constexpr (decltype(HasRegisterField<TClass>(fieldMap, ReactFieldId<I + 1>{}))::value) {
    TClass::template RegisterField<TClass>(fieldMap, ReactFieldId<I + 1>{});
    CollectStructFields<TClass, I + 1>(fieldMap);
  }
}

} // namespace winrt::Microsoft::ReactNative

#endif // MICROSOFT_REACTNATIVE_STRUCTINFO
