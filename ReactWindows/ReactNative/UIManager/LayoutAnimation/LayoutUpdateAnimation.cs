using System;
using System.Reactive;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Animation;

namespace ReactNative.UIManager.LayoutAnimation
{
    /// <summary>
    /// Defines which <see cref="Storyboard"/> should be used for animating
    /// layout updates for <see cref="FrameworkElement"/>.
    /// </summary>
    class LayoutUpdateAnimation : LayoutAnimation
    {
        /// <summary>
        /// Signals if the animation configuration is valid.
        /// </summary>
        protected override bool IsValid
        {
            get
            {
                return Duration > TimeSpan.Zero;
            }
        }

        /// <summary>
        /// Create a <see cref="Storyboard"/> to be used to animate the view, 
        /// based on the animation configuration supplied at initialization
        /// time and the new view position and size.
        /// </summary>
        /// <param name="view">The view to create the animation for.</param>
        /// <param name="dimensions">The view dimensions.</param>
        /// <returns>The storyboard.</returns>
        protected override IObservable<Unit> CreateAnimationCore(FrameworkElement view, Dimensions dimensions)
        {
            var currentX = Canvas.GetLeft(view);
            var currentY = Canvas.GetTop(view);
            var currentWidth = view.Width;
            var currentHeight = view.Height;

            var animateLocation = dimensions.X != currentX || dimensions.Y != currentY;
            var animateSize = dimensions.Width != currentWidth || dimensions.Height != currentHeight;

            if (!animateLocation && !animateSize)
            {
                return null;
            }

            var storyboard = new Storyboard();
            if (currentX != dimensions.X)
            {
                storyboard.Children.Add(
                    CreateTimeline(view, "(Canvas.Left)", currentX, dimensions.X));
            }

            if (currentY != dimensions.Y)
            {
                storyboard.Children.Add(
                    CreateTimeline(view, "(Canvas.Top)", currentY, dimensions.Y));
            }

            if (currentWidth != dimensions.Width)
            {
                var timeline = CreateTimeline(view, "Width", currentWidth, dimensions.Width);
                timeline.EnableDependentAnimation = true;
                storyboard.Children.Add(timeline);
            }

            if (currentHeight != dimensions.Height)
            {
                var timeline = CreateTimeline(view, "Height", currentHeight, dimensions.Height);
                timeline.EnableDependentAnimation = true;
                storyboard.Children.Add(timeline);
            }

            return new StoryboardObservable(storyboard, () =>
            {
                Canvas.SetLeft(view, dimensions.X);
                Canvas.SetTop(view, dimensions.Y);
                view.Width = dimensions.Width;
                view.Height = dimensions.Height;
            });
        }

        private DoubleAnimation CreateTimeline(FrameworkElement view, string path, double from, double to)
        {
            var timeline = new DoubleAnimation
            {
                From = from,
                To = to,
                EasingFunction = Interpolator,
                Duration = Duration,
                BeginTime = Delay,
            };

            Storyboard.SetTarget(timeline, view);
            Storyboard.SetTargetProperty(timeline, path);

            return timeline;
        }
    }
}
