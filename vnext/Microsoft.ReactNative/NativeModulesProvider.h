#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <NativeModuleProvider.h>
#include <winrt/Microsoft.ReactNative.Bridge.h>
#include <winrt/Microsoft.ReactNative.h>
#include "NativeModuleBase.h"
#include "ReactSupport.h"

using namespace winrt;
using namespace Microsoft::ReactNative;
using namespace Windows::Foundation;

namespace winrt::Microsoft::ReactNative::Bridge {

class NativeModulesProvider final
    : public facebook::react::NativeModuleProvider {
 public:
  virtual std::vector<facebook::react::NativeModuleDescription> GetModules(
      const std::shared_ptr<facebook::react::MessageQueueThread>
          &defaultQueueThread) override;
  void RegisterModule(NativeModuleBase const &module);

 private:
  std::vector<NativeModuleBase> m_modules;
  std::shared_ptr<facebook::react::MessageQueueThread> m_modulesWorkerQueue{
      nullptr};
};
} // namespace winrt::Microsoft::ReactNative::Bridge
