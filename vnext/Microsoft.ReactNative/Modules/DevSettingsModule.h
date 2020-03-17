// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include <cxxreact/CxxModule.h>
#include <functional/functor.h>

namespace Microsoft::ReactNative {

class DevSettingsModule : public facebook::xplat::module::CxxModule {
 public:
  DevSettingsModule(Mso::VoidFunctor &&reload);

  static const char *name;

  std::string getName() override;
  std::map<std::string, folly::dynamic> getConstants() override;
  std::vector<Method> getMethods() override;

  void reload();
  void setHotLoadingEnabled(bool isHotLoadingEnabled);
  void setIsDebuggingRemotely(bool isDebuggingRemotelyEnabled);
  void setLiveReloadEnabled(bool setLiveReloadEnabled);
  void setProfilingEnabled(bool setProfilingEnabled);
  void toggleElementInspector();

  // iOS only.
  void setIsShakeToShowDevMenuEnabled(bool enabled);

 private:
  Mso::VoidFunctor m_reload;
};

} // namespace Microsoft::ReactNative
