using ReactNative.Bridge;
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

            // Check if uiElement has an ancestor that "hides" children. If so, AccessibilityView is
            // already set to "Raw" for all children and no updates are required.
            var uiElementAutomationPeer = FrameworkElementAutomationPeer.FromElement(uiElement);
            if (IsHiddenByAncestor(uiElementAutomationPeer))
            {
                return;
            }

            // Update AccessibilityView property accordingly.
            UpdateAccessibilityViewForUIElement(uiElement, uiElementAutomationPeer, importantForAccessibility);
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
                case FrameworkElementAutomationPeer FEAutomationPeer:
                    ret = FEAutomationPeer.Owner;
                    break;
                case ItemAutomationPeer IAutomationPeer:
                    ret = IAutomationPeer.Item as UIElement;
                    break;
                default:
                    break;
            }

            return ret;
        }

        /// <summary>
        /// Sets AccessibilityView property for <paramref name="uiElement"/> and its children according <paramref name="importantForAccessibility"/>.
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
