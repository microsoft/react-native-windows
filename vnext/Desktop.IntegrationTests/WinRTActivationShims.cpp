// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

// This file provides local implementations for WinRT static methods and
// constructors that are declared by the CppWinRT-generated headers with
// CppWinRTOptimized=true. In that mode, cppwinrt emits non-inline
// declarations expecting the symbols to be supplied by the producing DLL's
// import library. When those symbols are intentionally excluded from the
// DLL's DEF file, the linker cannot resolve them.
//
// The implementations below obtain activation factories directly from the
// react-native-win32.dll via its DllGetActivationFactory export.  This
// bypasses RoGetActivationFactory and therefore does not require manifest
// or activation-context registration.

#include <activation.h>
#include <windows.h>
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Windows.Foundation.h>

namespace {

using DllGetActivationFactory_t = HRESULT(WINAPI *)(HSTRING, ::IActivationFactory **);

DllGetActivationFactory_t GetDllGetActivationFactory() noexcept {
  static auto pfn = reinterpret_cast<DllGetActivationFactory_t>(
      ::GetProcAddress(::GetModuleHandleW(L"react-native-win32.dll"), "DllGetActivationFactory"));
  return pfn;
}

template <typename TFactory>
TFactory GetFactory(std::wstring_view className) {
  winrt::hstring name{className};
  winrt::com_ptr<::IActivationFactory> factory;
  winrt::check_hresult(GetDllGetActivationFactory()(static_cast<HSTRING>(winrt::get_abi(name)), factory.put()));
  return factory.as<TFactory>();
}

} // anonymous namespace

namespace winrt::Microsoft::ReactNative {

// ReactPropertyBagHelper statics -----------------------------------------

IReactPropertyNamespace ReactPropertyBagHelper::GetNamespace(param::hstring const &namespaceName) {
  static auto factory = GetFactory<IReactPropertyBagHelperStatics>(winrt::name_of<ReactPropertyBagHelper>());
  return factory.GetNamespace(namespaceName);
}

IReactPropertyName ReactPropertyBagHelper::GetName(IReactPropertyNamespace const &ns, param::hstring const &localName) {
  static auto factory = GetFactory<IReactPropertyBagHelperStatics>(winrt::name_of<ReactPropertyBagHelper>());
  return factory.GetName(ns, localName);
}

// ReactDispatcherHelper statics ------------------------------------------

IReactPropertyName ReactDispatcherHelper::UIDispatcherProperty() {
  static auto factory = GetFactory<IReactDispatcherHelperStatics>(winrt::name_of<ReactDispatcherHelper>());
  return factory.UIDispatcherProperty();
}

// ReactNativeHost default constructor ------------------------------------

ReactNativeHost::ReactNativeHost()
    : ReactNativeHost(GetFactory<Windows::Foundation::IActivationFactory>(winrt::name_of<ReactNativeHost>())
                          .ActivateInstance<ReactNativeHost>()) {}

} // namespace winrt::Microsoft::ReactNative
