using ReactNative.Bridge;
using System;
using System.Collections.Generic;
using System.Text;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation;
using Windows.UI.Xaml.Automation.Peers;

namespace ReactNative.Common
{
    /// <summary>
    /// Accessibility helper methods.
    /// </summary>
    public static class AccessibilityHelper
    {
        #region ImportantForAccessibility - Provides support for ImportantForAccessibility property implementation.

        /// <summary>
        /// Initializes the value of AccessibilityView for all the children of <paramref name="parentUIElement"/>,
        /// based on the <paramref name="parentUIElement"/> and its ancesstor settings. Must be called
        /// when multiple children are initially added to <paramref name="parentUIElement"/>.
        /// </summary>
        /// <param name="parentUIElement">The element whos children will be updated.</param>        
        public static void InitImportantForAccessibility(UIElement parentUIElement)
        {
            // Post on UI thread because the method requires that parent/child
            // relationship is established but XAML does not set the relationship
            // until the next loop after the child is added.
            DispatcherHelpers.RunOnDispatcher(() =>
            {
                InitImportantForAccessibilityInternal(parentUIElement);
            });
        }

        /// <summary>
        /// Initializes the value of AccessibilityView for <paramref name="uiElement"/> and its children,
        /// based on the ancestor <paramref name="parentUIElement"/> settings. Must be called
        /// when <paramref name="uiElement"/> is initially added to the visual tree.
        /// </summary>
        /// <param name="parentUIElement">The element where <paramref name="uiElement"/> is added as child.</param>
        /// <param name="uiElement">The element being added to the visual tree.</param>
        public static void InitImportantForAccessibility(UIElement parentUIElement, UIElement uiElement)
        {
            // Post on UI thread because the method requires that parent/child
            // relationship is established but XAML does not set the relationship
            // until the next loop after the child is added.
            DispatcherHelpers.RunOnDispatcher(() =>
            {
                InitImportantForAccessibilityInternal(parentUIElement, uiElement);
            });
        }

        /// <summary>
        /// Sets the ImportantForAccessibility property for <paramref name="uiElement"/>.
        /// It uses AutomationProperties.AccessibilityView to expose the element and its children
        /// to narrator. ImportantForAccessibility value is stored as an attached property.
        /// </summary>
        /// <param name="uiElement">Element which ImportantForAccessibility property is set.</param>
        /// <param name="importantForAccessibility">The new value of ImportantForAccessibility property.</param>
        public static void SetImportantForAccessibility(UIElement uiElement, ImportantForAccessibility importantForAccessibility)
        {
            // Post on UI thread because the method requires that parent/child
            // relationship is established but XAML does not set the relationship
            // until the next loop after the child is added.
            DispatcherHelpers.RunOnDispatcher(() =>
            {
                SetImportantForAccessibilityInternal(uiElement, importantForAccessibility);
            });
        }

        /// <summary>
        /// Internal implementation of InitImportantForAccessibility.
        /// </summary>
        /// <param name="parentUIElement"></param>
        private static void InitImportantForAccessibilityInternal(UIElement parentUIElement)
        {
            var parentUIElementAutomationPeer = FrameworkElementAutomationPeer.FromElement(parentUIElement);
            if (DoesHideDescendants(parentUIElement) ||
                IsHiddenByAncestor(parentUIElementAutomationPeer))
            {
                // If the parent or an ancestor is "hiding" the children, set AccessibilityView to Raw.
                SetChildrenAccessibilityView(parentUIElementAutomationPeer, AccessibilityView.Raw);
            }
        }

        /// <summary>
        /// Internal implementation of InitImportantForAccessibility.
        /// </summary>
        /// <param name="parentUIElement"></param>
        /// <param name="uiElement"></param>
        private static void InitImportantForAccessibilityInternal(UIElement parentUIElement, UIElement uiElement)
        {
            var parentUIElementAutomationPeer = FrameworkElementAutomationPeer.FromElement(parentUIElement);
            if (DoesHideDescendants(parentUIElement) ||
                IsHiddenByAncestor(parentUIElementAutomationPeer))
            {
                // If an ancestor is "hiding" the element, set AccessibilityView to Raw.
                var uiElementAutomationPeer = FrameworkElementAutomationPeer.FromElement(uiElement);
                AutomationProperties.SetAccessibilityView(uiElement, AccessibilityView.Raw);
                SetChildrenAccessibilityView(uiElementAutomationPeer, AccessibilityView.Raw);
            }
        }

