// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//

#pragma once
#include <ReactUWP/ReactUwp.h>

struct HeadlessReactInstanceCreator : react::uwp::IReactInstanceCreator {
  static std::shared_ptr<HeadlessReactInstanceCreator> get();

  HeadlessReactInstanceCreator() = default;

  std::shared_ptr<react::uwp::IReactInstance> getInstance();

  void persistUseWebDebugger(bool /*useWebDebugger*/);

  void persistUseLiveReload(bool /*useLiveReload*/);

  void markAsNeedsReload();

 private:
  std::shared_ptr<react::uwp::IReactInstance> m_instance;
  static std::shared_ptr<HeadlessReactInstanceCreator>
      s_headlessReactInstanceCreator;
};
