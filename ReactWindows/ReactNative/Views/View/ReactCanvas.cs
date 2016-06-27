using ReactNative.UIManager;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml;

namespace ReactNative.Views.View
{
    /// <summary>
    /// Canvas with pointer event handling mode.
    /// </summary>
    public class ReactCanvas : Canvas, IReactPointerEventsView
    {
        /// <summary>
        /// The pointer events handling mode.
        /// </summary>
        public PointerEvents PointerEvents
        {
            get;
            set;
        }

        /// <summary>
        /// Get whether view handles pointer event or not.  
        /// </summary>
        /// <returns>
        /// The view if event is handled.
        /// Null if event is not handled.
        /// </returns>
        public FrameworkElement GetPointerEventsView()
        {
            var border = Parent as Border;
            var parentCanvas = border?.Parent as ReactCanvas;            
            var sendEvent = true;

            while (parentCanvas != null && sendEvent)
            {
                switch (parentCanvas.PointerEvents)
                {
                    case PointerEvents.None:
                    case PointerEvents.BoxOnly:
                        sendEvent = false;
                        break;
                }

                var parentBorder = parentCanvas.Parent as Border;
                parentCanvas = parentBorder?.Parent as ReactCanvas;
            }

            if (sendEvent && (PointerEvents == PointerEvents.Auto
                           || PointerEvents == PointerEvents.BoxOnly))
            {
                return border;
            }

            return null;
        }
    }
}
