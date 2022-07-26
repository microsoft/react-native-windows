// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ReactApplication.g.h"
#include <CppWinRTIncludes.h>
#include <UI.Xaml.Markup.h>
#include <winrt/Microsoft.ReactNative.h>
#include "ReactNativeHost.h"

#ifdef USE_WINUI3
namespace activation = xaml;
#else
namespace activation = winrt::Windows::ApplicationModel::Activation;
#endif

namespace winrt::Microsoft::ReactNative::implementation {

// NoDefaultCtorReactApplication_base is a copy of the generated ReactApplication_base
// without the default constructor where it calls a factory for the base type.
// This is done to fix the aggregation issue in types inheriting from the ReactApplication.
// We call the factory in the ReactApplication constructor where we can pass correct
// 'outer' interface.
//
// This class must match the generated ReactApplication_base.
// It must be updated if the shape of generated ReactApplication_base is changed in future.
// The only difference is that this class has no default constructor.

template <typename D, typename... I>
struct __declspec(empty_bases) NoDefaultCtorReactApplication_base :
#ifndef USE_WINUI3
    implements<
        D,
        Microsoft::ReactNative::ReactApplication,
        composable,
        composing,
        xaml::IApplicationOverrides,
        xaml::IApplicationOverrides2,
        I...>,
    impl::require<D, xaml::IApplication, xaml::IApplication2, xaml::IApplication3>,
    impl::base<D, xaml::Application>,
    xaml::IApplicationOverridesT<D>,
    xaml::IApplicationOverrides2T<D>
#else
    implements<D, Microsoft::ReactNative::ReactApplication, composable, composing, xaml::IApplicationOverrides, I...>,
    impl::require<D, xaml::IApplication>,
    impl::base<D, xaml::Application>,
    xaml::IApplicationOverridesT<D>
#endif
{
  using base_type = NoDefaultCtorReactApplication_base;
  using class_type = Microsoft::ReactNative::ReactApplication;
  using implements_type = typename NoDefaultCtorReactApplication_base::implements_type;
  using implements_type::implements_type;
  using composable_base = xaml::Application;

  hstring GetRuntimeClassName() const {
    return L"Microsoft.ReactNative.ReactApplication";
  }

 protected:
  using dispatch = impl::dispatch_to_overridable<
      D,
      xaml::IApplicationOverrides
#ifndef USE_WINUI3
      ,
      xaml::IApplicationOverrides2
#endif
      >;
  auto overridable() noexcept {
    return dispatch::overridable(static_cast<D &>(*this));
  }
};

struct ReactApplication : NoDefaultCtorReactApplication_base<ReactApplication, xaml::Markup::IXamlMetadataProvider> {
 public: // ReactApplication ABI API
  ReactApplication();
  ReactApplication(IInspectable const &outer) noexcept;

  ReactNative::ReactInstanceSettings InstanceSettings() noexcept;
  void InstanceSettings(ReactNative::ReactInstanceSettings const &value) noexcept;

  Windows::Foundation::Collections::IVector<IReactPackageProvider> PackageProviders() noexcept;

  ReactNative::ReactNativeHost Host() noexcept;

  bool UseDeveloperSupport() noexcept;
  void UseDeveloperSupport(bool value) noexcept;

  hstring JavaScriptBundleFile() noexcept;
  void JavaScriptBundleFile(hstring const &value) noexcept;

  void OnActivated(Windows::ApplicationModel::Activation::IActivatedEventArgs const &e);
  void OnLaunched(activation::LaunchActivatedEventArgs const &e);
  void OnSuspending(IInspectable const &, Windows::ApplicationModel::SuspendingEventArgs const &);
  void OnNavigationFailed(IInspectable const &, xaml::Navigation::NavigationFailedEventArgs const &);

  using AppLaunchedDelegate = winrt::delegate<void(
      winrt::Microsoft::ReactNative::ReactApplication const &sender,
      winrt::Windows::ApplicationModel::Activation::LaunchActivatedEventArgs const &args)>;

  void LaunchedInternal(AppLaunchedDelegate delegate) noexcept;
  AppLaunchedDelegate LaunchedInternal() const noexcept;

  using AppViewCreatedDelegate =
      winrt::delegate<void(winrt::Microsoft::ReactNative::ReactApplication const &sender, winrt::hstring const &args)>;

  void ViewCreatedInternal(AppViewCreatedDelegate delegate) noexcept;

  AppViewCreatedDelegate ViewCreatedInternal() const noexcept;

  using AppPageNavigatedDelegate =
      winrt::delegate<void(winrt::Microsoft::ReactNative::ReactApplication const &sender, ReactRootView const &view)>;

  void PageNavigatedInternal(AppPageNavigatedDelegate delegate) noexcept;

  AppPageNavigatedDelegate PageNavigatedInternal() const noexcept;

  xaml::Markup::IXamlType GetXamlType(winrt::hstring const &name) const;

  xaml::Markup::IXamlType GetXamlType(::winrt::Windows::UI::Xaml::Interop::TypeName const &type) const;
  ::winrt::com_array<xaml::Markup::XmlnsDefinition> GetXmlnsDefinitions() const;

 private:
  winrt::Microsoft::ReactNative::ReactInstanceSettings m_instanceSettings{nullptr};
  winrt::Microsoft::ReactNative::ReactNativeHost m_host{nullptr};

  void OnCreate(Windows::ApplicationModel::Activation::IActivatedEventArgs const &e);

  AppLaunchedDelegate m_launched;
  AppViewCreatedDelegate m_viewCreated;
  AppPageNavigatedDelegate m_pageNavigated;

  XamlMetaDataProvider m_provider;
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {

// Override the CreateInstance method to pass baseInterface to the ReactApplication constructor
// to support correct COM aggregation that is need to inherit from the ReactApplication.
struct ReactApplication : ReactApplicationT<ReactApplication, implementation::ReactApplication> {
  auto CreateInstance(
      winrt::Windows::Foundation::IInspectable const &baseInterface,
      winrt::Windows::Foundation::IInspectable &innerInterface) {
    return impl::composable_factory<implementation::ReactApplication>::template CreateInstance<
        Microsoft::ReactNative::ReactApplication>(baseInterface, innerInterface, baseInterface);
  }
};

} // namespace winrt::Microsoft::ReactNative::factory_implementation
