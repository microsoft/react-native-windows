// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Text;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation;
using Windows.UI.Xaml.Automation.Peers;
using Windows.UI.Xaml.Documents;

namespace ReactNative.Accessibility
{
    /// <summary>
    /// Accessibility helper methods.
    /// </summary>
    public static class AccessibilityHelper
    {
        /// <summary>
        /// Finds the <see cref="UIElement"/> that owns the TextElement.
        /// </summary>
        /// <param name="textElement"></param>
        /// <returns><see cref="UIElement"/> parent of <paramref name="textElement"/>, null otherwise.</returns>
        public static UIElement GetParentElementFromTextElement(TextElement textElement)
        {
            // If the TextElement has not been added to an UIElement yet, accessing ElementStart
            // will throw NotSupportedException.
            DependencyObject parent;
            try
            {
                parent = textElement.ElementStart?.Parent;
            }
            catch (NotSupportedException)
            {
                return null;
            }

            while (parent is TextElement textElementParent)
            {
                try
                {
                    parent = textElementParent.ElementStart?.Parent;
                }
                catch (NotSupportedException)
                {
                    return null;
                }
            }

            if (parent is UIElement parentUIElement)
            {
                return parentUIElement;
            }

            return null;
        }

        /// <summary>
        /// Updates accessibility data in UI tree after <paramref name="child"/> is added to <paramref name="parent"/>
        /// Must be called when a child is added to <paramref name="parent"/>.
        /// </summary>
        /// <param name="parent">The element to which <paramref name="child"/> is added.</param>
        /// <param name="child">The added child.</param>
        public static void OnChildAdded(UIElement parent, DependencyObject child)
        {
            // Call UpdateLayout() on parent to make sure parent/child relationship is updated.
            parent.UpdateLayout();
            UpdateImportantForAccessibilityForAddedChild(parent, child);
            UpdateGeneratedNameHereAndUp(parent);
        }

        /// <summary>
        /// Updates accessibility data in UI tree after a child is removed from <paramref name="parent"/>
        /// Must be called when a child is removed from <paramref name="parent"/>.
        /// </summary>
        /// <param name="parent">The element from which a child is removed.</param>
        public static void OnChildRemoved(UIElement parent)
        {
            // Call UpdateLayout() on parent to make sure parent/child relationship is updated.
            parent.UpdateLayout();
            UpdateGeneratedNameHereAndUp(parent);
        }

        /// <summary>
        /// Updates accessibility data in UI tree after text is changed on a <see cref="TextElement"/>
        /// Must be called after text is changed on a <see cref="TextElement"/>
        /// </summary>
        /// <param name="textElement">The <see cref="TextElement"/>.</param>
        public static void OnTextChanged(TextElement textElement)
        {
            var parentUIElement = GetParentElementFromTextElement(textElement);
            if (parentUIElement != null)
            {
                // Call UpdateLayout() on parent to make sure parent/child relationship is updated.
                parentUIElement.UpdateLayout();
                UpdateGeneratedNameHereAndUp(parentUIElement);
            }
        }

        /// <summary>
        /// The framework calls this method on new view instance creation. The method is
        /// intended for performing necessary accessibility setup (if any) for the newly created views.
        /// </summary>
        /// <param name="view"></param>
        public static void OnViewInstanceCreated(UIElement view)
        {
            // Not all the UIElements will create AutomationPeer immediately after being created (e.g. Border, Canvas).
            // We need to make sure AutomationPeer is always created since the impementation relies on
            // traversing automation tree. Setting AutomationProperties.Name to some string and then clearing it will guarantee that
            // AutomationPeer is always created.
            AutomationProperties.SetName(view, " ");
            view.ClearValue(AutomationProperties.NameProperty);
        }

        /// <summary>
        /// If an UIElement owning the <paramref name="peer"/> can be determined, it is returned,
        /// otherwise returns null.
        /// </summary>
        /// <param name="peer"></param>
        /// <returns></returns>
        private static UIElement GetUIElementFromAutomationPeer(AutomationPeer peer)
        {
            UIElement ret = null;

            switch (peer)
            {
                case FrameworkElementAutomationPeer feAutomationPeer:
                    ret = feAutomationPeer.Owner;
                    break;
                case ItemAutomationPeer itemAutomationPeer:
                    ret = itemAutomationPeer.Item as UIElement;
                    break;
                default:
                    break;
            }

            return ret;
        }

        #region ImportantForAccessibility

