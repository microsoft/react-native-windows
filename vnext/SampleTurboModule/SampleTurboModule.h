#pragma once
#include <TurboModuleManager.h>
#include <turbomodule/samples/SampleTurboCxxModule.h>

extern "C" facebook::react::TurboModule *CreateDynamicTurboModule(
    const std::shared_ptr<facebook::react::CallInvoker> &invoker) {
  return new facebook::react::SampleTurboCxxModule(invoker);
}
