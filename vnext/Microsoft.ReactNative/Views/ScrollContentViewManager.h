// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Views/ViewViewManager.h>

namespace Microsoft::ReactNative {

class ScrollContentViewManager : public ViewViewManager {
  using Super = ViewViewManager;

 public:
  ScrollContentViewManager(const Mso::React::IReactContext &context);

  const wchar_t *GetName() const override;
};

} // namespace Microsoft::ReactNative
