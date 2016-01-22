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

        /// <summary>
        /// Sets an elevation 3D transformation effect on the <see cref="ReactCanvas"/>.
        /// </summary>
        /// <param name="elevation">The positive negative elevation Z Index value of the view.</param>
        public void SetElevationEffect(float elevation)
        {
            var transform = EnsureTransform();
            transform.TranslateZ = elevation;
            Transform3D = transform;
        }

        private CompositeTransform3D EnsureTransform()
        {
            var transform = Transform3D;
            var compositeTransform = transform as CompositeTransform3D;
            if (transform != null && compositeTransform == null)
            {
                throw new InvalidOperationException("Transform property is already set.");
            }

            if (transform == null)
            {
                compositeTransform = new CompositeTransform3D();
                Transform3D = compositeTransform;
            }

            return compositeTransform;
        }
    }
}
