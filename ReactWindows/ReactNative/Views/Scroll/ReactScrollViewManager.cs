// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Reflection;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using ReactNative.UIManager.Events;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Reflection;
using Windows.System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using static System.FormattableString;

namespace ReactNative.Views.Scroll
{
    /// <summary>
    /// The view manager for scrolling views.
    /// </summary>
    public class ReactScrollViewManager : ViewParentManager<ScrollViewer>
    {
        private const int CommandScrollTo = 1;

        private readonly ConcurrentDictionary<ScrollViewer, ScrollViewerData> _scrollViewerData =
            new ConcurrentDictionary<ScrollViewer, ScrollViewerData>();

        /// <summary>
        /// The name of the view manager.
        /// </summary>
        public override string Name
        {
            get
            {
                return "RCTScrollView";
            }
        }

        /// <summary>
        /// The commands map for the view manager.
        /// </summary>
        public override IReadOnlyDictionary<string, object> CommandsMap
        {
            get
            {
                return new Dictionary<string, object>
                {
                    { "scrollTo", CommandScrollTo },
                };
            }
        }

        /// <summary>
        /// The exported custom direct event types.
        /// </summary>
        public override IReadOnlyDictionary<string, object> ExportedCustomDirectEventTypeConstants
        {
            get
            {
                return new Dictionary<string, object>
                {
                    {
                        ScrollEventType.BeginDrag.GetJavaScriptEventName(),
                        new Dictionary<string, object>
                        {
                            { "registrationName", "onScrollBeginDrag" },
                        }
                    },
                    {
                        ScrollEventType.EndDrag.GetJavaScriptEventName(),
                        new Dictionary<string, object>
                        {
                            { "registrationName", "onScrollEndDrag" },
                        }
                    },
                    {
                        ScrollEventType.Scroll.GetJavaScriptEventName(),
                        new Dictionary<string, object>
                        {
                            { "registrationName", "onScroll" },
                        }
                    }
                };
            }
        }

        /// <summary>
        /// Sets the background color of the view.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="color">The masked color value.</param>
        [ReactProp(
            ViewProps.BackgroundColor,
            CustomType = "Color", 
            DefaultUInt32 = ColorHelpers.Transparent)]
        public void SetBackgroundColor(ScrollViewer view, uint color)
        {
            view.Background = new SolidColorBrush(ColorHelpers.Parse(color));
        }

        /// <summary>
        /// Sets whether scroll is enabled on the view.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="enabled">The enabled value.</param>
        [ReactProp("scrollEnabled", DefaultBoolean = true)]
        public void SetEnabled(ScrollViewer view, bool enabled)
        {
            if (enabled)
            {
                view.VerticalScrollMode = ScrollMode.Auto;
                view.HorizontalScrollMode = _scrollViewerData[view].HorizontalScrollMode;
            }
            else
            {
                view.VerticalScrollMode = ScrollMode.Disabled;
                view.HorizontalScrollMode = ScrollMode.Disabled;
            }
        }

        /// <summary>
        /// Sets whether horizontal scroll is enabled on the view.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="horizontal">
        /// The flag signaling whether horizontal scrolling is enabled.
        /// </param>
        [ReactProp("horizontal")]
        public void SetHorizontal(ScrollViewer view, bool horizontal)
        {
            var horizontalScrollMode = horizontal
                ? ScrollMode.Auto
                : ScrollMode.Disabled;

            view.HorizontalScrollMode = _scrollViewerData[view].HorizontalScrollMode = horizontalScrollMode;
        }

        /// <summary>
        /// Sets whether the horizontal scroll indicator is shown.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="showIndicator">
        /// The value to show the indicator or not.
        /// </param>
        [ReactProp("showsHorizontalScrollIndicator")]
        public void SetShowsHorizontalScrollIndicator(ScrollViewer view, bool showIndicator)
        {
            view.HorizontalScrollBarVisibility = showIndicator
                ? ScrollBarVisibility.Auto
                : ScrollBarVisibility.Hidden;
        }

        /// <summary>
        /// Sets whether the vertical scroll indicator is shown.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="showIndicator">
        /// The value to show the indicator or not.
        /// </param>
        [ReactProp("showsVerticalScrollIndicator")]
        public void SetShowsVerticalScrollIndicator(ScrollViewer view, bool showIndicator)
        {
            view.VerticalScrollBarVisibility = showIndicator
                ? ScrollBarVisibility.Auto
                : ScrollBarVisibility.Hidden;
        }

        /// <summary>
        /// Sets the content offset of the scroll view.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="contentOffset">The content offset.</param>
        [ReactProp("contentOffset")]
        public void SetContentOffset(ScrollViewer view, JObject contentOffset)
        {
            view.ViewChanging -= OnViewChanging;
            view.ChangeView(
                contentOffset.Value<double>("x"), 
                contentOffset.Value<double>("y"), 
                null,
                true);
            view.ViewChanging += OnViewChanging;
        }

