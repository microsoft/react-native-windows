#if WINDOWS_UWP
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation;
using Windows.UI.Xaml.Controls;
#else
using System.Windows;
using System.Windows.Controls;
#endif

namespace ReactNative.Views.ControlView
{
    /// <summary>
    /// A native control with a single Canvas child.
    /// </summary>
    public class ReactControl : UserControl
    {
        private readonly Canvas _canvas;

        /// <summary>
        /// Instantiates the <see cref="ReactControl"/>. 
        /// </summary>
        public ReactControl()
        {
            Content = _canvas = new Canvas
            {
                HorizontalAlignment = HorizontalAlignment.Stretch,
                VerticalAlignment = VerticalAlignment.Stretch,
            };
#if WINDOWS_UWP
            UseSystemFocusVisuals = true;

            // Setting AutomationProperties.Name to some string and then clearing it will guarantee that
            // AutomationPeer is always created for the canvas. The default implementation does not
            // create AutomationPeer for canvas if AutomationProperties.Name has never been set,
            // but to implement accessibility it is required that the AutomationPeer is always created.
            AutomationProperties.SetName(_canvas, " ");
            _canvas.ClearValue(AutomationProperties.NameProperty);
#endif
        }

        /// <summary>
        /// The view children.
        /// </summary>
        public UIElementCollection Children
        {
            get
            {
                return _canvas.Children;
            }
        }

        /// <summary>
        /// Keys that should be handled during <see cref="UIElement.KeyDownEvent"/>. 
        /// </summary>
        public int[] HandledKeyDownKeys
        {
            get;
            set;
        }

        /// <summary>
        /// Keys that should be handled during <see cref="UIElement.KeyUpEvent"/>. 
        /// </summary>
        public int[] HandledKeyUpKeys
        {
            get;
            set;
        }
    }
}
