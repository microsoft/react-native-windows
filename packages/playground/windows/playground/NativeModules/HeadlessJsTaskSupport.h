// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//

#pragma once

#include <ReactUWP/ReactUwp.h>
#include <cxxReact/CxxModule.h>
#include <cxxReact/Instance.h>
#include <folly/dynamic.h>

/// Native Module to communicate between javascript background task and UWP
/// BackgroundTaskInstance. Essentially this is a little bit of glue to connect
/// these two concepts
/// https://docs.microsoft.com/en-us/windows/uwp/launch-resume/support-your-app-with-background-tasks
/// https://facebook.github.io/react-native/docs/headless-js-android
/// Two responsibilities here.
/// - Expose a function to JS for registering a background task
/// - Serve as the entry point when the background task runs.
/// --> When the task runs we need to grab the defferal from the task object,
/// and complete the deferral when JS notifies that we are done. NOTE: This
/// implementation assumes that there is only one background task being
/// registered.
///		  If you need to extend this to support multiple background tasks
///then you will need to keep track of their deferrals separately. (Think a map
///of taskId to deferral)
class HeadlessJsTaskSupport : public facebook::xplat::module::CxxModule {
  // Constants
 public:
  static const char *NAME;

  // Methods
 public:
  HeadlessJsTaskSupport() = default;
  virtual ~HeadlessJsTaskSupport() = default;
  std::string getName() override;
  std::map<std::string, folly::dynamic> getConstants() override;
  std::vector<Method> getMethods() override;

  void registerNativeJsTaskHook(const folly::dynamic &args);

  static void nativeJsTaskEntry(
      const winrt::Windows::ApplicationModel::Background::
          IBackgroundTaskInstance &nativeTaskInstance,
      std::shared_ptr<facebook::react::Instance> instance);

  // Member variables
 private:
  static winrt::Windows::ApplicationModel::Background::BackgroundTaskDeferral
      s_taskDefferal;
};
