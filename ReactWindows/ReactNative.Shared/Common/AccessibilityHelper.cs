using ReactNative.UIManager;
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
        /// Sets the ImportantForAccessibility property for uiElement.
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

            var uiElementAutomationPeer = FrameworkElementAutomationPeer.FromElement(uiElement);
            // If this element is 'hidden' by a parrent, do not update AccessibilityView property.
            if (IsHiddenElement(uiElementAutomationPeer))
            {
                return;
            }

            // Update AccessibilityView property accordingly.
            switch (importantForAccessibilityNew)
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

        /// <summary>
        /// Attached property used to store ImportantForAccessibility value in native controls.
        /// </summary>
        public static readonly DependencyProperty ImportantForAccessibilityAttachedProperty = DependencyProperty.RegisterAttached(
                                                                                                "ImportantForAccessibilityAttached",
                                                                                                typeof(ImportantForAccessibility),
                                                                                                typeof(UIElement),
                                                                                                new PropertyMetadata(ImportantForAccessibility.Auto));

        /// <summary>
        /// ImportantForAccessibilityAttached property setter.
        /// </summary>
        /// <param name="element"></param>
        /// <param name="value"></param>
        public static void SetImportantForAccessibilityAttached(UIElement element, ImportantForAccessibility value)
        {
            element.SetValue(ImportantForAccessibilityAttachedProperty, value);
        }

        /// <summary>
        /// ImportantForAccessibilityAttached property getter.
        /// </summary>
        /// <param name="element"></param>
        /// <returns></returns>
        public static ImportantForAccessibility GetImportantForAccessibilityAttached(UIElement element)
        {
            return (ImportantForAccessibility)element.GetValue(ImportantForAccessibilityAttachedProperty);
        }

        /// <summary>
        /// Recursively sets AccessibilityView to accessibilityView for parentAutomationPeer children.
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
                if (childAutomationPeer is FrameworkElementAutomationPeer childFEAutomationPeer)
                {
                    if (childFEAutomationPeer?.Owner != null)
                    {
                        AutomationProperties.SetAccessibilityView(childFEAutomationPeer.Owner, accessibilityView);
                    }
                }
                else if (childAutomationPeer is ItemAutomationPeer childIAutomationPeer)
                {
                    if (childIAutomationPeer?.Item is UIElement childUIElement)
                    {
                        AutomationProperties.SetAccessibilityView(childUIElement, accessibilityView);
                    }
                }

                SetChildrenAccessibilityView(childAutomationPeer, accessibilityView);
            }
        }

        /// <summary>
        /// Returns true if in uiElementAutomationPeer parents chain there is an UIElement with
        /// ImportantForAccessibility value of Yes or NoHideDescendants.
        /// </summary>
        /// <param name="uiElementAutomationPeer"></param>
        /// <returns></returns>
        private static bool IsHiddenElement(AutomationPeer uiElementAutomationPeer)
        {
            if (uiElementAutomationPeer == null)
            {
                return false;
            }

            ImportantForAccessibility importantForAccessibilityAttached = ImportantForAccessibility.Auto;
            var parentAutomationPeer = uiElementAutomationPeer.Navigate(AutomationNavigationDirection.Parent) as AutomationPeer;
            if (parentAutomationPeer is FrameworkElementAutomationPeer parentFEAutomationPeer)
            {
                if (parentFEAutomationPeer?.Owner != null)
                {
                    importantForAccessibilityAttached = GetImportantForAccessibilityAttached(parentFEAutomationPeer.Owner);
                }
            }
            else if (parentAutomationPeer is ItemAutomationPeer parentIAutomationPeer)
            {
                if (parentIAutomationPeer?.Item is UIElement parentUIElement)
                {
                    importantForAccessibilityAttached = GetImportantForAccessibilityAttached(parentUIElement);
                }
            }

            if (importantForAccessibilityAttached == ImportantForAccessibility.Yes ||
                importantForAccessibilityAttached == ImportantForAccessibility.NoHideDescendants)
            {
                return true;
            }
            else
            {
                return IsHiddenElement(parentAutomationPeer);
            }
        }

        /// <summary>
        /// Recursively sets the value of AccessibilityView according ImportantForAccessibility setting
        /// for children of uiElementAutomationPeer
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
                UIElement childUIElement = null;
                if (automationPeer is FrameworkElementAutomationPeer childFEAutomationPeer)
                {
                    if (childFEAutomationPeer?.Owner != null)
                    {
                        childUIElement = childFEAutomationPeer.Owner;
                    }
                }
                else if (automationPeer is ItemAutomationPeer childIAutomationPeer)
                {
                    if (childIAutomationPeer?.Item is UIElement)
                    {
                        childUIElement = childIAutomationPeer?.Item as UIElement;
                    }
                }

                if (childUIElement == null)
                {
                    SetChildrenAccessibilityViewFromImportantForAccessibility(automationPeer);
                    continue;
                }

                var importantForAccessibilityAttached = GetImportantForAccessibilityAttached(childUIElement);
                switch (importantForAccessibilityAttached)
                {
                    case ImportantForAccessibility.Auto:
                        childUIElement.ClearValue(AutomationProperties.AccessibilityViewProperty);
                        SetChildrenAccessibilityViewFromImportantForAccessibility(automationPeer);
                        break;
                    case ImportantForAccessibility.No:
                        AutomationProperties.SetAccessibilityView(childUIElement, AccessibilityView.Raw);
                        SetChildrenAccessibilityViewFromImportantForAccessibility(automationPeer);
                        break;
                    case ImportantForAccessibility.NoHideDescendants:
                        AutomationProperties.SetAccessibilityView(childUIElement, AccessibilityView.Raw);
                        SetChildrenAccessibilityView(automationPeer, AccessibilityView.Raw);
                        break;
                    case ImportantForAccessibility.Yes:
                        AutomationProperties.SetAccessibilityView(childUIElement, AccessibilityView.Content);
                        SetChildrenAccessibilityView(automationPeer, AccessibilityView.Raw);
                        break;
                    default:
                        break;
                }
            }
        }

        #endregion ImportantForAccessibility
    }
}
