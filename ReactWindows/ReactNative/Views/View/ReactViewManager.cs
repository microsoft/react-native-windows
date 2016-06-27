using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using Windows.UI.Xaml.Controls;

namespace ReactNative.Views.View
{
    /// <summary>
    /// View manager for React view instances.
    /// </summary>
    public class ReactViewManager : BorderedCanvasManager<ReactCanvas>
    {
        /// <summary>
        /// The name of this view manager. This will be the name used to 
        /// reference this view manager from JavaScript.
        /// </summary>
        public override string Name
        {
            get
            {
                return "RCTView";
            }
        }

        /// <summary>
        /// Set the pointer events handling mode for the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="pointerEvents">The pointerEvents mode.</param>
        [ReactProp(ViewProps.PointerEvents)]
        public void SetPointerEvents(Border view, string pointerEvents)
        {
            var inner = GetInnerElement(view);

            if (pointerEvents.Equals("none"))
            {
                inner.PointerEvents = PointerEvents.None;
            }
            else if (pointerEvents.Equals("box-none"))
            {
                inner.PointerEvents = PointerEvents.BoxNone;
            }
            else if (pointerEvents.Equals("box-only"))
            {
                inner.PointerEvents = PointerEvents.BoxOnly;
            }
            else
            {
                inner.PointerEvents = PointerEvents.Auto;
            }
        }

        /// <summary>
        /// Creates a new view instance of type <see cref="ReactCanvas"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <returns>The view instance.</returns>
        protected override ReactCanvas CreateInnerElement(ThemedReactContext reactContext)
        {
            return new ReactCanvas();
        }
    }
}
