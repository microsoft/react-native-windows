#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"
#include "winrt/Microsoft.ReactNative.h"
#include "winrt/TreeDumpLibrary.h"
#include <winrt/Windows.UI.ViewManagement.h>

using namespace winrt;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Storage::Pickers;

namespace winrt::playground::implementation {

MainPage::MainPage() {
  InitializeComponent();

  x_DebuggerPort().Text(std::to_wstring(Host().InstanceSettings().DebuggerPort()));

  // IsEditable is only supported on RS4 or higher
  if (Windows::Foundation::Metadata::ApiInformation::IsApiContractPresent(
          L"Windows.Foundation.UniversalApiContract", 6)) {
    x_rootComponentNameCombo().IsEditable(true);
    x_entryPointCombo().IsEditable(true);
  }
}

void MainPage::OnLoadClick(
    Windows::Foundation::IInspectable const & /*sender*/,
    Windows::UI::Xaml::RoutedEventArgs const & /*args*/) {
  auto host = Host();
  auto bundleFile = unbox_value<hstring>(x_entryPointCombo().SelectedItem().as<ComboBoxItem>().Content());
  host.InstanceSettings().JavaScriptBundleFile(bundleFile);
  auto mainComponentName = unbox_value<hstring>(x_rootComponentNameCombo().SelectedItem().as<ComboBoxItem>().Content());
  host.InstanceSettings().MainComponentName(mainComponentName);
  host.InstanceSettings().UseWebDebugger(x_UseWebDebuggerCheckBox().IsChecked().GetBoolean());
  host.InstanceSettings().UseDirectDebugger(x_UseDirectDebuggerCheckBox().IsChecked().GetBoolean());
  host.InstanceSettings().DebuggerBreakOnNextLine(x_BreakOnFirstLineCheckBox().IsChecked().GetBoolean());
  host.InstanceSettings().UseFastRefresh(x_UseFastRefreshCheckBox().IsChecked().GetBoolean());
  host.InstanceSettings().DebuggerPort(static_cast<uint16_t>(std::stoi(std::wstring(x_DebuggerPort().Text()))));

  // Nudge the ReactNativeHost to create the instance and wrapping context
  host.ReloadInstance();

  m_reactRootView = Microsoft::ReactNative::ReactRootView();
  m_reactRootView.ComponentName(host.InstanceSettings().MainComponentName());
  m_reactRootView.ReactNativeHost(host);

  x_rootElement().Children().Clear();
  x_rootElement().Children().Append(m_reactRootView);
}

void MainPage::UpdateTreeDump(
    Windows::Foundation::IInspectable const & /*sender*/,
    Windows::UI::Xaml::RoutedEventArgs const & /*args*/) {

    Windows::UI::ViewManagement::ApplicationView::GetForCurrentView().TryResizeView(Size(1280, 1024));
    
    auto str = TreeDumpLibrary::VisualTreeDumper::DumpTree(m_reactRootView, nullptr, {}, TreeDumpLibrary::DumpTreeMode::Json);

    TreeDumpLibrary::VisualTreeDumper::DoesTreeDumpMatchForRNTester(m_reactRootView)
        .Completed([=](const IAsyncOperation<bool> &ao, auto &&b) {
          bool matches = false;

          try {
            matches = ao.GetResults();
          } catch (winrt::hresult_error &e) {
            OutputDebugString(L"Error from DoesTreeDumpMatchForRNTester: ");
            OutputDebugString(e.message().data());
            OutputDebugString(L"\n");
          }
          x_TreeDump().Content(winrt::box_value(winrt::to_hstring(matches ? L"OK" : L"ERROR")));
        });
    
}


void winrt::playground::implementation::MainPage::x_entryPointCombo_SelectionChanged(
    winrt::Windows::Foundation::IInspectable const & /*sender*/,
    winrt::Windows::UI::Xaml::Controls::SelectionChangedEventArgs const & /*e*/) {
  if (x_rootComponentNameCombo()) {
    auto bundleFile = unbox_value<hstring>(x_entryPointCombo().SelectedItem().as<ComboBoxItem>().Content());
    if (std::wstring(bundleFile).compare(L"Samples\\rntester") == 0) {
      x_rootComponentNameCombo().SelectedIndex(0);
    } else {
      x_rootComponentNameCombo().SelectedIndex(1);
    }
  }
}

Microsoft::ReactNative::ReactNativeHost MainPage::Host() noexcept {
  if (!m_host) {
    m_host = Microsoft::ReactNative::ReactNativeHost();
    m_host.InstanceSettings(InstanceSettings());
    m_host.PackageProviders(PackageProviders());
  }

  return m_host;
}

Microsoft::ReactNative::ReactInstanceSettings MainPage::InstanceSettings() noexcept {
  if (!m_instanceSettings) {
    m_instanceSettings = Microsoft::ReactNative::ReactInstanceSettings();
  }

  return m_instanceSettings;
}

Windows::Foundation::Collections::IVector<Microsoft::ReactNative::IReactPackageProvider>
MainPage::PackageProviders() noexcept {
  if (!m_packageProviders) {
    m_packageProviders = single_threaded_vector<Microsoft::ReactNative::IReactPackageProvider>();
  }

  return m_packageProviders;
}

} // namespace winrt::playground::implementation
