// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using ReactNative.UIManager;
using System.Linq;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation.Peers;
using Windows.UI.Xaml.Automation.Provider;

namespace ReactNative.Accessibility
{
    /// <summary>
    /// Automation peer that allows to dynamically change behavior.
    /// </summary>
    /// <typeparam name="T">Type of owner of the automation peer.</typeparam>
    public sealed class DynamicAutomationPeer<T> : FrameworkElementAutomationPeer, IInvokeProvider
        where T : FrameworkElement, IAccessible
    {
        /// <summary>
        /// Gets typed owner of this peer.
        /// </summary>
        /// <returns>Owner of this peer. Null if the owner does not exist or is invalid.</returns>
        private T GetOwner()
        {
            T owner = null;
            try
            {
                owner = (T)base.Owner;
            }
            catch
            {
                // base.Owner getter will throw an exception constructed
                // from HRESULT UIA_E_ELEMENTNOTAVAILABLE = -2147220991; 0x80040201
                // if the owner UIElement is already garbage collected or otherwise invalid.
                // We swallow it not to report error to screen reader which in our
                // testing sometimes gets confused and stops reading the app at all.
            }
            return owner;
        }

        /// <inheritdoc />
        public DynamicAutomationPeer(T owner)
            : base(owner)
        {
        }

        /// <inheritdoc />
        protected override AutomationControlType GetAutomationControlTypeCore()
        {
            T owner = GetOwner();
            if (owner != null)
            {
                if (owner.AccessibilityTraits?.Contains(AccessibilityTrait.ListItem) == true)
                {
                    return AutomationControlType.ListItem;
                }
                if (owner.AccessibilityTraits?.Contains(AccessibilityTrait.Button) == true)
                {
                    return AutomationControlType.Button;
                }

                // We expose a view that hides all children but makes itself visible to screen reader
                // with an (expected) accessible name as Text control type instead of Group to avoid
                // "group" suffix screen reader appends to the name.
                // Another argument for this is that it's not ideal to tell user that something without children
                // is a "group".
                var isLabelSet = !string.IsNullOrEmpty(AccessibilityHelper.GetAccessibilityLabel(owner));
                var i4a = AccessibilityHelper.GetImportantForAccessibility(owner);
                if (i4a == ImportantForAccessibility.Yes
                    || (i4a == ImportantForAccessibility.Auto && isLabelSet))
                {
                    return AutomationControlType.Text;
                }

                return AutomationControlType.Group;
            }
            return AutomationControlType.Text;
        }

        /// <inheritdoc />
        protected override object GetPatternCore(PatternInterface patternInterface)
        {
            T owner = GetOwner();
            if (owner != null)
            {
                if (patternInterface == PatternInterface.Invoke
                    && owner.AccessibilityTraits?.Contains(AccessibilityTrait.Button) == true)
                {
                    return this;
                }
                if (owner.AccessibilityTraits?.Contains(AccessibilityTrait.ListItem) == true)
                {
                    return base.GetPatternCore(patternInterface);
                }
            }
            return null;
        }

        /// <inheritdoc />
        public void Invoke()
        {
            T owner = GetOwner();
            if (owner != null)
            {
                owner.GetReactContext()
                    .GetNativeModule<UIManagerModule>()
                    .EventDispatcher
                    .DispatchEvent(new AccessibilityTapEvent(owner.GetTag()));
            }
        }
    }
}
