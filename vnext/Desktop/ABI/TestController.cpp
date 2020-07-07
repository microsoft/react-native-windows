#include "pch.h"

#include "DynamicReader.h"
#include "DynamicWriter.h"
#include "TestController.h"

#include "Microsoft.Internal.TestController.g.cpp"

using namespace winrt;

namespace {

// Extends the 'DynamicReader' class with the ability the manage the lifetime of the folly::dynamic value it's reading
// from.
struct DynamicWrapperReader : public winrt::implements<DynamicWrapperReader, Microsoft::ReactNative::IJSValueReader> {
  DynamicWrapperReader(folly::dynamic &&value) : m_value{std::move(value)} {
    m_innerReader = make<Microsoft::ReactNative::DynamicReader>(m_value);
  }

  Microsoft::ReactNative::JSValueType ValueType() noexcept {
    return m_innerReader.ValueType();
  }

  bool GetNextObjectProperty(hstring &propertyName) noexcept {
    return m_innerReader.GetNextObjectProperty(propertyName);
  }

  bool GetNextArrayItem() noexcept {
    return m_innerReader.GetNextArrayItem();
  }

  hstring GetString() noexcept {
    return m_innerReader.GetString();
  }

  bool GetBoolean() noexcept {
    return m_innerReader.GetBoolean();
  }

  int64_t GetInt64() noexcept {
    return m_innerReader.GetInt64();
  }

  double GetDouble() noexcept {
    return m_innerReader.GetDouble();
  }

 private:
  Microsoft::ReactNative::IJSValueReader m_innerReader;
  folly::dynamic m_value;
};

} // namespace

namespace winrt::Microsoft::Internal::implementation {

Microsoft::ReactNative::IJSValueReader TestController::CreateDynamicReader(Microsoft::ReactNative::IJSValueWriter writer) {
  auto dw = writer.as<Microsoft::ReactNative::DynamicWriter>();
  return make<DynamicWrapperReader>(dw->TakeValue());
}

Microsoft::ReactNative::IJSValueWriter TestController::CreateDynamicWriter() {
  return make<winrt::Microsoft::ReactNative::DynamicWriter>();
}

} // namespace winrt::Microsoft::Internal::implementation
