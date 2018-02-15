using System;
#if WINDOWS_UWP
using Windows.UI.Xaml.Automation.Peers;
using Windows.UI.Xaml.Controls;
#else
using System.Windows.Automation.Peers;
using System.Windows.Controls;
#endif

namespace ReactNative.UIManager
{
    /// <summary>
    /// Represents a Canvas with an optional Border inside.
    /// </summary>
    public class BorderedCanvas : Canvas, IAccessible
    {
        private Border _border = null;

        /// <summary>
        /// The Border associated with this Canvas or null if it doesn't have a border.
        /// The Border is always the first element in the Children collection.
        /// </summary>
        public Border Border
        {
            get
            {
                return _border;
            }
            set
            {
                if (_border != null)
                {
                    throw new InvalidOperationException("The Canvas already has a Border");
                }

                _border = value;

                Children.Insert(0, _border);
            }
        }

        /// <inheritdoc />
        protected override AutomationPeer OnCreateAutomationPeer()
        {
            return new DynamicAutomationPeer<BorderedCanvas>(this);
        }

        // TODO: implement runtime change raising event to screen reader #1562
        /// <inheritdoc />
        public AccessibilityTrait[] AccessibilityTraits { get; set; }
    }
}
