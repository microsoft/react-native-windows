// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//

#pragma once
#include <ReactUWP/ReactUwp.h>

/// This instance creator exists to demonstrate starting a React Instance while
/// running an in-process background task, and transitioning that instance to
/// run in the forground.
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
