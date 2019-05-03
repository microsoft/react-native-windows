// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <folly/dynamic.h>
#include <cxxreact/CxxModule.h>

namespace react { namespace uwp {

class ClipboardModule final : public facebook::xplat::module::CxxModule
{
public:
  virtual ~ClipboardModule();

  // CxxModule
  std::string getName() override;
  std::map<std::string, folly::dynamic> getConstants() override;
  auto getMethods() -> std::vector<Method> override;

  static const char* name;

private:
  static void SetClipboardText(const std::string& text);
  static void GetClipboardText(const Callback& cbSuccess, const Callback& cbFail);
};

} } // namespace react::uwp
