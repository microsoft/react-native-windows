// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.#include "pch.h"

#include "ReactPackageProvider.h"

using namespace winrt::playground;
using namespace winrt::playground::implementation;

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

struct App : ApplicationT<App> {
  void OnLaunched(LaunchActivatedEventArgs const &) {
    const hstring xamlString =
        L" <Grid"
        L"  xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'"
        L"  xmlns:x='http://schemas.microsoft.com/winfx/2006/xaml'>"
        L"   <Grid.RowDefinitions>"
        L"     <RowDefinition Height='auto'/>"
        L"     <RowDefinition Height='auto'/>"
        L"     <RowDefinition Height='auto'/>"
        L"     <RowDefinition Height='*'/>"
        L"   </Grid.RowDefinitions>"
        L"   <Grid.ColumnDefinitions>"
        L"     <ColumnDefinition Width='auto'/>"
        L"     <ColumnDefinition Width='*'/>"
        L"   </Grid.ColumnDefinitions>"
        L""
        L"   <Button x:Name='x_LoadButton'"
        L"     Grid.Column='0'"
        L"     Grid.RowSpan='2'"
        L"     Margin='2,2,2,2'"
        L"     VerticalAlignment='Stretch'"
        L"     Content='Load' />"
        L""
        L"   <Grid Grid.Row='0' Grid.Column='1'>"
        L"     <Grid.ColumnDefinitions>"
        L"       <ColumnDefinition Width='auto'/>"
        L"       <ColumnDefinition Width='*'/>"
        L"       <ColumnDefinition Width='auto'/>"
        L"       <ColumnDefinition Width='auto'/>"
        L"     </Grid.ColumnDefinitions>"
        L""
        L"     <TextBlock"
        L"       Grid.Column='0'"
        L"       Margin='2,2,2,2'"
        L"       VerticalAlignment='Center'"
        L"       Text='JavaScript File (no extension):'/>"
        L"     <ComboBox x:Name='x_JavaScriptFilename'"
        L"       Grid.Column='1'"
        L"       Margin='2,2,2,2'"
        L"       MinWidth='250'"
        L"       SelectedIndex='0'"
        L"       VerticalAlignment='Center'"
        L"       HorizontalAlignment='Stretch'>"
        L"       <ComboBox.Items>"
        L"         <ComboBoxItem Content='Samples\\rntester' />"
        L"         <ComboBoxItem Content='Samples\\accessible' />"
        L"         <ComboBoxItem Content='Samples\\animation' />"
        L"         <ComboBoxItem Content='Samples\\calculator' />"
        L"         <ComboBoxItem Content='Samples\\callbackTest' />"
        L"         <ComboBoxItem Content='Samples\\click' />"
        L"         <ComboBoxItem Content='Samples\\control' />"
        L"         <ComboBoxItem Content='Samples\\flexbox' />"
        L"         <ComboBoxItem Content='Samples\\focusTest' />"
        L"         <ComboBoxItem Content='Samples\\geosample' />"
        L"         <ComboBoxItem Content='Samples\\image' />"
        L"         <ComboBoxItem Content='Samples\\index' />"
        L"         <ComboBoxItem Content='Samples\\messages' />"
        L"         <ComboBoxItem Content='Samples\\mouse' />"
        L"         <ComboBoxItem Content='Samples\\scrollViewSnapSample' />"
        L"         <ComboBoxItem Content='Samples\\simple' />"
        L"         <ComboBoxItem Content='Samples\\text' />"
        L"         <ComboBoxItem Content='Samples\\textinput' />"
        L"         <ComboBoxItem Content='Samples\\ticTacToe' />"
        L"         <ComboBoxItem Content='Samples\\view' />"
        L"       </ComboBox.Items>"
        L"     </ComboBox>"
        L""
        L"     <TextBlock"
        L"       Grid.Column='2'"
        L"       Margin='2,2,2,2'"
        L"       VerticalAlignment='Center'"
        L"       Text='App Name:'/>"
        L"     <ComboBox"
        L"       x:Name='x_ReactAppName'"
        L"       Grid.Column='3'"
        L"       Margin='2,2,2,2'"
        L"       MinWidth='150'"
        L"       SelectedIndex='0'"
        L"       HorizontalAlignment='Stretch'"
        L"       VerticalAlignment='Center' >"
        L"       <ComboBox.Items>"
        L"         <ComboBoxItem Content='RNTesterApp' />"
        L"       </ComboBox.Items>"
        L"     </ComboBox>"
        L"   </Grid>"
        L""
        L"   <StackPanel"
        L"     Grid.Row='1'"
        L"     Grid.Column='1'"
        L"     Orientation='Horizontal'>"
        L"     <CheckBox x:Name='x_UseWebDebuggerCheckBox'"
        L"       Margin='5,2'"
        L"       VerticalAlignment='Center'"
        L"       IsChecked='true'"
        L"       Content='Web Debugger'/>"
        L"    <CheckBox x:Name='x_UseDirectDebuggerCheckBox'"
        L"       Margin='5,2'"
        L"       VerticalAlignment='Center'"
        L"       IsChecked='false'"
        L"       Content='Direct Debugging'/>"
        L"     <CheckBox x:Name='x_BreakOnFirstLineCheckBox'"
        L"       Margin='5,2'"
        L"       VerticalAlignment='Center'"
        L"       IsChecked='false'"
        L"       Content='Break on First Line'/>"
        L"     <CheckBox x:Name='x_UseFastRefreshCheckBox'"
        L"       Margin='5,2'"
        L"       VerticalAlignment='Center'"
        L"       IsChecked='true'"
        L"       Content='Fast Refresh'/>"
        L"     <TextBlock"
        L"       VerticalAlignment='Center'"
        L"       Margin='5,0,5,3'"
        L"       Text='Debugger Port'/>"
        L"     <TextBox x:Name='x_DebuggerPort'"
        L"       Margin='5,0'"
        L"       VerticalAlignment='Center'"
        L"       InputScope='Number'"
        L"       MaxLength='5' />"
        L"     <TextBlock"
        L"       Grid.Column='2'"
        L"       Margin='2,2,2,2'"
        L"       VerticalAlignment='Center'"
        L"       Text='JS Engine (NYI):'/>"
        L"     <ComboBox"
        L"       x:Name='x_JsEngine'"
        L"       Grid.Column='3'"
        L"       Margin='2,2,2,2'"
        L"       MinWidth='150'"
        L"       HorizontalAlignment='Stretch'"
        L"       VerticalAlignment='Center'>"
        L"       <ComboBox.Items>"
        L"         <ComboBoxItem Content='Chakra' />"
        L"         <ComboBoxItem Content='Hermes' x:Name='x_engineHermes' />"
        L"         <ComboBoxItem Content='V8' x:Name='x_engineV8' />"
        L"       </ComboBox.Items>"
        L"     </ComboBox>"
        L"   </StackPanel>"
        L""
        L"   <Grid x:Name='RootElement'"
        L"     Grid.Row='3'"
        L"     Grid.ColumnSpan='2'"
        L"     Background='{ThemeResource ApplicationPageBackgroundThemeBrush}'/>"
        L" </Grid>";

    Grid rootGrid = unbox_value<Grid>(Markup::XamlReader::Load(xamlString));
    Window window = Window::Current();
    window.Content(rootGrid);
    window.Activate();

    m_rootElement = rootGrid.FindName(L"RootElement").as<Grid>();

    m_entryPointCombo = rootGrid.FindName(L"x_JavaScriptFilename").as<ComboBox>();
    m_rootComponentNameCombo = rootGrid.FindName(L"x_ReactAppName").as<ComboBox>();
    m_UseWebDebuggerCheckBox = rootGrid.FindName(L"x_UseWebDebuggerCheckBox").as<CheckBox>();
    m_UseDirectDebuggerCheckBox = rootGrid.FindName(L"x_UseDirectDebuggerCheckBox").as<CheckBox>();
    m_BreakOnFirstLineCheckBox = rootGrid.FindName(L"x_BreakOnFirstLineCheckBox").as<CheckBox>();
    m_UseFastRefreshCheckBox = rootGrid.FindName(L"x_UseFastRefreshCheckBox").as<CheckBox>();
    m_DebuggerPort = rootGrid.FindName(L"x_DebuggerPort").as<TextBox>();

    m_DebuggerPort.Text(std::to_wstring(Host().InstanceSettings().DebuggerPort()));

    // IsEditable is only supported on RS4 or higher
    if (Windows::Foundation::Metadata::ApiInformation::IsApiContractPresent(
            L"Windows.Foundation.UniversalApiContract", 6)) {
      m_rootComponentNameCombo.IsEditable(true);
      m_entryPointCombo.IsEditable(true);
    }

#if !defined(USE_HERMES)
    rootGrid.FindName(L"x_engineHermes").as<ComboBoxItem>().IsEnabled(false);
#endif

#if !defined(USE_V8)
    rootGrid.FindName(L"x_engineV8").as<ComboBoxItem>().IsEnabled(false);
#endif

    rootGrid.FindName(L"x_JsEngine").as<ComboBox>().SelectedIndex(0);

    auto loadButton = rootGrid.FindName(L"x_LoadButton").as<Button>();

    m_LoadButtonRevoker = loadButton.Click(
        winrt::auto_revoke, [this](auto const & /*sender*/, RoutedEventArgs const & /*args*/) noexcept { Load(); });
  }

