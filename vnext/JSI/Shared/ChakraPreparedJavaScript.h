// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "jsi/jsi.h"

#include <memory>
#include <string>
#include <utility>

class ChakraPreparedJavaScript : public facebook::jsi::PreparedJavaScript {
 public:
  ChakraPreparedJavaScript(
      std::string &&sourceUrl,
      const std::shared_ptr<const facebook::jsi::Buffer> &sourceCode,
      const std::shared_ptr<const facebook::jsi::Buffer> &bytecode)
      : m_sourceUrl{std::move(sourceUrl)}, m_sourceCode{sourceCode}, m_bytecode{bytecode} {}

  inline const std::string &SourceUrl() const {
    return m_sourceUrl;
  }

  inline const std::shared_ptr<const facebook::jsi::Buffer> &SourceCode() const {
    return m_sourceCode;
  }

  inline const std::shared_ptr<const facebook::jsi::Buffer> &Bytecode() const {
    return m_bytecode;
  }

 private:
  std::string m_sourceUrl;
  std::shared_ptr<const facebook::jsi::Buffer> m_sourceCode;
  std::shared_ptr<const facebook::jsi::Buffer> m_bytecode;
};