        /// <summary>
        /// Sets the ImportantForAccessibility property for <paramref name="uiElement"/>.
        /// It uses AutomationProperties.AccessibilityView to expose the element and its children
        /// to narrator. ImportantForAccessibility value is stored as an attached property.
        /// </summary>
        /// <param name="uiElement">Element which ImportantForAccessibility property is set.</param>
        /// <param name="importantForAccessibility">The new value of ImportantForAccessibility property.</param>
        public static void SetImportantForAccessibility(UIElement uiElement, ImportantForAccessibility importantForAccessibility)
        {
            // Check if property is already set to requested value.
            if (GetImportantForAccessibilityAttached(uiElement) == importantForAccessibility)
            {
                return;
            }
            SetImportantForAccessibilityAttached(uiElement, importantForAccessibility);
            UpdateName(uiElement);
            UpdateAccessibilityViewIfNeeded(uiElement);
        }

        private static void UpdateImportantForAccessibilityForAddedChild(UIElement parent, DependencyObject child)
        {
            var parentPeer = FrameworkElementAutomationPeer.FromElement(parent);
            if (DoesHideDescendants(parent)
                || IsHiddenByAncestor(parentPeer))
            {
                if (child is UIElement childElement)
                {
                    // If an ancestor is "hiding" the element, set AccessibilityView to Raw.
                    AutomationPeer childPeer = FrameworkElementAutomationPeer.FromElement(childElement);
                    AutomationProperties.SetAccessibilityView(childElement, AccessibilityView.Raw);
                    SetChildrenAccessibilityView(childPeer, AccessibilityView.Raw);
                }
                else
                {
                    // If the parent or an ancestor is "hiding" the children, set AccessibilityView to Raw.
                    SetChildrenAccessibilityView(parentPeer, AccessibilityView.Raw);
                }
            }
        }

        /// <summary>
        /// If needed sets AccessibilityView property for <paramref name="element"/> and its children
        /// according to the element's <see cref="ImportantForAccessibilityAttachedProperty"/> and
        /// <see cref="AccessibilityLabelAttachedProperty"/> values.
        /// </summary>
        /// <param name="element"></param>
        private static void UpdateAccessibilityViewIfNeeded(UIElement element)
        {
            // Check if uiElement has an ancestor that "hides" children. If so, AccessibilityView is
            // already set to "Raw" for all children and no updates are required.
            var elementPeer = FrameworkElementAutomationPeer.FromElement(element);
            if (IsHiddenByAncestor(elementPeer))
            {
                return;
            }

            // Update AccessibilityView property accordingly.
            UpdateAccessibilityViewForUIElement(element, elementPeer, GetImportantForAccessibilityAttached(element));
        }

        /// <summary>
        /// Attached property used to store ImportantForAccessibility value in native controls.
        /// </summary>
        private static readonly DependencyProperty ImportantForAccessibilityAttachedProperty =
            DependencyProperty.RegisterAttached(
                "ImportantForAccessibilityAttached",
                typeof(ImportantForAccessibility),
                typeof(UIElement),
                new PropertyMetadata(ImportantForAccessibility.Auto));

        /// <summary>
        /// ImportantForAccessibilityAttached property setter.
        /// </summary>
        /// <param name="element"></param>
        /// <param name="value"></param>
        private static void SetImportantForAccessibilityAttached(UIElement element, ImportantForAccessibility value)
        {
            element.SetValue(ImportantForAccessibilityAttachedProperty, value);
        }

        /// <summary>
        /// ImportantForAccessibilityAttached property getter.
        /// </summary>
        /// <param name="element"></param>
        /// <returns></returns>
        private static ImportantForAccessibility GetImportantForAccessibilityAttached(UIElement element)
        {
            return (ImportantForAccessibility)element.GetValue(ImportantForAccessibilityAttachedProperty);
        }

        /// <summary>
        /// Recursively sets AccessibilityView to <paramref name="accessibilityView"/> for <paramref name="parentPeer"/> children.
        /// </summary>
        /// <param name="parentPeer"></param>
        /// <param name="accessibilityView"></param>
        private static void SetChildrenAccessibilityView(AutomationPeer parentPeer, AccessibilityView accessibilityView)
        {
            if (parentPeer?.GetChildren() == null)
            {
                return;
            }

            foreach (AutomationPeer childPeer in parentPeer.GetChildren())
            {
                UIElement child = GetUIElementFromAutomationPeer(childPeer);
                if (child != null)
                {
                    AutomationProperties.SetAccessibilityView(child, accessibilityView);
                }
                SetChildrenAccessibilityView(childPeer, accessibilityView);
            }
        }

