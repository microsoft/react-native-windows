// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using System;
using System.Reactive;
#if WINDOWS_UWP
using Windows.UI.Xaml;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Animation;
#else
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Animation;
#endif

namespace ReactNative.UIManager.LayoutAnimation
{
    /// <summary>
    /// Class responsible for default layout animation.
    /// </summary>
    abstract class BaseLayoutAnimation : LayoutAnimation
    {
        /// <summary>
        /// Signals if the animation should be performed in reverse.
        /// </summary>
        protected abstract bool IsReverse { get; }

        /// <summary>
        /// Signals if the animation configuration is valid.
        /// </summary>
        protected override bool IsValid
        {
            get
            {
                return Duration > TimeSpan.Zero && AnimatedProperty != null;
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
        protected override IObservable<Unit> CreateAnimationCore(IViewManager viewManager, object view, Dimensions dimensions)
        {
            var element = ViewConversion.GetDependencyObject<UIElement>(view);
            var animatedProperty = AnimatedProperty;
            if (animatedProperty.HasValue)
            {
                var storyboard = new Storyboard();
                var @finally = default(Action);
                switch (animatedProperty.Value)
                {
                    case AnimatedPropertyType.Opacity:
                        var opacity = element.Opacity;
                        var fromOpacity = IsReverse ? opacity : 0.0;
                        var toOpacity = IsReverse ? 0.0 : opacity;
                        element.Opacity = fromOpacity;
                        storyboard.Children.Add(CreateDoubleAnimation(element, "Opacity", fromOpacity, toOpacity));
                        @finally = () => element.Opacity = toOpacity;
                        break;
                    case AnimatedPropertyType.ScaleXY:
                        var fromScale = IsReverse ? 1.0 : 0.0;
                        var toScale = IsReverse ? 0.0 : 1.0;
                        var scaleTransform = new ScaleTransform
                        {
                            CenterX = dimensions.X + dimensions.Width / 2,
                            CenterY = dimensions.Y + dimensions.Height / 2,
                            ScaleX = fromScale,
                            ScaleY = fromScale,
                        };

                        var originalTransform = element.RenderTransform;
                        var transformGroup = new TransformGroup();
                        transformGroup.Children.Add(originalTransform);
                        transformGroup.Children.Add(scaleTransform);
                        element.RenderTransform = transformGroup;
                        storyboard.Children.Add(CreateDoubleAnimation(scaleTransform, "ScaleX", fromScale, toScale));
                        storyboard.Children.Add(CreateDoubleAnimation(scaleTransform, "ScaleY", fromScale, toScale));
                        @finally = () => element.RenderTransform = originalTransform;
                        break;
                    default:
                        throw new InvalidOperationException(
                            "Missing animation for property: " + animatedProperty.Value);
                }

                return new StoryboardObservable(storyboard, @finally);
            }

            throw new InvalidOperationException(
                "Missing animated property from the animation configuration.");
        }

        private Timeline CreateDoubleAnimation(DependencyObject target, string path, double from, double to)
        {
            var timeline = new DoubleAnimation
            {
                From = from,
                To = to,
                EasingFunction = Interpolator,
                Duration = Duration,
                BeginTime = Delay,
            };

            Storyboard.SetTarget(timeline, target);
#if WINDOWS_UWP
            Storyboard.SetTargetProperty(timeline, path);
#else
            Storyboard.SetTargetProperty(timeline, new PropertyPath(path));
#endif

            return timeline;
        }
    }
}
