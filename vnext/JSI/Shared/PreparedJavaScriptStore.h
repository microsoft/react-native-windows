// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <memory>
#include <string>

namespace facebook::jsi {
class PreparedJavaScript;
}

namespace Microsoft::JSI {

/**
 * TODO (yicyao)
 */
class PreparedJavaScriptStore {
 public:
  virtual ~PreparedJavaScriptStore() = 0;

  /**
   * TODO (yicyao)
   */
  virtual std::shared_ptr<const facebook::jsi::PreparedJavaScript> getPreparedJavaScript(
      const std::string &bundleId) = 0;

  /**
   * TODO (yicyao)
   */
  virtual bool persistPreparedJavaScript(const std::string &bundleId) = 0;
};

} // namespace Microsoft::JSI
