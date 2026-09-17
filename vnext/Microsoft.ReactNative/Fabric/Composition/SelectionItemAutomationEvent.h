// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <UIAutomationClient.h>
#include <cstddef>
#include <functional>
#include <optional>
#include <vector>

namespace winrt::Microsoft::ReactNative::implementation {

inline bool ShouldRaiseSelectionItemStateChanged(
    bool isMounted,
    const std::optional<bool> &oldSelected,
    const std::optional<bool> &newSelected) noexcept {
  return isMounted && oldSelected.value_or(false) != newSelected.value_or(false);
}

template <typename Node, typename GetChildren, typename IsMounted, typename IsSelectionContainer, typename IsSelected>
std::vector<Node> GetSelectedItemsInSelectionContainer(
    const Node &selectionContainer,
    GetChildren &&getChildren,
    IsMounted &&isMounted,
    IsSelectionContainer &&isSelectionContainer,
    IsSelected &&isSelected) {
  std::vector<Node> selectedItems;
  std::function<void(const Node &)> visitChildren = [&](const Node &parent) {
    for (const auto &child : getChildren(parent)) {
      if (!isMounted(child)) {
        continue;
      }

      if (isSelected(child)) {
        selectedItems.push_back(child);
      }

      if (!isSelectionContainer(child)) {
        visitChildren(child);
      }
    }
  };

  visitChildren(selectionContainer);
  return selectedItems;
}

inline EVENTID
GetSelectionItemAutomationEventId(bool isSelected, bool canSelectMultiple, size_t selectedItemCount) noexcept {
  if (isSelected && (!canSelectMultiple || selectedItemCount == 1)) {
    return UIA_SelectionItem_ElementSelectedEventId;
  }

  if (isSelected) {
    return UIA_SelectionItem_ElementAddedToSelectionEventId;
  }

  return canSelectMultiple && selectedItemCount == 1 ? UIA_SelectionItem_ElementSelectedEventId
                                                     : UIA_SelectionItem_ElementRemovedFromSelectionEventId;
}

inline bool
ShouldRaiseSelectionItemNotification(bool isSelected, bool canSelectMultiple, bool hasKeyboardFocus) noexcept {
  return isSelected && !canSelectMultiple && hasKeyboardFocus;
}

} // namespace winrt::Microsoft::ReactNative::implementation
