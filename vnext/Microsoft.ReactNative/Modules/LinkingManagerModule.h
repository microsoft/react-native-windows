// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/CxxModule.h>
#include <folly/dynamic.h>

namespace react::uwp {

struct LinkingManagerModule final : facebook::xplat::module::CxxModule {
  LinkingManagerModule() noexcept;
  ~LinkingManagerModule() noexcept override;

  static void OpenUri(winrt::Windows::Foundation::Uri const &uri) noexcept;
  void HandleOpenUri(winrt::hstring const &uri) noexcept;

 public: // CxxModule
  std::string getName() override;
  std::map<std::string, folly::dynamic> getConstants() override;
  std::vector<Method> getMethods() override;

  static const char *name;

 private:
  static std::mutex s_mutex;
  static winrt::Windows::Foundation::Uri s_initialUri;
  static std::vector<LinkingManagerModule *> s_linkingModules;
};

} // namespace react::uwp