        /// <summary>
        /// Returns true if in <paramref name="elementPeer"/> parents chain there is an UIElement with
        /// ImportantForAccessibility value of Yes or NoHideDescendants or (Auto and AccessibilityLabelAttached attached property is set).
        /// 'Yes' or (Auto and AccessibilityLabelAttached attached property is set) denotes that the element is visible
        /// to the narrator as a container i.e. its children are 'hidden'. 'NoHideDescendants' denotes that both
        /// the element and its children are 'hidden'. 'Hidden' children have AccessibilityView set to 'Raw'.
        /// </summary>
        /// <param name="elementPeer"></param>
        /// <returns></returns>
        private static bool IsHiddenByAncestor(AutomationPeer elementPeer)
        {
            if (elementPeer == null)
            {
                return false;
            }

            bool result;
            switch (elementPeer.Navigate(AutomationNavigationDirection.Parent))
            {
                case FrameworkElementAutomationPeer parentFEAutomationPeer:
                    if (parentFEAutomationPeer.Owner != null)
                    {
                        if (DoesHideDescendants(parentFEAutomationPeer.Owner))
                        {
                            return true;
                        }
                    }
                    result = IsHiddenByAncestor(parentFEAutomationPeer);
                    break;
                case ItemAutomationPeer parentIAutomationPeer:
                    if (parentIAutomationPeer.Item is UIElement parentUIElement)
                    {
                        if (DoesHideDescendants(parentUIElement))
                        {
                            return true;
                        }
                    }
                    result = IsHiddenByAncestor(parentIAutomationPeer);
                    break;
                case AutomationPeer parentAutomationPeer:
                    result = IsHiddenByAncestor(parentAutomationPeer);
                    break;
                case null:
                    result = false;
                    break;
                default:
                    result = false;
                    break;
            }

            return result;
        }

        /// <summary>
        /// Recursively sets the value of AccessibilityView according ImportantForAccessibility setting
        /// for children of <paramref name="elementPeer"/>
        /// </summary>
        /// <param name="elementPeer"></param>
        private static void SetChildrenAccessibilityViewFromImportantForAccessibility(AutomationPeer elementPeer)
        {
            if (elementPeer?.GetChildren() == null)
            {
                return;
            }

            foreach (AutomationPeer childPeer in elementPeer.GetChildren())
            {
                UIElement child = GetUIElementFromAutomationPeer(childPeer);
                if (child == null)
                {
                    SetChildrenAccessibilityViewFromImportantForAccessibility(childPeer);
                }
                else
                {
                    var importantForAccessibilityAttached = GetImportantForAccessibilityAttached(child);
                    UpdateAccessibilityViewForUIElement(child, childPeer, importantForAccessibilityAttached);
                }
            }
        }

        /// <summary>
        /// Sets AccessibilityView property for <paramref name="element"/> and its children according to
        /// the element's <paramref name="importantForAccessibility"/> property and
        /// <see cref="AccessibilityLabelAttachedProperty"/> values.
        /// </summary>
        /// <param name="element"></param>
        /// <param name="elementPeer"></param>
        /// <param name="importantForAccessibility"></param>
        private static void UpdateAccessibilityViewForUIElement(UIElement element, AutomationPeer elementPeer, ImportantForAccessibility importantForAccessibility)
        {
            switch (importantForAccessibility)
            {
                case ImportantForAccessibility.Auto when GetAccessibilityLabelAttached(element) == null:
                    element.ClearValue(AutomationProperties.AccessibilityViewProperty);
                    SetChildrenAccessibilityViewFromImportantForAccessibility(elementPeer);
                    break;
                case ImportantForAccessibility.Auto when GetAccessibilityLabelAttached(element) != null:
                case ImportantForAccessibility.Yes:
                    AutomationProperties.SetAccessibilityView(element, AccessibilityView.Content);
                    SetChildrenAccessibilityView(elementPeer, AccessibilityView.Raw);
                    break;
                case ImportantForAccessibility.No:
                    AutomationProperties.SetAccessibilityView(element, AccessibilityView.Raw);
                    SetChildrenAccessibilityViewFromImportantForAccessibility(elementPeer);
                    break;
                case ImportantForAccessibility.NoHideDescendants:
                    AutomationProperties.SetAccessibilityView(element, AccessibilityView.Raw);
                    SetChildrenAccessibilityView(elementPeer, AccessibilityView.Raw);
                    break;
                default:
                    break;
            }
        }

        private static bool DoesHideDescendants(UIElement element)
        {
            ImportantForAccessibility importantForAccessibility = GetImportantForAccessibilityAttached(element);
            bool isAccessibilityLabelSet = GetAccessibilityLabelAttached(element) != null;

            return importantForAccessibility == ImportantForAccessibility.Yes
                || importantForAccessibility == ImportantForAccessibility.NoHideDescendants
                || (importantForAccessibility == ImportantForAccessibility.Auto && isAccessibilityLabelSet == true);
        }

