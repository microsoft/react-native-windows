// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// HostingPane.xaml.cpp
// Implementation of the HostingPane class
//

#include "pch.h"

#include "HostingPane.xaml.h"

#include <ReactUWP/ReactUwp.h>

#include <winrt/Windows.Storage.h>
#include <winrt/Windows.UI.Xaml.h>
#include <wrl.h>

#include <Windows.UI.Xaml.h>

#include <codecvt>
#include <folly/dynamic.h>
#include <Tracing.h>
#include <unicode.h>
#include <cxxReact/Instance.h>

#include "CustomViewManager.h"

using namespace WindowsSampleApp;

using namespace Microsoft::WRL;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;

void EnsureExportedFunctions(bool createThings)
{
  // These calls exist purely to make sure that the .def files are exporting all the methods required to consume
  // React.UWP as a DLL

  facebook::react::InitializeTracing(nullptr);

  if (createThings)
  {
    auto queue = react::uwp::CreateWorkerMessageQueue();
    auto instance = react::uwp::CreateReactInstance(nullptr);
    auto instance2 = react::uwp::UnSafeCreateReactInstance(nullptr);
    auto control = react::uwp::CreateReactRootView(react::uwp::XamlView(nullptr), L"componentname", nullptr);
    facebook::react::Instance* pInstance = nullptr;
    pInstance->callJSFunction("m", "f", folly::dynamic());

    // verify folly exports
    folly::dynamic dy = folly::dynamic();
    dy.asDouble();
    dy.asInt();
    dy.asString();
    dy.asBool();
    dy[0];
    dy["test"];
    const folly::dynamic& cdy = dy;
    cdy[0];
    cdy["test"];
  }
}

class SampleCxxModule : public facebook::xplat::module::CxxModule
{
public:
  SampleCxxModule()
  { }
  virtual ~SampleCxxModule() = default;
  static const char* Name;
  std::string getName() override { return Name; }

  std::map<std::string, folly::dynamic> getConstants() override;
  std::vector<Method> getMethods() override;
private:
};

const char* SampleCxxModule::Name = "Sample";

std::map<std::string, folly::dynamic> SampleCxxModule::getConstants()
{
  return {
  { "one", 1 },
  { "two", 2 },
  { "animal", "fox" },
  };
}

std::vector<facebook::xplat::module::CxxModule::Method> SampleCxxModule::getMethods()
{
  return {
    facebook::xplat::module::CxxModule::Method("GetStuff", [this](folly::dynamic args, Callback cb)
  {
    folly::dynamic d = folly::dynamic::object
    ("Message", "Hello World!")
      ("Data", folly::dynamic::array(
        folly::dynamic::object("name", "item1") ("prop1", "foo") ("anotherprop1", "bar"),
        folly::dynamic::object("name", "item2") ("prop2", "bar") ("anotherprop2", "yyyyy"),
        folly::dynamic::object("name", "item3") ("prop3", "baz") ("anotherprop1", "zzz")));

    cb({ d });
  }),
  };
}


class SampleNativeModuleProvider final : public facebook::react::NativeModuleProvider
{
public:
  virtual std::vector<facebook::react::NativeModuleDescription> GetModules(const std::shared_ptr<facebook::react::MessageQueueThread>& defaultQueueThread) override
  {
    std::vector<facebook::react::NativeModuleDescription> modules;
    std::shared_ptr<facebook::react::MessageQueueThread> queue = defaultQueueThread;

    modules.emplace_back(
      SampleCxxModule::Name,
      []() { return std::make_unique<SampleCxxModule>(); },
      queue
      );

    return modules;
  }
};

class SampleViewManagerProvider final : public react::uwp::ViewManagerProvider
{
public:
  virtual std::vector<react::uwp::NativeViewManager> GetViewManagers(const std::shared_ptr<react::uwp::IReactInstance>& instance) override
  {
    std::vector<react::uwp::NativeViewManager> viewManagers;

    viewManagers.emplace_back(std::make_unique<CustomFrameworkElementViewManager>(instance));

    return viewManagers;
  }
};

