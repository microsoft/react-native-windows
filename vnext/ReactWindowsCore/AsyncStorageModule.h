// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <AsyncStorage/AsyncStorageManager.h>
#include <cxxreact/CxxModule.h>
#include <cxxreact/MessageQueueThread.h>
#include <folly/dynamic.h>

namespace facebook {
namespace react {

class AsyncStorageModule : public facebook::xplat::module::CxxModule {
 public:
  AsyncStorageModule(const WCHAR *storageFileName);
  std::string getName() override;
  std::map<std::string, dynamic> getConstants() override;
  std::vector<facebook::xplat::module::CxxModule::Method> getMethods() override;

 private:
  std::unique_ptr<facebook::react::AsyncStorageManager> m_asyncStorageManager;
};
} // namespace react
} // namespace facebook