  void Load() {
    auto host = Host();
    auto bundleFile = unbox_value<hstring>(m_entryPointCombo.SelectedItem().as<ComboBoxItem>().Content());
    host.InstanceSettings().JavaScriptBundleFile(bundleFile);
    auto mainComponentName = unbox_value<hstring>(m_rootComponentNameCombo.SelectedItem().as<ComboBoxItem>().Content());
    host.InstanceSettings().MainComponentName(mainComponentName);
    host.InstanceSettings().UseWebDebugger(m_UseWebDebuggerCheckBox.IsChecked().GetBoolean());
    host.InstanceSettings().UseDirectDebugger(m_UseDirectDebuggerCheckBox.IsChecked().GetBoolean());
    host.InstanceSettings().DebuggerBreakOnNextLine(m_BreakOnFirstLineCheckBox.IsChecked().GetBoolean());
    host.InstanceSettings().UseFastRefresh(m_BreakOnFirstLineCheckBox.IsChecked().GetBoolean());
    host.InstanceSettings().DebuggerPort(static_cast<uint16_t>(std::stoi(std::wstring(m_DebuggerPort.Text()))));

    // Nudge the ReactNativeHost to create the instance and wrapping context
    host.ReloadInstance();

    m_reactRootView = ReactRootView();
    m_reactRootView.ComponentName(host.InstanceSettings().MainComponentName());
    m_reactRootView.ReactNativeHost(host);

    m_rootElement.Children().Clear();
    m_rootElement.Children().Append(m_reactRootView);
  }

