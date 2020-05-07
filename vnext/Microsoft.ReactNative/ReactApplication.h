// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "ReactApplication.g.h"
#include <CppWinRTIncludes.h>
#include "ReactNativeHost.h"

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
struct __declspec(empty_bases) NoDefaultCtorReactApplication_base : implements<
                                                                        D,
                                                                        Microsoft::ReactNative::ReactApplication,
                                                                        composable,
                                                                        composing,
                                                                        Windows::UI::Xaml::IApplicationOverrides,
                                                                        Windows::UI::Xaml::IApplicationOverrides2,
                                                                        I...>,
                                                                    impl::require<
                                                                        D,
                                                                        Windows::UI::Xaml::IApplication,
                                                                        Windows::UI::Xaml::IApplication2,
                                                                        Windows::UI::Xaml::IApplication3>,
                                                                    impl::base<D, Windows::UI::Xaml::Application>,
                                                                    Windows::UI::Xaml::IApplicationOverridesT<D>,
                                                                    Windows::UI::Xaml::IApplicationOverrides2T<D> {
  using base_type = NoDefaultCtorReactApplication_base;
  using class_type = Microsoft::ReactNative::ReactApplication;
  using implements_type = typename NoDefaultCtorReactApplication_base::implements_type;
  using implements_type::implements_type;
  using composable_base = Windows::UI::Xaml::Application;

  hstring GetRuntimeClassName() const {
    return L"Microsoft.ReactNative.ReactApplication";
  }

 protected:
  using dispatch = impl::
      dispatch_to_overridable<D, Windows::UI::Xaml::IApplicationOverrides, Windows::UI::Xaml::IApplicationOverrides2>;
  auto overridable() noexcept {
    return dispatch::overridable(static_cast<D &>(*this));
  }
};

struct ReactApplication : NoDefaultCtorReactApplication_base<ReactApplication> {
 public: // ReactApplication ABI API
  ReactApplication();
  ReactApplication(IInspectable const &outer) noexcept;

  ReactNative::ReactInstanceSettings InstanceSettings() noexcept;
  void InstanceSettings(ReactNative::ReactInstanceSettings const &value) noexcept;

  Windows::Foundation::Collections::IVector<IReactPackageProvider> PackageProviders() noexcept;
  void PackageProviders(Windows::Foundation::Collections::IVector<IReactPackageProvider> const &value) noexcept;

  ReactNative::ReactNativeHost Host() noexcept;

  hstring MainComponentName() noexcept;
  void MainComponentName(hstring const &value) noexcept;

  bool UseDeveloperSupport() noexcept;
  void UseDeveloperSupport(bool value) noexcept;

  hstring JavaScriptMainModuleName() noexcept;
  void JavaScriptMainModuleName(hstring const &value) noexcept;

  hstring JavaScriptBundleFile() noexcept;
  void JavaScriptBundleFile(hstring const &value) noexcept;

 public:
  void OnActivated(Windows::ApplicationModel::Activation::IActivatedEventArgs const &e);
  void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs const &e);
  void OnSuspending(IInspectable const &, Windows::ApplicationModel::SuspendingEventArgs const &);
  void OnNavigationFailed(IInspectable const &, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs const &);

 protected:
  virtual ReactApplicationDelegate __stdcall CreateReactApplicationDelegate();

 private:
  ReactNative::ReactInstanceSettings m_instanceSettings{nullptr};
  Windows::Foundation::Collections::IVector<IReactPackageProvider> m_packageProviders{nullptr};
  ReactNative::ReactNativeHost m_host{nullptr};

  ReactApplicationDelegate m_delegate{nullptr};

  void OnCreate(Windows::ApplicationModel::Activation::IActivatedEventArgs const &e);
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {

// Override the CreateInstance method to pass baseInterface to the ReactApplication constructor
// to support correct COM aggregation that is need to inherit from the ReactApplication.
struct ReactApplication : ReactApplicationT<ReactApplication, implementation::ReactApplication> {
  auto CreateInstance(IInspectable const &baseInterface, IInspectable &innerInterface) {
    return impl::composable_factory<implementation::ReactApplication>::template CreateInstance<
        Microsoft::ReactNative::ReactApplication>(baseInterface, innerInterface, baseInterface);
  }
};

} // namespace winrt::Microsoft::ReactNative::factory_implementation
