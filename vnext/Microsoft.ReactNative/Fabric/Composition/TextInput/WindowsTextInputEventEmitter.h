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

  struct OnSubmitEditing {
    int eventCount;
    int target;
    std::string text;
  };

  struct OnKeyPress {
    std::string key;
  };

  struct OnContentSizeChange {
    int target;
    facebook::react::Size contentSize;
  };

  struct OnEndEditing {
    int eventCount;
    int target;
    std::string text;
  };

  void onChange(OnChange value) const;
  void onSelectionChange(const OnSelectionChange &value) const;
  void onSubmitEditing(OnSubmitEditing value) const;
  void onKeyPress(OnKeyPress value) const;
  void onContentSizeChange(OnContentSizeChange value) const;
  void onPressIn(GestureResponderEvent event) const override;
  void onEndEditing(OnEndEditing value) const;
  void onPressOut(GestureResponderEvent event) const override;
  void onScroll(facebook::react::Point offset) const;
};

} // namespace facebook::react
