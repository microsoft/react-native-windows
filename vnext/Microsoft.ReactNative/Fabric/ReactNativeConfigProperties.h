// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <nativeModules.h>
#include <react/config/ReactNativeConfig.h>
#include <string>

namespace Microsoft::ReactNative {

class ReactNativeConfigProperties final : public facebook::react::ReactNativeConfig {
 public:
  ReactNativeConfigProperties(winrt::Microsoft::ReactNative::ReactPropertyBag const &properties);
  virtual ~ReactNativeConfigProperties();

  virtual bool getBool(const std::string &param) const override;
  virtual std::string getString(const std::string &param) const override;
  virtual int64_t getInt64(const std::string &param) const override;
  virtual double getDouble(const std::string &param) const override;

 private:
  winrt::Microsoft::ReactNative::ReactPropertyBag m_properties;
};

} // namespace Microsoft::ReactNative
