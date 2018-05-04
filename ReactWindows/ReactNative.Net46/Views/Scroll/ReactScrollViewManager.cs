// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using ReactNative.UIManager.Events;
using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using static System.FormattableString;

namespace ReactNative.Views.Scroll
{
    /// <summary>
    /// The view manager for scrolling views.
    /// </summary>
    public class ReactScrollViewManager : ViewParentManager<ScrollView>
    {
        private const int CommandScrollTo = 1;

        private readonly IDictionary<ScrollView, ScrollViewerData> _scrollViewerData =
            new Dictionary<ScrollView, ScrollViewerData>();

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
        public override JObject ViewCommandsMap
        {
            get
            {
                return new JObject
                {
                    { "scrollTo", CommandScrollTo },
                };
            }
        }

        /// <summary>
        /// The exported custom direct event types.
        /// </summary>
        public override JObject CustomDirectEventTypeConstants
        {
            get
            {
                return new JObject
                {
                    {
                        ScrollEventType.BeginDrag.GetJavaScriptEventName(),
                        new JObject
                        {
                            { "registrationName", "onScrollBeginDrag" },
                        }
                    },
                    {
                        ScrollEventType.EndDrag.GetJavaScriptEventName(),
                        new JObject
                        {
                            { "registrationName", "onScrollEndDrag" },
                        }
                    },
                    {
                        ScrollEventType.Scroll.GetJavaScriptEventName(),
                        new JObject
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
            CustomType = "Color")]
        public void SetBackgroundColor(ScrollView view, uint? color)
        {
            view.Background = color.HasValue
                ? new SolidColorBrush(ColorHelpers.Parse(color.Value))
                : null;
        }

        /// <summary>
        /// Sets whether scroll is enabled on the view.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="enabled">The enabled value.</param>
        [ReactProp("scrollEnabled", DefaultBoolean = true)]
        public void SetEnabled(ScrollView view, bool enabled)
        {
            view.CanContentScroll = enabled;
        }

        /// <summary>
        /// Sets whether horizontal scroll is enabled on the view.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="horizontal">
        /// The flag signaling whether horizontal scrolling is enabled.
        /// </param>
        [ReactProp("horizontal")]
        public void SetHorizontal(ScrollView view, bool horizontal)
        {
            var horizontalScrollMode = horizontal
                ? ScrollBarVisibility.Auto
                : ScrollBarVisibility.Disabled;

            view.HorizontalScrollBarVisibility = _scrollViewerData[view].HorizontalScrollBarVisibility = horizontalScrollMode;
        }

        /// <summary>
        /// Sets whether the horizontal scroll indicator is shown.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="showIndicator">
        /// The value to show the indicator or not.
        /// </param>
        [ReactProp("showsHorizontalScrollIndicator")]
        public void SetShowsHorizontalScrollIndicator(ScrollView view, bool showIndicator)
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
        public void SetShowsVerticalScrollIndicator(ScrollView view, bool showIndicator)
        {
            view.VerticalScrollBarVisibility = showIndicator
                ? ScrollBarVisibility.Visible
                : ScrollBarVisibility.Hidden;
        }

        /// <summary>
        /// Sets the content offset of the scroll view.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="contentOffset">The content offset.</param>
        [ReactProp("contentOffset")]
        public void SetContentOffset(ScrollView view, JObject contentOffset)
        {
            view.ScrollChanged -= OnViewChanging;
            view.ScrollToHorizontalOffset(contentOffset.Value<double>("x"));
            view.ScrollToVerticalOffset(contentOffset.Value<double>("y"));
            view.ScrollChanged += OnViewChanging;
        }

        /// <summary>
        /// Sets the minimum zoom scale for the view.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="zoomScale">The zoom scale.</param>
        [ReactProp("minimumZoomScale")]
        public void SetMinimumZoomScale(ScrollView view, float? zoomScale)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the maximum zoom scale for the view.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="zoomScale">The zoom scale.</param>
        [ReactProp("maximumZoomScale")]
        public void SetMaximumZoomScale(ScrollView view, float? zoomScale)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the zoom scale for the view.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="zoomScale">The zoom scale.</param>
        [ReactProp("zoomScale")]
        public void SetZoomScale(ScrollView view, float? zoomScale)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Enables or disables scroll view zoom.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="enabled">Signals whether zoom is enabled.</param>
        [ReactProp("zoomEnabled")]
        public void SetZoomScale(ScrollView view, bool? enabled)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Disables keyboaed based arrow scrolling.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="disabled">Signals whether keyboard based scrolling is disabled.</param>
        [ReactProp("disableKeyboardBasedScrolling")]
        public void SetDisableKeyboardBasedScrolling(ScrollView view, bool? disabled)
        {
            throw new NotImplementedException();
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
        public override void AddView(ScrollView parent, DependencyObject child, int index)
        {
            if (index != 0)
            {
                throw new ArgumentOutOfRangeException(nameof(index), Invariant($"{nameof(ScrollView)} currently only supports one child."));
            }

            if (parent.Content != null)
            {
                throw new InvalidOperationException(Invariant($"{nameof(ScrollView)} already has a child element."));
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
        public override DependencyObject GetChildAt(ScrollView parent, int index)
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
        public override int GetChildCount(ScrollView parent)
        {
            return parent.Content != null ? 1 : 0;
        }

        /// <summary>
        /// Removes all children from the view parent.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        public override void RemoveAllChildren(ScrollView parent)
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
        public override void RemoveChildAt(ScrollView parent, int index)
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
        public override void OnDropViewInstance(ThemedReactContext reactContext, ScrollView view)
        {
            base.OnDropViewInstance(reactContext, view);

            _scrollViewerData.Remove(view);

            var hashCode = view.GetHashCode();
            if (_scollViewCancelMap.ContainsKey(hashCode))
            {
                _scollViewCancelMap[hashCode].Cancel();
                _scollViewCancelMap.Remove(hashCode);
            }

            view.ScrollChanged -= OnViewChanging;
            view.ManipulationStarted -= OnDirectManipulationStarted;
            view.ManipulationCompleted -= OnDirectManipulationCompleted;
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
        public override void ReceiveCommand(ScrollView view, int commandId, JArray args)
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
        protected override ScrollView CreateViewInstance(ThemedReactContext reactContext)
        {
            var scrollViewerData = new ScrollViewerData();

            var scrollViewer = new ScrollView
            {
                HorizontalScrollBarVisibility = ScrollBarVisibility.Auto,
                VerticalScrollBarVisibility = ScrollBarVisibility.Auto,
                Focusable = false,
            };

            _scrollViewerData.Add(scrollViewer, scrollViewerData);

            return scrollViewer;
        }

        /// <summary>
        /// Adds event emitters for drag and scroll events.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The view instance.</param>
        protected override void AddEventEmitters(ThemedReactContext reactContext, ScrollView view)
        {
            base.AddEventEmitters(reactContext, view);
            view.ManipulationStarted += OnDirectManipulationStarted;
            view.ManipulationCompleted += OnDirectManipulationCompleted;
            view.ScrollChanged += OnViewChanging;
        }

        private void OnDirectManipulationCompleted(object sender, object e)
        {
            var scrollViewer = (ScrollView)sender;
            EmitScrollEvent(
                scrollViewer,
                ScrollEventType.EndDrag,
                scrollViewer.HorizontalOffset,
                scrollViewer.VerticalOffset,
                1);
        }

        private void OnDirectManipulationStarted(object sender, object e)
        {
            var scrollViewer = (ScrollView)sender;
            EmitScrollEvent(
                scrollViewer,
                ScrollEventType.BeginDrag,
                scrollViewer.HorizontalOffset,
                scrollViewer.VerticalOffset,
                1);
        }

        private void OnViewChanging(object sender, ScrollChangedEventArgs args)
        {
            var scrollViewer = (ScrollView)sender;
            EmitScrollEvent(
                scrollViewer,
                ScrollEventType.Scroll,
                args.HorizontalOffset,
                args.VerticalOffset,
                1);
        }

        private void EmitScrollEvent(
            ScrollView scrollViewer,
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

        private static DependencyObject EnsureChild(ScrollView view)
        {
            var child = view.Content;
            if (child == null)
            {
                throw new InvalidOperationException(Invariant($"{nameof(ScrollView)} does not have any children."));
            }

            var dependencyObject = child as DependencyObject;
            if (dependencyObject == null)
            {
                throw new InvalidOperationException(Invariant($"Invalid child element in {nameof(ScrollView)}."));
            }

            return dependencyObject;
        }

        private void ScrollTo(ScrollView scrollView, double x, double y, bool animated)
        {
            if (animated)
            {
                Task.Run(() => ScrollToAnimated(new WeakReference<IScrollView>(scrollView), x, y));
            }
            else
            {
                scrollView.ScrollToHorizontalOffset(x);
                scrollView.ScrollToVerticalOffset(y);
            }
        }

        /// <summary>
        /// As the <see cref="ScrollToAnimated"/> method is reentrant,
        /// _scollViewCancelMap is a Dictionary to map scrollView and <see cref="CancellationTokenSource"/>.
        /// </summary>
        private readonly Dictionary<int, CancellationTokenSource> _scollViewCancelMap = new Dictionary<int, CancellationTokenSource>();

        /// <summary>
        /// ScrollViewer does not support ScrollToSomeWhere animated, this is custom animated work.
        /// </summary>
        /// <param name="weakScrollView">WeakRefernce ScrollViewer</param>
        /// <param name="x">Scroll to x</param>
        /// <param name="y">Scroll to y</param>
        /// <returns>Async task once scrolling is done</returns>
        internal async Task ScrollToAnimated(WeakReference<IScrollView> weakScrollView, double x, double y)
        {
            // Will do animation in this period
            const int ANIMATED_TIME_INTERVAL = 300;

            // Progressively scroll time unit
            const int ANIMATED_TIME_UNIT = 100;

            // Total times to do progressive scroll
            const int ANIMATED_TIME_SLOT = ANIMATED_TIME_INTERVAL / ANIMATED_TIME_UNIT;

            // Threshold to check equalness for double value
            const double DOUBLE_EQUAL_THRESHOLD = 0.001;

            double currentScrollOffsetX = 0;
            double currentScrollOffsetY = 0;
            IScrollView scrollView;

            if (weakScrollView.TryGetTarget(out scrollView))
            {
                // Check to cancel previous scrolling
                var hashCode = scrollView.GetHashCode();
                if (_scollViewCancelMap.ContainsKey(hashCode))
                {
                    _scollViewCancelMap[hashCode].Cancel();
                }

                // Setup new cancellationTokenSource for this run
                var cancellationTokenSource = new CancellationTokenSource();
                _scollViewCancelMap[hashCode] = cancellationTokenSource;

                // Setup progressive scrolling settings
                currentScrollOffsetX = scrollView.HorizontalOffset;
                currentScrollOffsetY = scrollView.VerticalOffset;
                var biasX = (x - currentScrollOffsetX) / ANIMATED_TIME_SLOT;
                var biasY = (y - currentScrollOffsetY) / ANIMATED_TIME_SLOT;

                var isHorizontalScroll = Math.Abs(x - currentScrollOffsetX) >= DOUBLE_EQUAL_THRESHOLD;
                var isVerticalScroll = Math.Abs(y - currentScrollOffsetY) >= DOUBLE_EQUAL_THRESHOLD;

                while ((isHorizontalScroll || isVerticalScroll))
                {
                    cancellationTokenSource.Token.ThrowIfCancellationRequested();
                    if (isHorizontalScroll)
                    {
                        scrollView.ScrollToHorizontalOffset(currentScrollOffsetX + biasX);
                    }

                    if (isVerticalScroll)
                    {
                        scrollView.ScrollToVerticalOffset(currentScrollOffsetY + biasY);
                    }

                    await Task.Delay(TimeSpan.FromMilliseconds(ANIMATED_TIME_UNIT), cancellationTokenSource.Token);

                    // Setup next run's progressive scrolling settings
                    if (!weakScrollView.TryGetTarget(out scrollView))
                    {
                        break;
                    }
                    currentScrollOffsetX = scrollView.HorizontalOffset;
                    currentScrollOffsetY = scrollView.VerticalOffset;
                    isHorizontalScroll = Math.Abs(x - currentScrollOffsetX) >= DOUBLE_EQUAL_THRESHOLD;
                    isVerticalScroll = Math.Abs(y - currentScrollOffsetY) >= DOUBLE_EQUAL_THRESHOLD;
                }

                _scollViewCancelMap.Remove(hashCode);
            }
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
            public ScrollBarVisibility HorizontalScrollBarVisibility = ScrollBarVisibility.Disabled;
        }
    }
}
