using ReactNative.Touch;
using ReactNative.UIManager;
using System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media.Media3D;

namespace ReactNative.Views.View
{
    /// <summary>
    /// Backing for a React View. Has support for borders, but since borders 
    /// aren't common, lazy initializes most of the storage needed for them. Also supports
    /// 3D transformations such as elevation depth.  
    /// </summary>
    public class ReactCanvas : Canvas, IReactInterceptingViewParent, IReactPointerEventsView
    {
        private IOnInterceptTouchEventListener _onInterceptTouchEventListener;

        public ReactCanvas() : base()
        {
            this.SizeChanged += OnBoundsChanged;
        }

        public PointerEvents PointerEvents { get; set; } = PointerEvents.Auto;

        /// <summary>
        /// Stubbed out so any class that implements <see cref="ReactViewManager"/> can 
        /// override the OnBoundsChanged.
        /// TODO: The default behavior for RCTView still needs to be done.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected virtual void OnBoundsChanged(object sender, SizeChangedEventArgs e)
        {
        }

        /// <summary>
        /// Sets the touch event listener for the react view.
        /// </summary>
        /// <param name="listener">The custom touch event listener.</param>
        public void SetOnInterceptTouchEventListener(IOnInterceptTouchEventListener listener)
        {
            _onInterceptTouchEventListener = listener;
            this.PointerPressed += OnInterceptTouchEvent;
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