        /// <summary>
        /// Sets the minimum zoom scale for the view.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="zoomScale">The zoom scale.</param>
        [ReactProp("minimumZoomScale")]
        public void SetMinimumZoomScale(ScrollViewer view, float? zoomScale)
        {
            view.MinZoomFactor = zoomScale ?? 0.1f;
        }

        /// <summary>
        /// Sets the maximum zoom scale for the view.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="zoomScale">The zoom scale.</param>
        [ReactProp("maximumZoomScale")]
        public void SetMaximumZoomScale(ScrollViewer view, float? zoomScale)
        {
            view.MaxZoomFactor = zoomScale ?? 10.0f;
        }

        /// <summary>
        /// Sets the zoom scale for the view.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="zoomScale">The zoom scale.</param>
        [ReactProp("zoomScale")]
        public void SetZoomScale(ScrollViewer view, float? zoomScale)
        {
            view.ChangeView(null, null, zoomScale ?? 1.0f);
        }
        
        /// <summary>
        /// Enables or disables scroll view zoom.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="enabled">Signals whether zoom is enabled.</param>
        [ReactProp("zoomEnabled")]
        public void SetZoomScale(ScrollViewer view, bool? enabled)
        {
            view.ZoomMode = (enabled ?? false) 
                ? ZoomMode.Enabled 
                : ZoomMode.Disabled;
        }

        /// <summary>
        /// Sets the tab navigation for the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="tabNavigation">The tab navigation.</param>
        [ReactProp("tabNavigation")]
        public void SetTabNavigation(ScrollViewer view, string tabNavigation)
        {
            view.TabNavigation = EnumHelpers.ParseNullable<KeyboardNavigationMode>(tabNavigation) ?? KeyboardNavigationMode.Local;
        }

        /// <summary>
        /// Disables keyboard based arrow scrolling.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="disabled">Signals whether keyboard based scrolling is disabled.</param>
        [ReactProp("disableKeyboardBasedScrolling")]
        public void SetDisableKeyboardBasedScrolling(ScrollViewer view, bool? disabled)
        {
            var disabledValue = disabled ?? false;
            if (_scrollViewerData[view].DisableArrowNavigation != disabledValue)
            {
                if (_scrollViewerData[view].IsControlLoaded)
                {
                    UpdateDisableKeyboardBasedScrolling(view, disabledValue);
                }
                else
                {
                    _scrollViewerData[view].DisableArrowNavigation = disabledValue;
                }
            }
        }

        /// <summary>
        /// Adds a child at the given index.
        /// </summary>
        /// <param name="parent">The parent view.</param>
        /// <param name="child">The child view.</param>
        /// <param name="index">The index.</param>
        /// <remarks>
        /// <see cref="ReactScrollViewManager"/> only supports one child.
        /// </remarks>
        public override void AddView(ScrollViewer parent, DependencyObject child, int index)
        {
            if (index != 0)
            {
                throw new ArgumentOutOfRangeException(nameof(index), Invariant($"{nameof(ScrollViewer)} currently only supports one child."));
            }

            if (parent.Content != null)
            {
                throw new InvalidOperationException(Invariant($"{nameof(ScrollViewer)} already has a child element."));
            }

            child.SetValue(FrameworkElement.VerticalAlignmentProperty, VerticalAlignment.Top);
            child.SetValue(FrameworkElement.HorizontalAlignmentProperty, HorizontalAlignment.Left);
            parent.Content = child;
        }

        /// <summary>
        /// Gets the child at the given index.
        /// </summary>
        /// <param name="parent">The parent view.</param>
        /// <param name="index">The index.</param>
        /// <returns>The child view.</returns>
        /// <remarks>
        /// <see cref="ReactScrollViewManager"/> only supports one child.
        /// </remarks>
        public override DependencyObject GetChildAt(ScrollViewer parent, int index)
        {
            if (index != 0)
            {
                throw new ArgumentOutOfRangeException(nameof(index), "ScrollView currently only supports one child.");
            }

            return EnsureChild(parent);
        }

        /// <summary>
        /// Gets the number of children in the view parent.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        /// <returns>The number of children.</returns>
        public override int GetChildCount(ScrollViewer parent)
        {
            return parent.Content != null ? 1 : 0;
        }

        /// <summary>
        /// Removes all children from the view parent.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        public override void RemoveAllChildren(ScrollViewer parent)
        {
            parent.Content = null;
        }

