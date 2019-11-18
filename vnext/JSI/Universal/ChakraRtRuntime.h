// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#ifndef CHAKRACORE

#include "ChakraRuntime.h"
#include "ChakraRuntimeArgs.h"

#include <memory>
#include <string>

namespace Microsoft::JSI {

class ChakraRtRuntime : public ChakraRuntime {
 public:
  using ChakraRuntime::ChakraRuntime;
  ~ChakraRtRuntime() noexcept = default;

  // Functions inherited from jsi::Runtime =====================================

  facebook::jsi::Value evaluatePreparedJavaScript(
      const std::shared_ptr<const facebook::jsi::PreparedJavaScript> &preparedJs) override;

  std::string description() override;

 private:
  facebook::jsi::WeakObject createWeakObject(const facebook::jsi::Object &obj) override;
  facebook::jsi::Value lockWeakObject(const facebook::jsi::WeakObject &weakObj) override;

  //============================================================================

  // Functions inherited from ChakraRuntime ====================================

  facebook::jsi::JSRuntimeVersion_t Version() override;

  facebook::jsi::Value EvaluateJavaScriptSimple(
      const std::shared_ptr<const facebook::jsi::Buffer> &buffer,
      const std::string &sourceUrl) override;

  std::shared_ptr<const facebook::jsi::Buffer> SerializeScript(
      const std::shared_ptr<const facebook::jsi::Buffer> &buffer) override;

  void StartDebugging() override;

  //============================================================================

  facebook::jsi::Value EvaluateJavaScriptImpl(const wchar_t *sourceCode, const wchar_t *sourceUrl);
};

} // namespace Microsoft::JSI

#endif
