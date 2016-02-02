using ReactNative.Touch;
using ReactNative.UIManager;
using Windows.Foundation;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;

namespace ReactNative.Views.View
{
    /// <summary>
    /// Backing for a react view.
    /// </summary>
    /// <remarks>
    /// TODO: Implement clipping.
    /// </remarks>
    public class ReactCanvas : Canvas, IReactInterceptingViewParent, IReactPointerEventsView
    {
        private IOnInterceptTouchEventListener _onInterceptTouchEventListener;

        /// <summary>
        /// The pointer events supported by the view.
        /// </summary>
        public PointerEvents PointerEvents { get; internal set; } = PointerEvents.Auto;

        /// <summary>
        /// Sets the touch event listener for the react view.
        /// </summary>
        /// <param name="listener">The custom touch event listener.</param>
        public void SetOnInterceptTouchEventListener(IOnInterceptTouchEventListener listener)
        {
            _onInterceptTouchEventListener = listener;
            PointerPressed += OnInterceptTouchEvent;
        }

        /// <summary>
        /// Provides the behavior for the measure pass of the layout cycle.
        /// </summary>
        /// <param name="availableSize">The available size.</param>
        /// <returns>The desired size.</returns>
        /// <remarks>
        /// Simple override that asserts that the desired size is explicit.
        /// </remarks>
        protected override Size MeasureOverride(Size availableSize)
        {
            var resultSize = base.MeasureOverride(availableSize);
            MeasureAssertions.AssertExplicitMeasurement(resultSize.Width, resultSize.Height);
            return resultSize;
        }

        private void OnInterceptTouchEvent(object sender, PointerRoutedEventArgs ev)
        {
            if (PointerEvents == PointerEvents.None || PointerEvents == PointerEvents.BoxNone)
            {
                return;
            }

            _onInterceptTouchEventListener.OnInterceptTouchEvent(sender, ev);
        }
    }
}
