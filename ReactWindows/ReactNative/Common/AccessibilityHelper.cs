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
        #region ImportantForAccessibility

        /// <summary>
        /// Initializes the value of AccessibilityView for <paramref name="uiElement"/> and its children,
        /// based on the ancestor <paramref name="parentUIElement"/> settings.
        /// </summary>
        /// <param name="parentUIElement"></param>
        /// <param name="uiElement"></param>
        public static void InitImportantForAccessibility(UIElement parentUIElement, UIElement uiElement)
        {
            var parentImportantForAccessibilityAttached = GetImportantForAccessibilityAttached(parentUIElement);
            var parentUIElementAutomationPeer = FrameworkElementAutomationPeer.FromElement(parentUIElement);
            if (parentImportantForAccessibilityAttached == ImportantForAccessibility.Yes ||
                parentImportantForAccessibilityAttached == ImportantForAccessibility.NoHideDescendants ||
                IsHiddenByAncestor(parentUIElementAutomationPeer))
            {
                // If an ancestor is "hiding" the element, set AccessibilityView to Raw.
                var uiElementAutomationPeer = FrameworkElementAutomationPeer.FromElement(uiElement);
                AutomationProperties.SetAccessibilityView(uiElement, AccessibilityView.Raw);
                SetChildrenAccessibilityView(uiElementAutomationPeer, AccessibilityView.Raw);
            }
        }

        /// <summary>
        /// Sets the ImportantForAccessibility property for <paramref name="uiElement"/>.
        /// It uses AutomationProperties.AccessibilityView to implement the element and its children
        /// visibility to narrator. ImportantForAccessibility value is stored as an attached property.
        /// </summary>
        /// <param name="uiElement"></param>
        /// <param name="importantForAccessibilityNew"></param>
        public static void SetImportantForAccessibility(UIElement uiElement, ImportantForAccessibility importantForAccessibilityNew)
        {
            var importantForAccessibilityOld = GetImportantForAccessibilityAttached(uiElement);
            if (importantForAccessibilityOld == importantForAccessibilityNew)
            {
                return;
            }

            SetImportantForAccessibilityAttached(uiElement, importantForAccessibilityNew);

            // Check if uiElement has an ancestor that "hides" children. If so, AccessibilityView is
            // already set to "Raw" for all children and no updates are required.
            var uiElementAutomationPeer = FrameworkElementAutomationPeer.FromElement(uiElement);
            if (IsHiddenByAncestor(uiElementAutomationPeer))
            {
                return;
            }

            // Update AccessibilityView property accordingly.
            UpdateAccessibilityViewForUIElement(uiElement, uiElementAutomationPeer, importantForAccessibilityNew);
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
        /// ImportantForAccessibility value of Yes or NoHideDescendants. 'Yes' denotes that the element is visible
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
                        var importantForAccessibilityAttached = GetImportantForAccessibilityAttached(parentFEAutomationPeer.Owner);
                        if (importantForAccessibilityAttached == ImportantForAccessibility.Yes ||
                            importantForAccessibilityAttached == ImportantForAccessibility.NoHideDescendants)
                        {
                            return true;
                        }
                    }
                    result = IsHiddenByAncestor(parentFEAutomationPeer);
                    break;
                case ItemAutomationPeer parentIAutomationPeer:
                    if (parentIAutomationPeer.Item is UIElement parentUIElement)
                    {
                        var importantForAccessibilityAttached = GetImportantForAccessibilityAttached(parentUIElement);
                        if (importantForAccessibilityAttached == ImportantForAccessibility.Yes ||
                            importantForAccessibilityAttached == ImportantForAccessibility.NoHideDescendants)
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

            foreach (AutomationPeer automationPeer in uiElementAutomationPeer.GetChildren())
            {
                UIElement childUIElement = GetUIElementFromAutomationPeer(automationPeer);
                if (childUIElement == null)
                {
                    SetChildrenAccessibilityViewFromImportantForAccessibility(automationPeer);
                    continue;
                }

                var importantForAccessibilityAttached = GetImportantForAccessibilityAttached(childUIElement);
                UpdateAccessibilityViewForUIElement(childUIElement, automationPeer, importantForAccessibilityAttached);
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
                case ImportantForAccessibility.Auto:
                    uiElement.ClearValue(AutomationProperties.AccessibilityViewProperty);
                    SetChildrenAccessibilityViewFromImportantForAccessibility(uiElementAutomationPeer);
                    break;
                case ImportantForAccessibility.No:
                    AutomationProperties.SetAccessibilityView(uiElement, AccessibilityView.Raw);
                    SetChildrenAccessibilityViewFromImportantForAccessibility(uiElementAutomationPeer);
                    break;
                case ImportantForAccessibility.NoHideDescendants:
                    AutomationProperties.SetAccessibilityView(uiElement, AccessibilityView.Raw);
                    SetChildrenAccessibilityView(uiElementAutomationPeer, AccessibilityView.Raw);
                    break;
                case ImportantForAccessibility.Yes:
                    AutomationProperties.SetAccessibilityView(uiElement, AccessibilityView.Content);
                    SetChildrenAccessibilityView(uiElementAutomationPeer, AccessibilityView.Raw);
                    break;
                default:
                    break;
            }
        }

        #endregion ImportantForAccessibility
    }
}
