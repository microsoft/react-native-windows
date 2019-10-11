// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//

#pragma once
#include <ReactUWP/ReactUwp.h>

class SampleNativeModuleProvider final
    : public facebook::react::NativeModuleProvider {
 public:
  virtual std::vector<facebook::react::NativeModuleDescription> GetModules(
      const std::shared_ptr<facebook::react::MessageQueueThread>
          &defaultQueueThread) override;
};