        /// <summary>
        /// Removes the child at the given index.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        /// <param name="index">The index.</param>
        /// <remarks>
        /// <see cref="ReactScrollViewManager"/> only supports one child.
        /// </remarks>
        public override void RemoveChildAt(ScrollViewer parent, int index)
        {
            if (index != 0)
            {
                throw new ArgumentOutOfRangeException(nameof(index), "ScrollView currently only supports one child.");
            }

            EnsureChild(parent);
            RemoveAllChildren(parent);
        }

        /// <summary>
        /// Called when view is detached from view hierarchy and allows for 
        /// additional cleanup by the <see cref="ReactScrollViewManager"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The view.</param>
        public override void OnDropViewInstance(ThemedReactContext reactContext, ScrollViewer view)
        {
            base.OnDropViewInstance(reactContext, view);

            _scrollViewerData.TryRemove(view, out _);

            view.Loaded -= OnLoaded;
            view.ViewChanging -= OnViewChanging;
            view.DirectManipulationStarted -= OnDirectManipulationStarted;
            view.DirectManipulationCompleted -= OnDirectManipulationCompleted;
        }

        /// <summary>
        /// Receive events/commands directly from JavaScript through the 
        /// <see cref="UIManagerModule"/>.
        /// </summary>
        /// <param name="view">
        /// The view instance that should receive the command.
        /// </param>
        /// <param name="commandId">Identifer for the command.</param>
        /// <param name="args">Optional arguments for the command.</param>
        public override void ReceiveCommand(ScrollViewer view, int commandId, JArray args)
        {
            switch (commandId)
            {
                case CommandScrollTo:
                    var x = args[0].Value<double>();
                    var y = args[1].Value<double>();
                    var animated = args[2].Value<bool>();
                    ScrollTo(view, x, y, animated);
                    break;
                default:
                    throw new InvalidOperationException(
                        Invariant($"Unsupported command '{commandId}' received by '{typeof(ReactScrollViewManager)}'."));
            }
        }

        /// <summary>
        /// Creates a new view instance.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <returns>The view instance.</returns>
        protected override ScrollViewer CreateViewInstance(ThemedReactContext reactContext)
        {
            var scrollViewerData = new ScrollViewerData();

            var scrollViewer = new ScrollViewer
            {
                // Align to RN defaults
                HorizontalScrollBarVisibility = ScrollBarVisibility.Auto,
                HorizontalScrollMode = ScrollMode.Disabled,
                VerticalScrollBarVisibility = ScrollBarVisibility.Auto,
                VerticalScrollMode = ScrollMode.Auto,
                // The default tab index keeps the ScrollViewer (and its children) outside the normal flow of tabIndex==0 controls.
                // We force a better default, at least until we start supporting TabIndex/IsTabStop properties on RCTScrollView.
                TabIndex = 0,
            };

            _scrollViewerData.AddOrUpdate(scrollViewer, scrollViewerData, (k, v) => scrollViewerData);

            return scrollViewer;
        }

        /// <summary>
        /// Adds event emitters for drag and scroll events.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The view instance.</param>
        protected override void AddEventEmitters(ThemedReactContext reactContext, ScrollViewer view)
        {
            base.AddEventEmitters(reactContext, view);
            view.DirectManipulationCompleted += OnDirectManipulationCompleted;
            view.DirectManipulationStarted += OnDirectManipulationStarted;
            view.ViewChanging += OnViewChanging;
            view.Loaded += OnLoaded;
        }

        private void OnDirectManipulationCompleted(object sender, object e)
        {
            var scrollViewer = (ScrollViewer)sender;
            EmitScrollEvent(
                scrollViewer,
                ScrollEventType.EndDrag,
                scrollViewer.HorizontalOffset,
                scrollViewer.VerticalOffset,
                scrollViewer.ZoomFactor);
        }

        private void OnDirectManipulationStarted(object sender, object e)
        {
            var scrollViewer = (ScrollViewer)sender;
            EmitScrollEvent(
                scrollViewer,
                ScrollEventType.BeginDrag,
                scrollViewer.HorizontalOffset,
                scrollViewer.VerticalOffset,
                scrollViewer.ZoomFactor);
        }

        private void OnViewChanging(object sender, ScrollViewerViewChangingEventArgs args)
        {
            var nextView = args.NextView;
            var scrollViewer = (ScrollViewer)sender;
            EmitScrollEvent(
                scrollViewer,
                ScrollEventType.Scroll,
                nextView.HorizontalOffset,
                nextView.VerticalOffset,
                nextView.ZoomFactor);
        }

