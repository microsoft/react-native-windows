using System;

#if WINDOWS_UWP
using ReactNative.Common;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation;
using Windows.UI.Xaml.Automation.Peers;
using Windows.UI.Xaml.Controls;
#else
using System.Windows;
using System.Windows.Controls;
#endif

namespace ReactNative.UIManager
{
    /// <summary>
    /// This class subclasses Canvas and overrides OnCreateAutomationPeer()
    /// so that the creation of AutomationPeer can be adjusted to fit
    /// ImportantForAccessibility setting for the canvas.
    /// </summary>
    public class AutomationCanvas : Canvas
    {
#if WINDOWS_UWP
        /// <summary>
        /// Canvas by default returns null for AutomationPeer unless AutomationProperties.Name is set.
        /// For ImportantForAccessibility functionality to work in some cases it needs for canvas
        /// to return a valid AutomationPeer even if AutomationProperties.Name is not set since
        /// for these cases canvas will affect the descendants visibility to narrator. Once AutomationPeer
        /// is created, it is cached for the lifetime of the canvas and shows in the automation tree
        /// even if the AutomationProperties.Name property is cleared.
        /// </summary>
        /// <returns></returns>
        protected override AutomationPeer OnCreateAutomationPeer()
        {
            var canvasPeer = base.OnCreateAutomationPeer();

            if (canvasPeer == null)
            {
                if (AccessibilityHelper.DoesHideDescendants(this) &&
                    string.IsNullOrEmpty(AutomationProperties.GetName(this)))
                {
                    // Set AutomationProperties.Name to a non-empty string in order to
                    // obtain an AutomationPeer to return and then clear it.
                    AutomationProperties.SetName(this, " ");
                    canvasPeer = base.OnCreateAutomationPeer();
                    this.ClearValue(AutomationProperties.NameProperty);
                }
            }

            return canvasPeer;
        }
#endif
    }
}
