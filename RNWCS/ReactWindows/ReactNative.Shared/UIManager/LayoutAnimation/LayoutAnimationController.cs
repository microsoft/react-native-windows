// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using System;
using System.Reactive;
using System.Reactive.Disposables;
using System.Reactive.Linq;
using System.Runtime.CompilerServices;
#if WINDOWS_UWP
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
#else
using System.Windows;
using System.Windows.Controls;
#endif

namespace ReactNative.UIManager.LayoutAnimation
{
    /// <summary>
    /// Class responsible for animation layout changes, if a valid animation
    /// configuration has been supplied. If animation is not available, the
    /// layout change is applied immediately instead of animating.
    /// </summary>
    /// <remarks>
    /// TODO: Invoke success callback at the end of the animation.
    /// </remarks>
    public class LayoutAnimationController
    {
        private readonly ConditionalWeakTable<object, SerialDisposable> _activeAnimations =
            new ConditionalWeakTable<object, SerialDisposable>();

        private readonly LayoutAnimation _layoutCreateAnimation = new LayoutCreateAnimation();
        private readonly LayoutAnimation _layoutUpdateAnimation = new LayoutUpdateAnimation();
        private readonly LayoutAnimation _layoutDeleteAnimation = new LayoutDeleteAnimation();

        private bool _shouldAnimateLayout;

        /// <summary>
        /// Initializes the layout animation.
        /// </summary>
        /// <param name="config">The configuration.</param>
        public void InitializeFromConfig(JObject config)
        {
#if !LAYOUT_ANIMATION_DISABLED
            if (config == null)
            {
                Reset();
                return;
            }

            _shouldAnimateLayout = false;
            var globalDuration = config.Value<int>("duration");
            var createData = (JObject)config.GetValue("create", StringComparison.Ordinal);
            if (createData != null)
            {
                _layoutCreateAnimation.InitializeFromConfig(createData, globalDuration);
                _shouldAnimateLayout = true;
            }

            var updateData = (JObject)config.GetValue("update", StringComparison.Ordinal);
            if (updateData != null)
            {
                _layoutUpdateAnimation.InitializeFromConfig(updateData, globalDuration);
                _shouldAnimateLayout = true;
            }

            var deleteData = (JObject)config.GetValue("delete", StringComparison.Ordinal);
            if (deleteData != null)
            {
                _layoutDeleteAnimation.InitializeFromConfig(deleteData, globalDuration);
                _shouldAnimateLayout = true;
            }
#else
            return;
#endif
        }

        /// <summary>
        /// Determines if <see cref="FrameworkElement"/> should apply layout animation.
        /// </summary>
        /// <param name="view">The view to animate.</param>
        /// <returns>
        /// <code>true</code> if the layout operation should be animated, 
        /// otherwise <code>false</code>.
        /// </returns>
        public bool ShouldAnimateLayout(object view)
        {
            if (_shouldAnimateLayout)
            {
                var dependencyObject = ViewConversion.GetDependencyObject(view);
                return dependencyObject is FrameworkElement frameworkElement && frameworkElement.Parent != null;
            }
            
            return false;
        }

        /// <summary>
        /// Applies a layout animation for the given view on the transition to
        /// the given coordinates and dimensions.
        /// </summary>
        /// <param name="viewManager">The view manager for the native view.</param>
        /// <param name="view">The native view to animate.</param>
        /// <param name="dimensions">The new view dimensions to animate to.</param>
        public void ApplyLayoutUpdate(IViewManager viewManager, object view, Dimensions dimensions)
        {
            DispatcherHelpers.AssertOnDispatcher();

            var currentDimensions = viewManager.GetDimensions(view);
            var layoutAnimation = IsZeroOrNaN(currentDimensions.Width) || IsZeroOrNaN(currentDimensions.Height)
                ? _layoutCreateAnimation
                : _layoutUpdateAnimation;

            var animation = layoutAnimation.CreateAnimation(viewManager, view, dimensions);
            if (animation == null)
            {
                viewManager.SetDimensions(view, dimensions);
            }
            else
            {
                StartAnimation(view, animation);
            }
        }

        /// <summary>
        /// Animate a view deletion using the layout animation configuration
        /// supplied during initialization.
        /// </summary>
        /// <param name="viewManager">The view manager for the native view.</param>
        /// <param name="view">The view to animate.</param>
        /// <param name="finally">
        /// Called once the animation is finished, should be used to completely
        /// remove the view.
        /// </param>
        public void DeleteView(IViewManager viewManager, object view, Action @finally)
        {
            DispatcherHelpers.AssertOnDispatcher();

            var layoutAnimation = _layoutDeleteAnimation;

            var animation = layoutAnimation.CreateAnimation(viewManager, view, viewManager.GetDimensions(view));

            if (animation != null)
            {
                var element = ViewConversion.GetDependencyObject<UIElement>(view);
                element.IsHitTestVisible = false;
                StartAnimation(view, animation.Finally(@finally));
            }
            else
            {
                @finally();
            }
        }

        /// <summary>
        /// Reset the animation manager.
        /// </summary>
        public void Reset()
        {
            _layoutCreateAnimation.Reset();
            _layoutUpdateAnimation.Reset();
            _shouldAnimateLayout = false;
        }

        private void StartAnimation(object view, IObservable<Unit> animation)
        {
            // Get the serial disposable for the view
            var serialDisposable = _activeAnimations.GetOrCreateValue(view);

            // Dispose any existing animations
            serialDisposable.Disposable = Disposable.Empty;

            // Start the next animation
            serialDisposable.Disposable = animation.Subscribe();
        }

        private static bool IsZeroOrNaN(double value)
        {
            return value == 0 || double.IsNaN(value);
        }
    }
}