struct HostingPaneReactInstanceCreator : ::react::uwp::IReactInstanceCreator
{
  HostingPaneReactInstanceCreator(HostingPane^ outer)
  {
    m_wrPane = outer;
  }

  std::shared_ptr<::react::uwp::IReactInstance> getInstance()
  {
    HostingPane^ pane = m_wrPane.Resolve<HostingPane>();
    if (!pane)
      return nullptr;
    return pane->getInstance();
  }

  void markAsNeedsReload()
  {
    HostingPane^ pane = m_wrPane.Resolve<HostingPane>();
    if (pane)
      pane->markAsNeedsReload();
  }

private:
  Platform::WeakReference m_wrPane;
};


HostingPane::HostingPane()
{
  InitializeComponent();

  LoadFilenameSettings();

  m_instanceCreator = std::make_shared< HostingPaneReactInstanceCreator>(this);
}

void HostingPane::AddPaneCommand::set(ICommand^ value)
{
  x_AddPane->Command = value;
  x_AddPane->CommandParameter = this;
}

Windows::UI::Xaml::Input::ICommand^ HostingPane::AddPaneCommand::get()
{
  return x_AddPane->Command;
}

void HostingPane::RemovePaneCommand::set(ICommand^ value)
{
  x_RemovePane->Command = value;
  x_RemovePane->CommandParameter = this;
}

Windows::UI::Xaml::Input::ICommand^ HostingPane::RemovePaneCommand::get()
{
  return x_RemovePane->Command;
}

std::shared_ptr<react::uwp::IReactInstance> HostingPane::getInstance()
{
  if (!m_instance)
  {
    facebook::react::InitializeLogging([](facebook::react::RCTLogLevel logLevel, const char* message)
    {
      OutputDebugStringA(message);
    });

    // Create NativeModuleProvider
    std::shared_ptr<facebook::react::NativeModuleProvider> moduleLoader = std::make_shared<SampleNativeModuleProvider>();
    std::shared_ptr<react::uwp::ViewManagerProvider> viewManagerProvider = std::make_shared<SampleViewManagerProvider>();

    m_instance = react::uwp::CreateReactInstance(moduleLoader, viewManagerProvider);

    react::uwp::ReactInstanceSettings settings;
    settings.UseWebDebugger = x_UseWebDebuggerCheckBox->IsChecked->Value;
    settings.UseLiveReload = x_UseLiveReloadCheckBox->IsChecked->Value;
    settings.LoggingCallback = [](facebook::react::RCTLogLevel logLevel, const char* message)
    {
      OutputDebugStringA("In LoggingCallback");
      OutputDebugStringA(message);
    };
    settings.JsExceptionCallback = [](facebook::react::JSExceptionInfo&& exceptionInfo)
    {
      OutputDebugStringA("in JsExceptionCallback");
    };
    m_instance->Start(m_instance, settings);
    m_instance->loadBundle(facebook::react::unicode::utf16ToUtf8(m_loadedBundleFileName));
  }
  return m_instance;
}

void HostingPane::markAsNeedsReload()
{
  if (m_instance != nullptr)
    m_instance->SetAsNeedsReload();
  m_instance = nullptr;
}

void HostingPane::LoadReactNative()
{
  std::shared_ptr<react::uwp::IXamlRootView> rootView = m_rootView;

  if (!x_ReuseInstanceCheckBox->IsChecked && m_rootView != nullptr)
    m_rootView->GetReactInstance()->SetAsNeedsReload();

  if (m_rootView == nullptr)
  {
    EnsureExportedFunctions(false);

    folly::dynamic initialProps = folly::dynamic::object();

    // Retrieve ABI pointer from C++/CX pointer
    Windows::UI::Xaml::IFrameworkElement^ rootFrameworkElement = RootElement;
    ComPtr<::ABI::Windows::UI::Xaml::IFrameworkElement> spFrameworkElementABI =
      reinterpret_cast<ABI::Windows::UI::Xaml::IFrameworkElement*>(rootFrameworkElement);

    // Create C++/WinRT pointer from ABI pointer
    react::uwp::XamlView xamlView =
      reinterpret_cast<const winrt::Windows::UI::Xaml::FrameworkElement&>(spFrameworkElementABI);

    // Create the root view
    m_rootView = react::uwp::CreateReactRootView(xamlView, m_loadedJSComponentName.c_str(), m_instanceCreator);
    if (m_rootView == nullptr)
      return;

    // Set initial properties since just created
    m_rootView->SetInitialProps(std::move(initialProps));
  }

  m_rootView->SetInstanceCreator(m_instanceCreator);

  m_rootView->AttachRoot();
}

