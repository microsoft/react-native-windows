// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <react/renderer/components/view/ViewEventEmitter.h>

namespace facebook::react {

class WindowsTextInputEventEmitter : public ViewEventEmitter {
 public:
  using ViewEventEmitter::ViewEventEmitter;

  struct Selection {
    int start;
    int end;
  };

  struct OnChange {
    int eventCount;
    int target;
    std::string text;
  };

  struct OnSelectionChange {
    Selection selection;
  };

  void onChange(OnChange value) const;
  void onSelectionChange(const OnSelectionChange &value) const;
};

} // namespace facebook::react