        #endregion ImportantForAccessibility

        #region AccessibilityLabel

        /// <summary>
        /// Sets the AccessibilityLabel property for <paramref name="element"/>.
        /// It uses <see cref="AutomationProperties.NameProperty"/> to expose the element and its children
        /// to narrator. AccessibilityLabel value is stored as an attached property.
        /// </summary>
        /// <param name="element"></param>
        /// <param name="accessibilityLabel"></param>
        public static void SetAccessibilityLabel(UIElement element, string accessibilityLabel)
        {
            // Check if property is already set to requested value.
            if (GetAccessibilityLabelAttached(element) == accessibilityLabel)
            {
                return;
            }
            SetAccessibilityLabelAttached(element, accessibilityLabel);
            UpdateName(element);
            UpdateAccessibilityViewIfNeeded(element);
        }

        /// <summary>
        /// Updates name on <paramref name="element"/> from <see cref="AccessibilityLabelAttachedProperty"/>
        /// or from generation, and then updates all parents' generated names where update is due.
        /// </summary>
        /// <param name="element"></param>
        private static void UpdateName(UIElement element)
        {
            // Update name on the element itself from AccessibilityLabel or name generation.
            var peer = FrameworkElementAutomationPeer.FromElement(element);
            if (IsInGenerativeState(element))
            {
                var generatedName = GenerateNameFromUpdated(peer);
                AutomationProperties.SetName(element, generatedName);
            }
            else
            {
                var accessibilityLabel = GetAccessibilityLabelAttached(element);
                if (string.IsNullOrEmpty(accessibilityLabel))
                {
                    element.ClearValue(AutomationProperties.NameProperty);
                }
                else
                {
                    AutomationProperties.SetName(element, accessibilityLabel);
                }
            }

            // Update generated names only starting from parent and up.
            var parentPeer = peer.Navigate(AutomationNavigationDirection.Parent) as AutomationPeer;
            if (parentPeer != null)
            {
                UpdateGeneratedNameHereAndUp(parentPeer);
            }

        }

        /// <summary>
        /// Update <see cref="AutomationProperties.NameProperty"/> on <paramref name="element"/>
        /// and all its parents as far up as needed and only when the name is generated. This must be called
        /// when anything influencing generated names has changed in <paramref name="element"/> or somewhere in its subtree.
        /// </summary>
        /// <param name="element"></param>
        private static void UpdateGeneratedNameHereAndUp(UIElement element)
        {
            var peer = FrameworkElementAutomationPeer.FromElement(element);
            UpdateGeneratedNameHereAndUp(peer);
        }

        /// <summary>
        /// Update <see cref="AutomationProperties.NameProperty"/> on <paramref name="peer"/>'s owner
        /// and all its parents as far up as needed and only when the name is generated. This must be called
        /// when anything influencing generated names has changed in <paramref name="peer"/>'s owner or somewhere in its subtree.
        /// </summary>
        /// <param name="peer"></param>
        private static void UpdateGeneratedNameHereAndUp(AutomationPeer peer)
        {
            var current = peer;
            while (current != null)
            {
                UIElement element = GetUIElementFromAutomationPeer(current);
                if (IsElementIgnoresChildrenForName(element))
                {
                    break;
                }
                if (IsInGenerativeState(element))
                {
                    var generatedName = GenerateNameFromUpdated(current);
                    AutomationProperties.SetName(element, generatedName);
                }
                current = current.Navigate(AutomationNavigationDirection.Parent) as AutomationPeer;
            }
        }

        /// <summary>
        /// Checks if <paramref name="element"/> decides its name must be generated.
        /// </summary>
        /// <param name="element"></param>
        /// <returns>True if name should be generated. False otherwise.</returns>
        private static bool IsInGenerativeState(UIElement element)
        {
            var i4a = GetImportantForAccessibilityAttached(element);
            var label = GetAccessibilityLabelAttached(element);
            return i4a == ImportantForAccessibility.Yes && string.IsNullOrEmpty(label);
        }

        /// <summary>
        /// Checks if <paramref name="element"/> decides that its children are not needed
        /// for determining either its own name directly or its part it contributes to generation
        /// of a parent's name. 
        /// </summary>
        /// <param name="element"></param>
        /// <returns>True if element decides to ignore children. False otherwise.</returns>
        private static bool IsElementIgnoresChildrenForName(UIElement element)
        {
            var i4a = GetImportantForAccessibilityAttached(element);
            var label = GetAccessibilityLabelAttached(element);
            var peer = FrameworkElementAutomationPeer.FromElement(element);
            var ownName = peer.GetName();
            return i4a == ImportantForAccessibility.NoHideDescendants
                || (i4a == ImportantForAccessibility.Yes && !string.IsNullOrEmpty(label))
                || (i4a == ImportantForAccessibility.Auto && !string.IsNullOrEmpty(label))
                || (i4a == ImportantForAccessibility.Auto && string.IsNullOrEmpty(label) && string.IsNullOrEmpty(ownName));
        }

