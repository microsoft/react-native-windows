using System;
using System.Reactive;
using System.Reactive.Linq;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace ReactNative.UIManager.LayoutAnimation
{
    /// <summary>
    /// Layout animation manager for newly created views.
    /// </summary>
    class LayoutCreateAnimation : BaseLayoutAnimation
    {
        /// <summary>
        /// Create an observable animation to be used to animate the view, 
        /// based on the animation configuration supplied at initialization
        /// time and the new view position and size.
        /// </summary>
        /// <param name="view">The view to create the animation for.</param>
        /// <param name="dimensions">The view dimensions</param>
        /// <returns>
        /// An observable sequence that starts an animation when subscribed to,
        /// stops the animation when disposed, and that completes 
        /// simultaneously with the underlying animation.
        /// </returns>
        protected override IObservable<Unit> CreateAnimationCore(FrameworkElement view, Dimensions dimensions)
        {
            Canvas.SetLeft(view, dimensions.X);
            Canvas.SetTop(view, dimensions.Y);
            view.Width = dimensions.Width;
            view.Height = dimensions.Height;

            return base.CreateAnimationCore(view, dimensions);
        }

        /// <summary>
        /// Signals if the animation should be performed in reverse.
        /// </summary>
        protected override bool IsReverse
        {
            get
            {
                return false;
            }
        }
    }
}
