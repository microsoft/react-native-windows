using Windows.UI.Xaml;

namespace ReactNative.Views.View
{
    /// <summary>
    /// Interface to vefify whether view handles pointer event or not.
    /// </summary>
    public interface IReactPointerEventsView
    {
        /// <summary>
        /// Get whether view handles pointer event or not.  
        /// </summary>
        /// <returns>
        /// The view if event is handled.
        /// Null if event is not handled.
        /// </returns>
        FrameworkElement GetPointerEventsView();
    }
}
