// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "../Microsoft.ReactNative/Fabric/Composition/SelectionItemAutomationEvent.h"

namespace ReactNativeIntegrationTests {

namespace {

struct SelectionNode {
  bool mounted{true};
  bool selectionContainer{false};
  std::optional<bool> selected;
  std::vector<SelectionNode *> children;
};

std::vector<SelectionNode *> GetSelectedItems(SelectionNode &selectionContainer) {
  return winrt::Microsoft::ReactNative::implementation::GetSelectedItemsInSelectionContainer(
      &selectionContainer,
      [](const auto node) -> const auto & { return node->children; },
      [](const auto node) { return node->mounted; },
      [](const auto node) { return node->selectionContainer; },
      [](const auto node) { return node->selected.value_or(false); });
}

} // namespace

TEST_CLASS (UiaHelpersTests) {
  TEST_METHOD(UnmountedSelectionItemStateChangesAreSuppressed) {
    TestCheck(!winrt::Microsoft::ReactNative::implementation::ShouldRaiseSelectionItemStateChanged(
        false, std::nullopt, true));
    TestCheck(!winrt::Microsoft::ReactNative::implementation::ShouldRaiseSelectionItemStateChanged(false, false, true));
    TestCheck(!winrt::Microsoft::ReactNative::implementation::ShouldRaiseSelectionItemStateChanged(false, true, false));
  }

  TEST_METHOD(MountedSelectionItemStateChangesUseMissingAsFalse) {
    TestCheck(winrt::Microsoft::ReactNative::implementation::ShouldRaiseSelectionItemStateChanged(true, false, true));
    TestCheck(winrt::Microsoft::ReactNative::implementation::ShouldRaiseSelectionItemStateChanged(true, true, false));
    TestCheck(
        winrt::Microsoft::ReactNative::implementation::ShouldRaiseSelectionItemStateChanged(true, std::nullopt, true));
    TestCheck(
        winrt::Microsoft::ReactNative::implementation::ShouldRaiseSelectionItemStateChanged(true, true, std::nullopt));
    TestCheck(!winrt::Microsoft::ReactNative::implementation::ShouldRaiseSelectionItemStateChanged(true, false, false));
    TestCheck(!winrt::Microsoft::ReactNative::implementation::ShouldRaiseSelectionItemStateChanged(
        true, std::nullopt, false));
    TestCheck(!winrt::Microsoft::ReactNative::implementation::ShouldRaiseSelectionItemStateChanged(
        true, false, std::nullopt));
    TestCheck(!winrt::Microsoft::ReactNative::implementation::ShouldRaiseSelectionItemStateChanged(
        true, std::nullopt, std::nullopt));
  }

  TEST_METHOD(SelectedItemsComeFromMountedHierarchyAndStopAtNestedContainers) {
    SelectionNode directSelected;
    directSelected.selected = true;
    SelectionNode directUnselected;
    directUnselected.selected = false;
    SelectionNode unspecifiedSelection;
    SelectionNode wrappedSelected;
    wrappedSelected.selected = true;
    SelectionNode unmountedSelected;
    unmountedSelected.mounted = false;
    unmountedSelected.selected = true;
    SelectionNode nestedSelected;
    nestedSelected.selected = true;
    SelectionNode nestedContainer;
    nestedContainer.selectionContainer = true;
    nestedContainer.selected = true;
    nestedContainer.children = {&nestedSelected};
    SelectionNode wrapper;
    wrapper.children = {&wrappedSelected, &unmountedSelected, &nestedContainer};
    SelectionNode root;
    root.selectionContainer = true;
    root.children = {&directSelected, &directUnselected, &unspecifiedSelection, &wrapper};

    auto selectedItems = GetSelectedItems(root);

    TestCheckEqual(size_t{3}, selectedItems.size());
    TestCheckEqual(&directSelected, selectedItems[0]);
    TestCheckEqual(&wrappedSelected, selectedItems[1]);
    TestCheckEqual(&nestedContainer, selectedItems[2]);
  }

  TEST_METHOD(SelectedItemInSingleSelectionContainerRaisesElementSelected) {
    TestCheckEqual(
        UIA_SelectionItem_ElementSelectedEventId,
        winrt::Microsoft::ReactNative::implementation::GetSelectionItemAutomationEventId(true, false, 1));
  }

  TEST_METHOD(FirstSelectedItemInMultiSelectionContainerRaisesElementSelected) {
    TestCheckEqual(
        UIA_SelectionItem_ElementSelectedEventId,
        winrt::Microsoft::ReactNative::implementation::GetSelectionItemAutomationEventId(true, true, 1));
  }

  TEST_METHOD(AdditionalSelectedItemInMultiSelectionContainerRaisesElementAdded) {
    TestCheckEqual(
        UIA_SelectionItem_ElementAddedToSelectionEventId,
        winrt::Microsoft::ReactNative::implementation::GetSelectionItemAutomationEventId(true, true, 2));
  }

  TEST_METHOD(RemovedItemRaisesElementRemoved) {
    TestCheckEqual(
        UIA_SelectionItem_ElementRemovedFromSelectionEventId,
        winrt::Microsoft::ReactNative::implementation::GetSelectionItemAutomationEventId(false, true, 2));
  }

  TEST_METHOD(RemovedItemInSingleSelectionContainerRaisesElementRemoved) {
    TestCheckEqual(
        UIA_SelectionItem_ElementRemovedFromSelectionEventId,
        winrt::Microsoft::ReactNative::implementation::GetSelectionItemAutomationEventId(false, false, 1));
  }

  TEST_METHOD(RemovalLeavingOneSelectedItemRaisesElementSelected) {
    TestCheckEqual(
        UIA_SelectionItem_ElementSelectedEventId,
        winrt::Microsoft::ReactNative::implementation::GetSelectionItemAutomationEventId(false, true, 1));
  }
};

} // namespace ReactNativeIntegrationTests