 private:
  Microsoft::ReactNative::ReactNativeHost Host() noexcept {
    if (!m_host) {
      m_host = Microsoft::ReactNative::ReactNativeHost();
      m_host.InstanceSettings(InstanceSettings());
      m_host.PackageProviders(PackageProviders());
    }

    return m_host;
  }

  Microsoft::ReactNative::ReactInstanceSettings InstanceSettings() noexcept {
    if (!m_instanceSettings) {
      m_instanceSettings = ReactInstanceSettings();
    }

    return m_instanceSettings;
  }

  Windows::Foundation::Collections::IVector<Microsoft::ReactNative::IReactPackageProvider> PackageProviders() noexcept {
    if (!m_packageProviders) {
      m_packageProviders = single_threaded_vector<Microsoft::ReactNative::IReactPackageProvider>();
    }

    return m_packageProviders;
  }

  Microsoft::ReactNative::ReactRootView m_reactRootView{nullptr};
  Microsoft::ReactNative::ReactNativeHost m_host{nullptr};
  Microsoft::ReactNative::ReactInstanceSettings m_instanceSettings{nullptr};
  Windows::Foundation::Collections::IVector<Microsoft::ReactNative::IReactPackageProvider> m_packageProviders{nullptr};
  Button::Click_revoker m_LoadButtonRevoker{};
  CheckBox m_UseWebDebuggerCheckBox{nullptr};
  CheckBox m_UseDirectDebuggerCheckBox{nullptr};
  CheckBox m_BreakOnFirstLineCheckBox{nullptr};
  CheckBox m_UseFastRefreshCheckBox{nullptr};

  ComboBox m_entryPointCombo{nullptr};
  ComboBox m_rootComponentNameCombo{nullptr};
  Grid m_rootElement{nullptr};
  TextBox m_DebuggerPort{nullptr};
};

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int) {
  Application::Start([](auto &&) { make<App>(); });
}
