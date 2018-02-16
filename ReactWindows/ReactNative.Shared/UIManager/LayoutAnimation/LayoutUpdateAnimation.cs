// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using System;
using System.Reactive;
#if WINDOWS_UWP
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Animation;
#else
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media.Animation;
#endif

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
        /// <param name="viewManager">The view manager for the view.</param>
        /// <param name="view">The view to create the animation for.</param>
        /// <param name="dimensions">The view dimensions.</param>
        /// <returns>The storyboard.</returns>
        protected override IObservable<Unit> CreateAnimationCore(IViewManager viewManager, object view, Dimensions dimensions)
        {
            var currentDimensions = viewManager.GetDimensions(view);

            var animateLocation = dimensions.X != currentDimensions.X || dimensions.Y != currentDimensions.Y;
            var animateSize = dimensions.Width != currentDimensions.Width || dimensions.Height != currentDimensions.Height;

            if (!animateLocation && !animateSize)
            {
                return null;
            }

            var element = ViewConversion.GetDependencyObject(view);
            var storyboard = new Storyboard();
            if (currentDimensions.X != dimensions.X)
            {
                storyboard.Children.Add(
                    CreateTimeline(element, "(Canvas.Left)", currentDimensions.X, dimensions.X));
            }

            if (currentDimensions.Y != dimensions.Y)
            {
                storyboard.Children.Add(
                    CreateTimeline(element, "(Canvas.Top)", currentDimensions.Y, dimensions.Y));
            }

            if (currentDimensions.Width != dimensions.Width)
            {
                var timeline = CreateTimeline(element, "Width", currentDimensions.Width, dimensions.Width);
#if WINDOWS_UWP
                timeline.EnableDependentAnimation = true;
#endif
                storyboard.Children.Add(timeline);
            }

            if (currentDimensions.Height != dimensions.Height)
            {
                var timeline = CreateTimeline(element, "Height", currentDimensions.Height, dimensions.Height);
#if WINDOWS_UWP
                timeline.EnableDependentAnimation = true;
#endif
                storyboard.Children.Add(timeline);
            }

            return new StoryboardObservable(storyboard, () =>
            {
                viewManager.SetDimensions(element, dimensions);
            });
        }

        private DoubleAnimation CreateTimeline(DependencyObject view, string path, double from, double to)
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
#if WINDOWS_UWP
            Storyboard.SetTargetProperty(timeline, path);
#else
            Storyboard.SetTargetProperty(timeline, new PropertyPath(path));
#endif

            return timeline;
        }
    }
}
