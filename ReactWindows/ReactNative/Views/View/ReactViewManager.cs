using ReactNative.Reflection;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using Windows.UI.Xaml.Controls;

namespace ReactNative.Views.View
{
    /// <summary>
    /// View manager for React view instances.
    /// </summary>
    public class ReactViewManager : BorderedCanvasManager<Canvas>
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
        /// Sets whether or not the view is an accessibility element.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="accessible">A flag indicating whether or not the view is an accessibility element.</param>
        [ReactProp("accessible")]
        public void SetAccessible(Border view, bool accessible)
        {
            // TODO: #557 Provide implementation for View's accessible prop

            // We need to have this stub for this prop so that Views which
            // specify the accessible prop aren't considered to be layout-only.
            // The proper implementation is still to be determined.
        }

        /// <summary>
        /// Set the pointer events handling mode for the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="pointerEventsString">The pointerEvents mode.</param>
        [ReactProp(ViewProps.PointerEvents)]
        public void SetPointerEvents(Border view, string pointerEventsString)
        {
            var pointerEvents = EnumHelpers.ParseNullable<PointerEvents>(pointerEventsString) ?? PointerEvents.Auto;
            view.SetPointerEvents(pointerEvents);
        }

        /// <summary>
        /// Creates a new view instance of type <see cref="Canvas"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <returns>The view instance.</returns>
        protected override Canvas CreateInnerElement(ThemedReactContext reactContext)
        {
            return new Canvas();
        }
    }
}
