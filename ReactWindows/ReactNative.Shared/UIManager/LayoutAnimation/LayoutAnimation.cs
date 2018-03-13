// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Json;
using ReactNative.Reflection;
using System;
using System.Reactive;
#if WINDOWS_UWP
using Windows.UI.Xaml;
using Windows.UI.Xaml.Media.Animation;
#else
using System.Windows;
using System.Windows.Media.Animation;
#endif

namespace ReactNative.UIManager.LayoutAnimation
{
    /// <summary>
    /// Class responsible for parsing and converting layout animation data into
    /// a native <see cref="Storyboard"/> in order to animate layout when a
    /// value configuration has been supplied by the application.
    /// </summary>
    abstract class LayoutAnimation
    {
        /// <summary>
        /// Signals if the animation configuration is valid.
        /// </summary>
        protected abstract bool IsValid { get; }

        /// <summary>
        /// The easing function for the animation.
        /// </summary>
        protected EasingFunctionBase Interpolator
        {
            get;
            private set;
        }

        /// <summary>
        /// The animated property type.
        /// </summary>
        protected AnimatedPropertyType? AnimatedProperty
        {
            get;
            private set;
        }

        /// <summary>
        /// The layout animation duration.
        /// </summary>
        protected TimeSpan Duration
        {
            get;
            private set;
        }

        /// <summary>
        /// The layout animation delay.
        /// </summary>
        protected TimeSpan? Delay
        {
            get;
            private set;
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
        public IObservable<Unit> CreateAnimation(IViewManager viewManager, object view, Dimensions dimensions)
        {
            if (!IsValid)
            {
                return null;
            }

            return CreateAnimationCore(viewManager, view, dimensions);
        }

        /// <summary>
        /// Initializes the layout animation from configuration.
        /// </summary>
        /// <param name="data">The configuration.</param>
        /// <param name="globalDuration">The duration.</param>
        public void InitializeFromConfig(JObject data, int globalDuration)
        {
            AnimatedProperty = EnumHelpers.ParseNullable<AnimatedPropertyType>(
                data.Value<string>("property"));

            Duration = data.ContainsKey("duration")
                ? TimeSpan.FromMilliseconds(data.Value<int>("duration"))
                : TimeSpan.FromMilliseconds(globalDuration);

            Delay = !data.ContainsKey("delay")
                ? default(TimeSpan?)
                : TimeSpan.FromMilliseconds(data.Value<int>("delay"));

            Interpolator = EnumHelpers
                .ParseNullable<InterpolationType>(data.Value<string>("type"))?
                .GetEasingFunction(data);

            if (!IsValid)
            {
                throw new InvalidOperationException("Invalid layout animation: " + data);
            }
        }

        /// <summary>
        /// Resets the layout animation.
        /// </summary>
        public void Reset()
        {
            AnimatedProperty = default(AnimatedPropertyType?);
            Duration = default(TimeSpan);
            Delay = default(TimeSpan?);
            Interpolator = default(EasingFunctionBase);
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
        protected abstract IObservable<Unit> CreateAnimationCore(IViewManager viewManager, object view, Dimensions dimensions);
    }
}