        /// <summary>
        /// Internal implementation of SetImportantForAccessibility.
        /// </summary>
        /// <param name="uiElement"></param>
        /// <param name="importantForAccessibility"></param>
        private static void SetImportantForAccessibilityInternal(UIElement uiElement, ImportantForAccessibility importantForAccessibility)
        {
            // Check if property is already set to requested value.
            if (GetImportantForAccessibilityAttached(uiElement) == importantForAccessibility)
            {
                return;
            }

            SetImportantForAccessibilityAttached(uiElement, importantForAccessibility);

            UpdateNameOn(uiElement);
            UpdateNameOnParentUp(uiElement);
            UpdateAccessibilityViewIfNeeded(uiElement);
        }

        /// <summary>
        /// If needed sets AccessibilityView property for <paramref name="uiElement"/> and its children
        /// according to the element's <see cref="ImportantForAccessibilityAttachedProperty"/> and
        /// <see cref="AccessibilityLabelAttachedProperty"/> values.
        /// </summary>
        /// <param name="uiElement"></param>
        private static void UpdateAccessibilityViewIfNeeded(UIElement uiElement)
        {
            // Check if uiElement has an ancestor that "hides" children. If so, AccessibilityView is
            // already set to "Raw" for all children and no updates are required.
            var uiElementAutomationPeer = FrameworkElementAutomationPeer.FromElement(uiElement);
            if (IsHiddenByAncestor(uiElementAutomationPeer))
            {
                return;
            }

            // Update AccessibilityView property accordingly.
            UpdateAccessibilityViewForUIElement(uiElement, uiElementAutomationPeer, GetImportantForAccessibilityAttached(uiElement));
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
        /// Recursively sets AccessibilityView to <paramref name="accessibilityView"/> for <paramref name="parentAutomationPeer"/> children.
        /// </summary>
        /// <param name="parentAutomationPeer"></param>
        /// <param name="accessibilityView"></param>
        private static void SetChildrenAccessibilityView(AutomationPeer parentAutomationPeer, AccessibilityView accessibilityView)
        {
            if (parentAutomationPeer?.GetChildren() == null)
            {
                return;
            }

            foreach (AutomationPeer childAutomationPeer in parentAutomationPeer.GetChildren())
            {
                UIElement childUIElement = GetUIElementFromAutomationPeer(childAutomationPeer);
                if (childUIElement != null)
                {
                    AutomationProperties.SetAccessibilityView(childUIElement, accessibilityView);
                }
                SetChildrenAccessibilityView(childAutomationPeer, accessibilityView);
            }
        }

        /// <summary>
        /// Returns true if in <paramref name="uiElementAutomationPeer"/> parents chain there is an UIElement with
        /// ImportantForAccessibility value of Yes or NoHideDescendants or (Auto and AccessibilityLabelAttached attached property is set).
        /// 'Yes' or (Auto and AccessibilityLabelAttached attached property is set) denotes that the element is visible
        /// to the narrator as a container i.e. its children are 'hidden'. 'NoHideDescendants' denotes that both
        /// the element and its children are 'hidden'. 'Hidden' children have AccessibilityView set to 'Raw'.
        /// </summary>
        /// <param name="uiElementAutomationPeer"></param>
        /// <returns></returns>
        private static bool IsHiddenByAncestor(AutomationPeer uiElementAutomationPeer)
        {
            if (uiElementAutomationPeer == null)
            {
                return false;
            }

            bool result;
            switch (uiElementAutomationPeer.Navigate(AutomationNavigationDirection.Parent))
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
        /// for children of <paramref name="uiElementAutomationPeer"/>
        /// </summary>
        /// <param name="uiElementAutomationPeer"></param>
        private static void SetChildrenAccessibilityViewFromImportantForAccessibility(AutomationPeer uiElementAutomationPeer)
        {
            if (uiElementAutomationPeer?.GetChildren() == null)
            {
                return;
            }

            foreach (AutomationPeer childAutomationPeer in uiElementAutomationPeer.GetChildren())
            {
                UIElement childUIElement = GetUIElementFromAutomationPeer(childAutomationPeer);
                if (childUIElement == null)
                {
                    SetChildrenAccessibilityViewFromImportantForAccessibility(childAutomationPeer);
                }
                else
                {
                    var importantForAccessibilityAttached = GetImportantForAccessibilityAttached(childUIElement);
                    UpdateAccessibilityViewForUIElement(childUIElement, childAutomationPeer, importantForAccessibilityAttached);
                }
            }
        }

        /// <summary>
        /// If an UIElement owning the <paramref name="automationPeer"/> can be determined, it is returned,
        /// otherwise returns null.
        /// </summary>
        /// <param name="automationPeer"></param>
        /// <returns></returns>
        private static UIElement GetUIElementFromAutomationPeer(AutomationPeer automationPeer)
        {
            UIElement ret = null;

            switch (automationPeer)
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

        /// <summary>
        /// Sets AccessibilityView property for <paramref name="uiElement"/> and its children according to
        /// the element's <paramref name="importantForAccessibility"/> property and
        /// <see cref="AccessibilityLabelAttachedProperty"/> values.
        /// </summary>
        /// <param name="uiElement"></param>
        /// <param name="uiElementAutomationPeer"></param>
        /// <param name="importantForAccessibility"></param>
        private static void UpdateAccessibilityViewForUIElement(UIElement uiElement, AutomationPeer uiElementAutomationPeer, ImportantForAccessibility importantForAccessibility)
        {
            switch (importantForAccessibility)
            {
                case ImportantForAccessibility.Auto when GetAccessibilityLabelAttached(uiElement) == null:
                    uiElement.ClearValue(AutomationProperties.AccessibilityViewProperty);
                    SetChildrenAccessibilityViewFromImportantForAccessibility(uiElementAutomationPeer);
                    break;
                case ImportantForAccessibility.Auto when GetAccessibilityLabelAttached(uiElement) != null:
                case ImportantForAccessibility.Yes:
                    AutomationProperties.SetAccessibilityView(uiElement, AccessibilityView.Content);
                    SetChildrenAccessibilityView(uiElementAutomationPeer, AccessibilityView.Raw);
                    break;
                case ImportantForAccessibility.No:
                    AutomationProperties.SetAccessibilityView(uiElement, AccessibilityView.Raw);
                    SetChildrenAccessibilityViewFromImportantForAccessibility(uiElementAutomationPeer);
                    break;
                case ImportantForAccessibility.NoHideDescendants:
                    AutomationProperties.SetAccessibilityView(uiElement, AccessibilityView.Raw);
                    SetChildrenAccessibilityView(uiElementAutomationPeer, AccessibilityView.Raw);
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

        ////////////////////////// NEW START

        /// <summary>
        /// Sets the AccessibilityLabel property for <paramref name="element"/>.
        /// It uses <see cref="AutomationProperties.NameProperty"/> to expose the element and its children
        /// to narrator. AccessibilityLabel value is stored as an attached property.
        /// </summary>
        /// <param name="element"></param>
        /// <param name="accessibilityLabel"></param>
        public static void SetAccessibilityLabel(UIElement element, string accessibilityLabel)
        {
            // Post on UI thread because the method requires that parent/child
            // relationship is established but XAML does not set the relationship
            // until the next loop after the child is added.
            DispatcherHelpers.RunOnDispatcher(() =>
            {
                SetAccessibilityLabelInternal(element, accessibilityLabel);
            });
        }

        /// <summary>
        /// Update <see cref="AutomationProperties.NameProperty"/> on <paramref name="element"/>
        /// and all its parents as far up as needed. This method should be called when something has
        /// changed in <paramref name="element"/> or somewhere in its subtree that may influence
        /// AccessibilityLabel value for <paramref name="element"/> (and so also for everyone up parent chain).
        /// </summary>
        /// <param name="element"></param>
        public static void UpdateNameFromHereUp(UIElement element)
        {
            // Post on UI thread because the method requires that parent/child
            // relationship is established but XAML does not set the relationship
            // until the next loop after the child is added.
            DispatcherHelpers.RunOnDispatcher(() =>
            {
                UpdateNameFromHereUpInternal(element);
            });
        }

        /// <summary>
        /// Internal implementation of <see cref="SetAccessibilityLabel(UIElement, string)"/>.
        /// </summary>
        /// <param name="element"></param>
        /// <param name="accessibilityLabel"></param>
        private static void SetAccessibilityLabelInternal(UIElement element, string accessibilityLabel)
        {
            // Check if property is already set to requested value.
            if (GetAccessibilityLabelAttached(element) == accessibilityLabel)
            {
                return;
            }

            SetAccessibilityLabelAttached(element, accessibilityLabel);
            UpdateNameOn(element);
            UpdateNameOnParentUp(element);
            UpdateAccessibilityViewIfNeeded(element);
        }

        private static void UpdateNameOnParentUp(UIElement element)
        {
            var peer = FrameworkElementAutomationPeer.FromElement(element);
            var parentPeer = peer.Navigate(AutomationNavigationDirection.Parent) as AutomationPeer;
            if (parentPeer != null)
            {
                UpdateNameFromHereUpInternal(parentPeer);
            }
        }

        private static void UpdateNameFromHereUpInternal(UIElement element)
        {
            var peer = FrameworkElementAutomationPeer.FromElement(element);
            UpdateNameFromHereUpInternal(peer);
        }

        private static void UpdateNameFromHereUpInternal(AutomationPeer peer)
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

        private static void UpdateNameOn(UIElement element)
        {
            var peer = FrameworkElementAutomationPeer.FromElement(element);
            UpdateNameOn(peer);
        }

        private static void UpdateNameOn(AutomationPeer peer)
        {
            var element = GetUIElementFromAutomationPeer(peer);
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
        }

        private static bool IsInGenerativeState(UIElement element)
        {
            var i4a = GetImportantForAccessibilityAttached(element);
            var label = GetAccessibilityLabelAttached(element);
            return i4a == ImportantForAccessibility.Yes && string.IsNullOrEmpty(label);
        }

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

        ////////////////////////// NEW END



        ///// <summary>
        ///// Internal implementation of <see cref="UpdateAccessibilityNameFromHereUp(UIElement)"/>
        ///// </summary>
        ///// <param name="element"></param>
        //private static void UpdateAccessibilityNameFromHereUpInternal(UIElement element)
        //{
        //    var peer = FrameworkElementAutomationPeer.FromElement(element);
        //    UpdateAccessibilityNameFromHereUpInternal(peer);
        //}

        ///// <summary>
        ///// Update <see cref="AutomationProperties.NameProperty"/> on owner of <paramref name="peer"/>
        ///// and all its parents as far up as needed. This method should be called when something has
        ///// changed in owner of <paramref name="peer"/> or somewhere in its subtree that may influence
        ///// AccessibilityLabel value for owner <paramref name="peer"/> (and so also for everyone up parent chain).
        ///// </summary>
        ///// <param name="peer"></param>
        //private static void UpdateAccessibilityNameFromHereUpInternal(AutomationPeer peer)
        //{
        //    var current = peer;
        //    while (current != null)
        //    {
        //        var element = GetUIElementFromAutomationPeer(current);
        //        if (element != null)
        //        {
        //            var name = GenerateName(current);
        //            AutomationProperties.SetName(element, name);
        //        }
        //        current = current.Navigate(AutomationNavigationDirection.Parent) as AutomationPeer;
        //    }
        //}

        ///// <summary>
        ///// Generates name to be presented to an UIA client (e.g. a screen reader) for given <paramref name="peer"/>
        ///// according to rules for <see cref="ImportantForAccessibility.Yes"/>, which is to recursively join generated
        ///// names of all children with space (" ") between them, while using AccessibilityLabel or control's natively
        ///// provided accessible name (in that order) if available.
        ///// </summary>
        ///// <remarks>
        ///// This is not recursive relying on fact that by construction each time a change potentially influencing
        ///// the name will trigger updating all affected elements. So this method assumes children already have up
        ///// to date name stored as <see cref="AutomationProperties.NameProperty"/>.
        ///// </remarks>
        ///// <param name="peer"></param>
        ///// <returns>Returns the generated name.</returns>
        //private static string GenerateName(AutomationPeer peer)
        //{
        //    UIElement element = GetUIElementFromAutomationPeer(peer);
        //    if (element != null)
        //    {
        //        var label = GetAccessibilityLabelAttached(element);
        //        if (label != null)
        //        {
        //            return label;
        //        }
        //    }

        //    var generatedName = new StringBuilder();
        //    var children = peer.GetChildren();
        //    if (children != null)
        //    {
        //        foreach(var child in children)
        //        {
        //            var childElement = GetUIElementFromAutomationPeer(child);
        //            if (childElement != null
        //                && GetImportantForAccessibilityAttached(childElement) == ImportantForAccessibility.NoHideDescendants)
        //            {
        //                continue;
        //            }

        //            if (generatedName.Length > 0)
        //            {
        //                generatedName.Append(" ");
        //            }
        //            generatedName.Append(child.GetName());
        //        }
        //    }
        //    return generatedName.ToString();
        //}

        #endregion AccessibilityLabel
    }
}
