#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"
#include "winrt/Microsoft.ReactNative.h"
#include "winrt/TreeDumpLibrary.h"
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.System.h>

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
  SetUpTreeDump();
}

DependencyObject FindElementByAutomationId(DependencyObject node, const hstring &id) {
  auto value = node.GetValue(Automation::AutomationProperties::AutomationIdProperty());
  if (value && winrt::unbox_value<hstring>(value) == id) {
    return node;
  } else {
    auto count = VisualTreeHelper::GetChildrenCount(node);
    for (auto i = 0; i < count; i++) {
      auto ret = FindElementByAutomationId(VisualTreeHelper::GetChild(node, i), id);
      if (ret) {
        return ret;
      }
    }
  }
  return nullptr;
}

hstring GetCurentPageName(DependencyObject root) {
  auto header = FindElementByAutomationId(root, L"PageHeader");
  if (header != nullptr) {
    return header.as<TextBlock>().Text();
  }
  return {};
}

static Windows::System::DispatcherQueueTimer timer{nullptr};
static hstring lastPageName{};

void MainPage::SetUpTreeDump() {
    auto str = TreeDumpLibrary::VisualTreeDumper::DumpTree(m_reactRootView, nullptr, {}, TreeDumpLibrary::DumpTreeMode::Json);

    if (timer == nullptr) {
      auto dispatcher = winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
      timer = dispatcher.CreateTimer();
      timer.IsRepeating(true);
      timer.Interval(std::chrono::milliseconds(200));
      timer.Tick([=](auto &&, auto &&) {
        auto name = GetCurentPageName(m_reactRootView);
        auto text = x_TreeDump().Text();
        if ((lastPageName != name) || (text != L"OK")) {
            Windows::UI::ViewManagement::ApplicationView::GetForCurrentView().TryResizeView(Size(1280, 1024));
            auto ok = SolidColorBrush(ColorHelper::FromArgb(0xff, 0, 0xee, 0x40));
            timer.Interval(timer.Interval() * 2);
          x_TreeDump().Foreground(ok);
          x_TreeDump().Text(L"...");
          lastPageName = name;
          TreeDumpLibrary::VisualTreeDumper::DoesTreeDumpMatchForRNTester(m_reactRootView)
              .Completed([=](const IAsyncOperation<bool> &ao, auto &&) {
                bool matches = false;

                try {
                  matches = ao.GetResults();
                  if (matches) {
                    timer.Interval(std::chrono::milliseconds(200));
                  }
                } catch (winrt::hresult_error &e) {
                  OutputDebugString(L"Error from DoesTreeDumpMatchForRNTester: ");
                  OutputDebugString(e.message().data());
                  OutputDebugString(L"\n");
                }
                x_TreeDump().Text(matches ? L"OK" : L"ERROR");
                x_TreeDump().Foreground(matches ? ok : SolidColorBrush(ColorHelper::FromArgb(0xff, 0xee, 00, 40)));
              });
        }
      });
      timer.Start();
    }

    
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