        private void EmitScrollEvent(
            ScrollViewer scrollViewer, 
            ScrollEventType eventType,
            double x, 
            double y,
            double zoomFactor)
        {
            var reactTag = scrollViewer.GetTag();

            // Scroll position
            var contentOffset = new JObject
            {
                { "x", x },
                { "y", y },
            };

            // Distance the content view is inset from the enclosing scroll view
            // TODO: Should these always be 0 for the XAML ScrollViewer?
            var contentInset = new JObject
            {
                { "top", 0 },
                { "bottom", 0 },
                { "left", 0 },
                { "right", 0 },
            };

            // Size of the content view
            var contentSize = new JObject
            {
                { "width", scrollViewer.ExtentWidth },
                { "height", scrollViewer.ExtentHeight },
            };

            // Size of the viewport
            var layoutMeasurement = new JObject
            {
                { "width", scrollViewer.ActualWidth },
                { "height", scrollViewer.ActualHeight },
            };

            scrollViewer.GetReactContext()
                .GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(
                    new ScrollEvent(
                        reactTag,
                        eventType,
                        new JObject
                        {
                            { "target", reactTag },
                            { "responderIgnoreScroll", true },
                            { "contentOffset", contentOffset },
                            { "contentInset", contentInset },
                            { "contentSize", contentSize },
                            { "layoutMeasurement", layoutMeasurement },
                            { "zoomScale", zoomFactor },
                        }));
        }

        private void OnLoaded(object sender, RoutedEventArgs e)
        {
            var scrollViewer = (ScrollViewer)sender;
            _scrollViewerData[scrollViewer].IsControlLoaded = true;

            UpdateDisableKeyboardBasedScrolling(scrollViewer, _scrollViewerData[scrollViewer].DisableArrowNavigation);
        }

        private void UpdateDisableKeyboardBasedScrolling(ScrollViewer view, bool disable)
        {
            ScrollContentPresenter contentPresenter = _scrollViewerData[view].ContentPresenter;
            if (disable)
            {
                if (contentPresenter == null)
                {
                    contentPresenter = FindChild<ScrollContentPresenter>(view);
                    if (contentPresenter == null)
                    {
                        throw new InvalidOperationException("ScrollViewer doesn't seem to contain a ScrollContentPresenter");
                    }
                    _scrollViewerData[view].ContentPresenter = contentPresenter;
                }

                contentPresenter.KeyDown += OnPresenterKeyDown;
            }
            else
            {
                if (contentPresenter != null)
                {
                    contentPresenter.KeyDown -= OnPresenterKeyDown;
                }
            }

            _scrollViewerData[view].DisableArrowNavigation = disable;
        }

        private static T FindChild<T>(DependencyObject startNode)
          where T : DependencyObject
        {
            int count = VisualTreeHelper.GetChildrenCount(startNode);
            for (int i = 0; i < count; i++)
            {
                DependencyObject current = VisualTreeHelper.GetChild(startNode, i);
                if ((current.GetType()).Equals(typeof(T)) || (current.GetType().GetTypeInfo().IsSubclassOf(typeof(T))))
                {
                    T asType = (T)current;
                    return asType;
                }
                var result = FindChild<T>(current);
                if (result != null)
                {
                    return result;
                }
            }
            return null;
        }

        private void OnPresenterKeyDown(object sender, KeyRoutedEventArgs e)
        {
            if (e.Key == VirtualKey.Up || e.Key == VirtualKey.Down || e.Key == VirtualKey.Left || e.Key == VirtualKey.Right)
            {
                e.Handled = true;
            }
        }

        private static DependencyObject EnsureChild(ScrollViewer view)
        {
            var child = view.Content;
            if (child == null)
            {
                throw new InvalidOperationException(Invariant($"{nameof(ScrollViewer)} does not have any children."));
            }

            var dependencyObject = child as DependencyObject;
            if (dependencyObject == null)
            {
                throw new InvalidOperationException(Invariant($"Invalid child element in {nameof(ScrollViewer)}."));
            }

            return dependencyObject;
        }

        private static void ScrollTo(ScrollViewer scrollView, double x, double y, bool animated)
        {
            scrollView.ChangeView(x, y, null, !animated);
        }

        class ScrollEvent : Event
        {
            private readonly ScrollEventType _type;
            private readonly JObject _data;

            public ScrollEvent(int viewTag, ScrollEventType type, JObject data)
                : base(viewTag)
            {
                _type = type;
                _data = data;
            }

            public override string EventName
            {
                get
                {
                    return _type.GetJavaScriptEventName();
                }
            }

            public override void Dispatch(RCTEventEmitter eventEmitter)
            {
                eventEmitter.receiveEvent(ViewTag, EventName, _data);
            }
        }

        class ScrollViewerData
        {
            public ScrollMode HorizontalScrollMode = ScrollMode.Disabled;

            public bool IsControlLoaded;
            public bool DisableArrowNavigation;
            public ScrollContentPresenter ContentPresenter;
        }
    }
}
