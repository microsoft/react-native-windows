// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//

#pragma once

#include <cxxreact/CxxModule.h>

class SampleCxxModule : public facebook::xplat::module::CxxModule {
 public:
  SampleCxxModule() {}
  virtual ~SampleCxxModule() = default;
  static const char *Name;
  std::string getName() override {
    return Name;
  }

  std::map<std::string, folly::dynamic> getConstants() override;
  std::vector<Method> getMethods() override;

 private:
};
