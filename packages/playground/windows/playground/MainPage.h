#pragma once
#include "MainPage.g.h"

namespace winrt::playground::implementation {

struct MainPage : MainPageT<MainPage> {
  MainPage();

  void OnLoadClick(
      Windows::Foundation::IInspectable const & /*sender*/,
      Windows::UI::Xaml::RoutedEventArgs const & /*args*/);

  void UpdateTreeDump(
      Windows::Foundation::IInspectable const & /*sender*/,
      Windows::UI::Xaml::RoutedEventArgs const & /*args*/);


 private:
  Microsoft::ReactNative::ReactNativeHost Host() noexcept;
  Microsoft::ReactNative::ReactInstanceSettings InstanceSettings() noexcept;
  Windows::Foundation::Collections::IVector<Microsoft::ReactNative::IReactPackageProvider> PackageProviders() noexcept;

  Microsoft::ReactNative::ReactRootView m_reactRootView;
  Microsoft::ReactNative::ReactNativeHost m_host;
  Microsoft::ReactNative::ReactInstanceSettings m_instanceSettings;
  Windows::Foundation::Collections::IVector<Microsoft::ReactNative::IReactPackageProvider> m_packageProviders;

  bool m_useWebDebugger{false};

 public:
  void x_entryPointCombo_SelectionChanged(
      winrt::Windows::Foundation::IInspectable const &sender,
      winrt::Windows::UI::Xaml::Controls::SelectionChangedEventArgs const &e);
};

} // namespace winrt::playground::implementation
namespace winrt::playground::factory_implementation {
struct MainPage : MainPageT<MainPage, implementation::MainPage> {};
} // namespace winrt::playground::factory_implementation
