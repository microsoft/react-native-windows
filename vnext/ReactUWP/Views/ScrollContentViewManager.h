// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <Views/ViewViewManager.h>

namespace react {
namespace uwp {

class ScrollContentViewManager : public ViewViewManager {
  using Super = ViewViewManager;

 public:
  ScrollContentViewManager(const std::shared_ptr<IReactInstance> &reactInstance);

  const char *GetName() const override;
};

} // namespace uwp
} // namespace react