void HostingPane::UpdateUI()
{
  bool rootLoaded = (m_rootView != nullptr);

  x_ReloadButton->IsEnabled = rootLoaded;
  x_UnloadButton->IsEnabled = rootLoaded;
  x_LoadButton->IsEnabled = !x_JavaScriptFilename->Text->IsEmpty();
}

void HostingPane::DetachAndClearRoot()
{
  if (m_rootView != nullptr)
  {
    markAsNeedsReload();
    if (m_rootView != nullptr)
      m_rootView->DetachRoot();

    m_rootView.reset();
  }
}

void HostingPane::OnTextChanged_JavaScriptFilename(
  Platform::Object^ sender,
  Windows::UI::Xaml::Controls::TextChangedEventArgs args)
{
  UpdateUI();
}

void HostingPane::OnLoadClicked(Platform::Object^, Platform::Object^)
{
  DetachAndClearRoot();
  StoreFilenameSettings();
  LoadReactNative();
  UpdateUI();
}

void HostingPane::OnReloadClicked(Platform::Object^, Platform::Object^)
{
  assert(m_rootView != nullptr);

  DetachAndClearRoot();

  CoreWindow::GetForCurrentThread()->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
  {
    LoadReactNative();
    UpdateUI();
  }));
}

void HostingPane::OnUnloadClicked(Platform::Object^, Platform::Object^)
{
  assert(m_rootView != nullptr);

  DetachAndClearRoot();
  UpdateUI();
}

static const wchar_t* c_containerName = L"js";
static const wchar_t* c_filenameSetting = L"filename";
static const wchar_t* c_appnameSetting = L"appname";
static const wchar_t* c_filenameSettingDefault = L"Universal.SampleApp\\index.uwp";
static const wchar_t* c_appnameSettingDefault = L"Bootstrap";

static winrt::Windows::Storage::ApplicationDataContainer GetJsSettings()
{
  auto localSettings = winrt::Windows::Storage::ApplicationData::Current().LocalSettings();
  auto container = localSettings.CreateContainer(
    c_containerName,
    winrt::Windows::Storage::ApplicationDataCreateDisposition::Always);

  return container;
}

void HostingPane::LoadFilenameSettings()
{
  auto container = GetJsSettings();

  winrt::hstring filename;
  if (container.Values().HasKey(c_filenameSetting)) {
    filename = container.Values().Lookup(c_filenameSetting).as<winrt::Windows::Foundation::IPropertyValue>().GetString();
  }
  else {
    // Default
    filename = c_filenameSettingDefault;
  }

  x_JavaScriptFilename->Text = reinterpret_cast<Platform::String^>(winrt::get_abi(filename));

  winrt::hstring appname;
  if (container.Values().HasKey(c_appnameSetting))
  {
    appname = container.Values().Lookup(c_appnameSetting).as<winrt::Windows::Foundation::IPropertyValue>().GetString();
  }
  else
  {
    // Default
    appname = c_appnameSettingDefault;
  }

  x_ReactAppName->Text = reinterpret_cast<Platform::String^>(winrt::get_abi(appname));
}

void HostingPane::StoreFilenameSettings()
{
  auto container = GetJsSettings();

  const wchar_t* currentFilename = x_JavaScriptFilename->Text->Data();
  const wchar_t* currentAppname = x_ReactAppName->Text->Data();

  container.Values().Insert(
    c_filenameSetting,
    winrt::Windows::Foundation::PropertyValue::CreateString(currentFilename));
  container.Values().Insert(
    c_appnameSetting,
    winrt::Windows::Foundation::PropertyValue::CreateString(currentAppname));

  m_loadedBundleFileName = currentFilename;
  m_loadedJSComponentName = currentAppname;
}
