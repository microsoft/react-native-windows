// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//

#include "pch.h"
#include "HeadlessJsTaskSupport.h"
#include "winrt/Windows.ApplicationModel.Background.h"

const char *HeadlessJsTaskSupport::Name = "HeadlessJsTaskSupport";

winrt::Windows::ApplicationModel::Background::BackgroundTaskDeferral
    HeadlessJsTaskSupport::s_taskDefferal{nullptr};

namespace winrt {
using namespace winrt::Windows::ApplicationModel::Background;
}

std::string HeadlessJsTaskSupport::getName() {
  return Name;
}

void HeadlessJsTaskSupport::registerNativeJsTaskHook(
    const folly::dynamic &args) {
  auto taskName = args[0]["taskName"].asString();
  auto runInterval = static_cast<uint32_t>(args[0]["runInterval"].asInt());

  auto taskRegistrations = winrt::BackgroundTaskRegistration::AllTasks();

  for (const auto &taskRegistration : taskRegistrations) {
    // Assuming one background task. Just un-register all tasks when registering
    // a new one. This prevents needing to worry about the name of the task
    // changing and leaving zombie tasks.
    taskRegistration.Value().Unregister(false);
  }

  // The minimum runInterval is 15 minutes. Calling Register with a lower
  // runInterval throws so just skip it.
  if (runInterval >= 15) {
    winrt::BackgroundTaskBuilder builder;
    builder.Name(winrt::to_hstring(taskName));
    builder.SetTrigger(winrt::TimeTrigger(runInterval, false));
    auto registration = builder.Register();
  }
}

/*static*/ void HeadlessJsTaskSupport::nativeJsTaskEntry(
    const winrt::Windows::ApplicationModel::Background::IBackgroundTaskInstance
        &nativeTaskInstance,
    std::shared_ptr<facebook::react::Instance> instance) {
  s_taskDefferal = nativeTaskInstance.GetDeferral();
  auto taskName = winrt::to_string(nativeTaskInstance.Task().Name());
  instance->callJSFunction(
      "AppRegistry",
      "startHeadlessTask",
      folly::dynamic::array(/* taskId */ 1, /* taskKey */ taskName));
}

std::map<std::string, folly::dynamic> HeadlessJsTaskSupport::getConstants() {
  return {};
}

std::vector<facebook::xplat::module::CxxModule::Method>
HeadlessJsTaskSupport::getMethods() {
  return {
      facebook::xplat::module::CxxModule::Method{
          "notifyTaskFinished",
          [this](folly::dynamic /* args */) { s_taskDefferal.Complete(); }},
      facebook::xplat::module::CxxModule::Method{
          "registerNativeJsTaskHook",
          [this](folly::dynamic args) { registerNativeJsTaskHook(args); }},
  };
}
