#include "pch.h"
#include "CompositionAnnotationProvider.h"
#include <Fabric/ComponentView.h>
#include <Unicode.h>
#include "RootComponentView.h"
#include "UiaHelpers.h"

namespace winrt::Microsoft::ReactNative::implementation {

CompositionAnnotationProvider::CompositionAnnotationProvider(
    const winrt::Microsoft::ReactNative::Composition::ComponentView &componentView) noexcept
    : m_view{componentView} {}
HRESULT __stdcall CompositionAnnotationProvider::get_AnnotationTypeId(int *retVal) {
  if (retVal == nullptr)
    return E_POINTER;
  auto strongView = m_view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;

  auto props = std::static_pointer_cast<const facebook::react::ViewProps>(
      winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(strongView)->props());
  if (props == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;

  if (props->accessibilityAnnotation.value().typeID.empty()) {
    return E_FAIL;
  }
  *retVal = static_cast<int>(GetAnnotationTypeId(props->accessibilityAnnotation.value().typeID));
  return S_OK;
}
HRESULT __stdcall CompositionAnnotationProvider::get_AnnotationTypeName(BSTR *retVal) {
  if (retVal == nullptr)
    return E_POINTER;
  auto strongView = m_view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;

  auto props = std::static_pointer_cast<const facebook::react::ViewProps>(
      winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(strongView)->props());
  if (props == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;

  if (props->accessibilityAnnotation.value().typeName.empty()) {
    return E_FAIL;
  }
  auto typeName = ::Microsoft::Common::Unicode::Utf8ToUtf16(props->accessibilityAnnotation.value().typeName);
  *retVal = SysAllocString(typeName.c_str());
  return S_OK;
}
HRESULT __stdcall CompositionAnnotationProvider::get_Author(BSTR *retVal) {
  if (retVal == nullptr)
    return E_POINTER;
  auto strongView = m_view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;

  auto props = std::static_pointer_cast<const facebook::react::ViewProps>(
      winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(strongView)->props());
  if (props == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;

  if (props->accessibilityAnnotation.value().author.empty()) {
    return E_FAIL;
  }
  auto author = ::Microsoft::Common::Unicode::Utf8ToUtf16(props->accessibilityAnnotation.value().author);
  *retVal = SysAllocString(author.c_str());
  return S_OK;
}
HRESULT __stdcall CompositionAnnotationProvider::get_DateTime(BSTR *retVal) {
  if (retVal == nullptr)
    return E_POINTER;
  auto strongView = m_view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;

  auto props = std::static_pointer_cast<const facebook::react::ViewProps>(
      winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(strongView)->props());
  if (props == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;

  if (props->accessibilityAnnotation.value().dateTime.empty()) {
    return E_FAIL;
  }
  auto dateTime = ::Microsoft::Common::Unicode::Utf8ToUtf16(props->accessibilityAnnotation.value().dateTime);
  *retVal = SysAllocString(dateTime.c_str());
  return S_OK;
}

HRESULT __stdcall CompositionAnnotationProvider::get_Target(IRawElementProviderSimple **retVal) {
  // no-opt
  return E_NOTIMPL;
}
} // namespace winrt::Microsoft::ReactNative::implementation