        /// <summary>
        /// Generates name for <paramref name="peer"/>.
        /// Takes into account <see cref="AccessibilityLabelAttachedProperty"/>.
        /// Does not take into account <see cref="ImportantForAccessibilityAttachedProperty"/>
        /// on <paramref name="peer"/> itself but does take it into account on children transitively.
        /// This assumes that the caller knows that the <see cref="AccessibilityLabelAttachedProperty"/> on
        /// <paramref name="peer"/> must be taken into account no matter what is the value of
        /// <see cref="ImportantForAccessibilityAttachedProperty"/> on it.
        /// </summary>
        /// <param name="peer">Peer to generate name for.</param>
        /// <returns>The generated name.</returns>
        private static string GenerateNameFromUpdated(AutomationPeer peer)
        {
            var element = GetUIElementFromAutomationPeer(peer);
            var label = GetAccessibilityLabelAttached(element);
            if (!string.IsNullOrEmpty(label))
            {
                return label;
            }
            return GenerateNameFromChildren(peer.GetChildren());
        }

        /// <summary>
        /// Generates name from list of children.
        /// </summary>
        /// <param name="children"></param>
        /// <returns>Generated name.</returns>
        private static string GenerateNameFromChildren(IList<AutomationPeer> children)
        {
            if (children == null)
            {
                return "";
            }

            var result = new StringBuilder();
            foreach (var child in children)
            {
                var childElement = GetUIElementFromAutomationPeer(child);
                var i4a = GetImportantForAccessibilityAttached(childElement);
                var childResult = default(string);
                switch (i4a)
                {
                    case ImportantForAccessibility.NoHideDescendants:
                        // The child is hidden from accessibility along with whole subtree.
                        break;
                    case ImportantForAccessibility.No:
                        // Ignore yourself even if AccessibilityLabel is set, go to children.
                        childResult = GenerateNameFromChildren(child.GetChildren());
                        break;
                    case ImportantForAccessibility.Yes:
                        // AutomationProperties.Name should have been already set as
                        // either generated from subtree, if AccessibilityLabel is not set,
                        // or as value of AccessiblityLabel, if it's set.
                        childResult = AutomationProperties.GetName(childElement);
                        break;
                    case ImportantForAccessibility.Auto:
                        // Priority order is: AccessiblityLabel, control-provided name, children.
                        var label = GetAccessibilityLabelAttached(childElement);
                        if (!string.IsNullOrEmpty(label))
                        {
                            childResult = label;
                        }
                        else
                        {
                            var ownName = child.GetName();
                            if (!string.IsNullOrEmpty(ownName))
                            {
                                childResult = ownName;
                            }
                            else
                            {
                                childResult = GenerateNameFromChildren(child.GetChildren());
                            }
                        }
                        break;
                    default:
                        throw new InvalidOperationException($"Unknown ImportantForAccessibility value [{i4a}]");
                }
                if (!string.IsNullOrWhiteSpace(childResult))
                {
                    if (result.Length > 0)
                    {
                        result.Append(" ");
                    }
                    result.Append(childResult);
                }
            }
            return result.ToString();
        }

        /// <summary>
        /// Attached property used to store AccessibilityLabel value in native controls.
        /// </summary>
        private static readonly DependencyProperty AccessibilityLabelAttachedProperty =
            DependencyProperty.RegisterAttached(
                "AccessibilityLabelAttached",
                typeof(string),
                typeof(UIElement),
                new PropertyMetadata(null));

        /// <summary>
        /// AccessibilityLabelAttached property setter.
        /// </summary>
        /// <param name="element"></param>
        /// <param name="value"></param>
        private static void SetAccessibilityLabelAttached(UIElement element, string value)
        {
            element.SetValue(AccessibilityLabelAttachedProperty, value);
        }

        /// <summary>
        /// AccessibilityLabelAttached property getter.
        /// </summary>
        /// <param name="element"></param>
        /// <returns></returns>
        private static string GetAccessibilityLabelAttached(UIElement element)
        {
            return (string)element.GetValue(AccessibilityLabelAttachedProperty);
        }

        #endregion AccessibilityLabel
    }
}
