#include "pch.h"
#include <Fabric/ComponentView.h>
#include <Unicode.h>
#include "CompositionAnnotationProvider.h"
#include "RootComponentView.h"
#include "UiaHelpers.h"

namespace winrt::Microsoft::ReactNative::implementation {

CompositionAnnotationProvider::CompositionAnnotationProvider(
    const winrt::Microsoft::ReactNative::Composition::ComponentView &componentView,
    CompositionDynamicAutomationProvider *parentProvider) noexcept
    : m_view{componentView} {
  m_parentProvider.copy_from(parentProvider);
}
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

  if (!props->accessibilityAnnotation.typeID.has_value())
  {
    return E_FAIL;
  }
  *retVal = static_cast<int>(GetAnnotationTypeId(props->accessibilityAnnotation.typeID.value()));
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

  if (!props->accessibilityAnnotation.typeName.has_value()) {
    return E_FAIL;
  }
  auto typeName = ::Microsoft::Common::Unicode::Utf8ToUtf16(props->accessibilityAnnotation.typeName.value());
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

  if (!props->accessibilityAnnotation.author.has_value()) {
    return E_FAIL;
  }
  auto author = ::Microsoft::Common::Unicode::Utf8ToUtf16(props->accessibilityAnnotation.author.value());
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

  if (!props->accessibilityAnnotation.dateTime.has_value()) {
    return E_FAIL;
  }
  auto dateTime = ::Microsoft::Common::Unicode::Utf8ToUtf16(props->accessibilityAnnotation.dateTime.value());
  *retVal = SysAllocString(dateTime.c_str());
  return S_OK;
}
HRESULT __stdcall CompositionAnnotationProvider::get_Target(IRawElementProviderSimple **retVal) {
  return E_NOTIMPL;
}
} // namespace winrt::Microsoft::ReactNative::implementation
