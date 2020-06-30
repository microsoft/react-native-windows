// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/CxxModule.h>

namespace react {
namespace windows {

struct II18nModule {
  virtual ~II18nModule(){};
  virtual std::string getLocaleIdentifier() const = 0;
  virtual bool getIsRTL() const = 0;
  virtual void setAllowRTL(bool allowRTL) = 0;
  virtual void setForceRTL(bool forceRTL) = 0;
};

std::unique_ptr<facebook::xplat::module::CxxModule> createI18nModule(std::unique_ptr<II18nModule> module);
} // namespace windows
} // namespace react
