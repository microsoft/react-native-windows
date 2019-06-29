// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#include "pch.h"

#if !defined(OSS_RN)
#include <cxxreact/Platform.h>
#endif
#include <InstanceFactory.h>
#include <Logging.h>
#include <TestRunner.h>
#include <XamlView.h>//Remove?
#include "UniversalTestInstance.h"

#include <winrt/Windows.UI.Xaml.h>
#include <Windows.UI.Xaml.h>
#include <wrl.h>

#include <concrt.h> // Concurrency namespace.

#include "MainPage.xaml.h"

namespace xaml = Windows::UI::Xaml;

using namespace facebook::react;
using namespace facebook::xplat::module;

using std::make_shared;
using std::make_tuple;
using std::shared_ptr;
using std::string;
using std::tuple;
using std::vector;

namespace {

NativeLoggingHook g_nativeLogHook;

void LogHook(RCTLogLevel logLevel, const char* message)
{
  g_nativeLogHook(logLevel, message);
}

static double nativePerformanceNow()
{
  return std::chrono::duration<double, std::milli>(
    std::chrono::steady_clock::now().time_since_epoch()).count();
}

#if !defined(OSS_RN)
void logMarker(const ReactMarker::ReactMarkerId id, const char* tag)
{
  std::cout << "Marker: " << id;
  if (tag)
    std::cout << " Tag: " << tag;
  std::cout << std::endl;
}
#endif

} // end anonymous namespace

namespace Microsoft::React
{

void InitializeLogging(NativeLoggingHook&& hook)
{
  g_nativeLogHook = std::move(hook);
#if !defined(OSS_RN)
    ReactMarker::logTaggedMarker = logMarker;
#endif
}

namespace Test {

class TestRunnerModuleProvider final : public facebook::react::NativeModuleProvider
{
  vector<tuple<string, CxxModule::Provider>> m_cxxModules;
public:
  TestRunnerModuleProvider(vector<tuple<string, CxxModule::Provider>>&& cxxModules)
    : m_cxxModules { std::move(cxxModules) }
  {
  }

  vector<NativeModuleDescription> GetModules(const shared_ptr<facebook::react::MessageQueueThread>& defaultQueueThread) override
  {
    vector<NativeModuleDescription> result;
    for (auto& t : m_cxxModules)
    {
      result.emplace_back(make_tuple(std::get<0>(t), std::get<1>(t), defaultQueueThread));
    }

    return result;
  }
};

shared_ptr<ITestInstance> TestRunner::GetInstance(
  string&& jsBundleFile,
  vector<tuple<string, CxxModule::Provider>>&& cxxModules,
  shared_ptr<DevSettings> devSettings) noexcept
{

  ::react::uwp::ReactInstanceSettings settings;
  settings.UseWebDebugger = false;
  settings.UseLiveReload = true;

  //TODO: Pass this setting to the instance.
  devSettings->platformName = "uwp";
  //TODO: Fix unresolved externals.
  //::react::uwp::UpdateDevSettings(devSettings->useWebDebugger, /*useLiveReload*/ false, /*reuseReactInstances*/ false);

  shared_ptr<::react::uwp::IReactInstance> instance;
  //shared_ptr<Concurrency::event> completed = make_shared<Concurrency::event>();
  //auto action = Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
  //  Windows::UI::Core::CoreDispatcherPriority::Normal,
  //  ref new Windows::UI::Core::DispatchedHandler([&completed, &instance, jsBundleFile=std::move(jsBundleFile), &cxxModules]()
  //{
    instance = ::react::uwp::CreateReactInstance(make_shared<TestRunnerModuleProvider>(std::move(cxxModules)));
    instance->Start(instance, settings);
    instance->loadBundle(std::move(jsBundleFile));
  //  completed->set();
  //}));
  //completed->wait();

  return shared_ptr<ITestInstance>(new UniversalTestInstance(std::move(instance)));
}

} // namespace Microsoft::React::Test

} // namespace Microsoft::React
