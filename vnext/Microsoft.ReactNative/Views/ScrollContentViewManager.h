// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Views/ViewViewManager.h>

namespace react::uwp {

class ScrollContentViewManager : public ViewViewManager {
  using Super = ViewViewManager;

 public:
  ScrollContentViewManager(const std::shared_ptr<IReactInstance> &reactInstance);

  const char *GetName() const override;
};

} // namespace react::uwp
