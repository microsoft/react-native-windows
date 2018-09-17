// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using ReactNative.UIManager;
using ReactNative.UIManager.Events;
using System;
using System.Collections.Generic;
using System.Linq;
using Windows.Foundation;
using Windows.Foundation.Metadata;
using Windows.Graphics.Display;
using Windows.System;
using Windows.UI.Core;
using Windows.UI.Input;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;

namespace ReactNative.Touch
{
    class TouchHandler : IDisposable
    {
        private readonly FrameworkElement _view;
        private readonly List<ReactPointer> _pointers;
        private readonly Dictionary<RoutedEvent, PointerEventHandler> _pointerHandlers;

        private uint _pointerIDs;

        private readonly Dictionary<uint, HashSet<DependencyObject>> _pointersInViews;

        public TouchHandler(FrameworkElement view)
        {
            _view = view;
            _pointers = new List<ReactPointer>();
            _pointerHandlers = new Dictionary<RoutedEvent, PointerEventHandler>()
            {
                { UIElement.PointerPressedEvent, new PointerEventHandler(OnPointerPressed) },
                { UIElement.PointerMovedEvent, new PointerEventHandler(OnPointerMoved) },
                { UIElement.PointerReleasedEvent, new PointerEventHandler(OnPointerReleased) },
                { UIElement.PointerCanceledEvent, new PointerEventHandler(OnPointerCanceled) },
                { UIElement.PointerCaptureLostEvent, new PointerEventHandler(OnPointerCaptureLost) },
                { UIElement.PointerExitedEvent, new PointerEventHandler(OnPointerExited) },
            };

            _pointersInViews = new Dictionary<uint, HashSet<DependencyObject>>();

            foreach (KeyValuePair<RoutedEvent, PointerEventHandler> handler in _pointerHandlers)
            {
                _view.AddHandler(handler.Key, handler.Value, true);
            }
        }

        public void Dispose()
        {
            foreach (KeyValuePair<RoutedEvent, PointerEventHandler> handler in _pointerHandlers)
            {
                _view.RemoveHandler(handler.Key, handler.Value);
            }
            _pointersInViews.Clear();
        }

        private void OnPointerPressed(object sender, PointerRoutedEventArgs e)
        {
            var pointerId = e.Pointer.PointerId;
            if (IndexOfPointerWithId(pointerId) != -1)
            {
                throw new InvalidOperationException("A pointer with this ID already exists.");
            }

            var originalSource = e.OriginalSource as DependencyObject;
            var rootPoint = e.GetCurrentPoint(_view);
            var reactView = GetReactViewTarget(e) as UIElement;
            if (reactView != null && _view.CapturePointer(e.Pointer))
            {
                // Pointer pressing updates the enter/leave state
                UpdatePointersInViews(reactView, rootPoint, e);

                var pointer = CreateReactPointer(reactView, rootPoint, e, true);
                var pointerIndex = _pointers.Count;
                _pointers.Add(pointer);
                DispatchTouchEvent(TouchEventType.Start, _pointers, pointerIndex);
            }
        }

        private void OnPointerMoved(object sender, PointerRoutedEventArgs e)
        {
            var pointerIndex = IndexOfPointerWithId(e.Pointer.PointerId);
            if (pointerIndex != -1)
            {
                // Touch/Dragging case
                var pointer = _pointers[pointerIndex];
                UpdatePointerForEvent(pointer, e);
                DispatchTouchEvent(TouchEventType.Move, _pointers, pointerIndex);
            }
            else
            {
                // Moving case (with no buttons pressed)
                var originalSource = e.OriginalSource as DependencyObject;
                var rootPoint = e.GetCurrentPoint(_view);
                var reactView = GetReactViewTarget(e) as UIElement;
                if (reactView != null)
                {
                    // Moving a non captured pointer updates the enter/leave state
                    UpdatePointersInViews(reactView, rootPoint, e);
                    if (ShouldSendPointerMoveEvent(reactView))
                    {
                        var pointer = CreateReactPointer(reactView, rootPoint, e, true);
                        pointerIndex = _pointers.Count;
                        _pointers.Add(pointer);
                        DispatchTouchEvent(TouchEventType.PointerMove, _pointers, pointerIndex);
                        _pointers.Remove(pointer);
                        if (_pointers.Count == 0)
                        {
                            _pointerIDs = 0;
                        }
                    }
                }
            }
        }

        private void OnPointerReleased(object sender, PointerRoutedEventArgs e)
        {
            OnPointerConcluded(TouchEventType.End, e);
        }

        private void OnPointerCanceled(object sender, PointerRoutedEventArgs e)
        {
            OnPointerConcluded(TouchEventType.Cancel, e);
        }

        private void OnPointerCaptureLost(object sender, PointerRoutedEventArgs e)
        {
            OnPointerConcluded(TouchEventType.Cancel, e);
        }

        private void OnPointerConcluded(TouchEventType touchEventType, PointerRoutedEventArgs e)
        {
            var pointerIndex = IndexOfPointerWithId(e.Pointer.PointerId);
            if (pointerIndex != -1)
            {
                var pointer = _pointers[pointerIndex];
                UpdatePointerForEvent(pointer, e);
                DispatchTouchEvent(touchEventType, _pointers, pointerIndex);

                _pointers.RemoveAt(pointerIndex);

                if (_pointers.Count == 0)
                {
                    _pointerIDs = 0;
                }

                _view.ReleasePointerCapture(e.Pointer);
            }
        }

        private void OnPointerExited(object sender, PointerRoutedEventArgs e)
        {
            // UWP only calls this on the "_view" when pointer exits it. There's no bubbling from children involved
            // Documentation is a little confusing on this one.

            // Update the "pointers in views" state
            var rootPoint = e.GetCurrentPoint(_view);
            UpdatePointersInViews(null, rootPoint, e);
        }

        private int IndexOfPointerWithId(uint pointerId)
        {
            for (var i = 0; i < _pointers.Count; ++i)
            {
                if (_pointers[i].PointerId == pointerId)
                {
                    return i;
                }
            }

            return -1;
        }

        private DependencyObject GetReactViewTarget(PointerRoutedEventArgs e)
        {
            // Ensure the original source is a DependencyObject
            var originalSource = e.OriginalSource as DependencyObject;
            if (originalSource == null)
            {
                return null;
            }

            // Get the React view hierarchy from the original source.
            var enumerator = RootViewHelper.GetReactViewHierarchy(originalSource).GetEnumerator();

            // Try to get the first React view.
            if (!enumerator.MoveNext())
            {
                return null;
            }

            // If the first React view has `pointerEvents: box-none`, revert
            // to using the `VisualTreeHelper` to find potentially occluded views.
            // This condition should be rare.
            if (enumerator.Current.GetPointerEvents() == PointerEvents.BoxNone)
            {
                var rootPoint = e.GetCurrentPoint(_view);
                var transform = _view.TransformToVisual(Window.Current.Content);
                var point = transform.TransformPoint(rootPoint.Position);
                var adjustedPoint = AdjustPointForStatusBar(point);

                // Get the first view in at the pointer point that is not `box-none`.
                // Note: Rounding related errors can keep the position outside all react views, hence using FirstOrDefault.
                var nonBoxOnlyView = VisualTreeHelper.FindElementsInHostCoordinates(adjustedPoint, _view)
                    .FirstOrDefault(v => v.HasTag() && v.GetPointerEvents() != PointerEvents.BoxNone);

                // Update the enumerator for the non-`box-only` view.
                enumerator = RootViewHelper.GetReactViewHierarchy(nonBoxOnlyView).GetEnumerator();
                if (!enumerator.MoveNext())
                {
                    return null;
                }
            }

            // Views with `pointerEvents: none` are not hit test enabled, so we
            // will not encounter any view subtrees with this value. Given the
            // prior conditional for `pointerEvents: box-none`, we only need to
            // loop through the parents of the current React view target to
            // check for uses of `pointerEvents: box-only`. If found, the
            // parent becomes the new target.
            var source = enumerator.Current;
            while (enumerator.MoveNext())
            {
                var current = enumerator.Current;
                if (source == null || current.GetPointerEvents() == PointerEvents.BoxOnly)
                {
                    source = current;
                }
            }

            return source;
        }
       
        private void UpdatePointersInViews(UIElement reactView, PointerPoint rootPoint, PointerRoutedEventArgs e)
        {
            // Create list of react views that should be tracked based on the
            // view hierarchy starting from reactView, keeping in just the views that intersect the rootPoint
            var newViews = reactView != null ? new HashSet<DependencyObject>(
                    RootViewHelper.GetReactViewHierarchy(reactView).Where((v) =>
                    {
                        if (v is FrameworkElement element)
                        {
                            var viewPoint = e.GetCurrentPoint(element);
                            return viewPoint.Position.X >= 0 && viewPoint.Position.X < element.ActualWidth &&
                                    viewPoint.Position.Y >= 0 && viewPoint.Position.Y < element.ActualHeight;
                        }
                        else
                        {
                            return false;
                        }
                    })) :
                new HashSet<DependencyObject>();

            // Get existing list of react views for the pointer id
            HashSet<DependencyObject> existingViews;
            if (!_pointersInViews.TryGetValue(e.Pointer.PointerId, out existingViews))
            {
                existingViews = new HashSet<DependencyObject>();
            }

            // Return quick if list didn't change
            if (newViews.SetEquals(existingViews))
            {
                return;
            }

            // Notify the tags that disappeared from the list if:
            // - there's still a tag associated with the view (it hasn't been removed from tree)
            // - there's a need (driven by handlers hooked on the JS side) to send events
            foreach (var existingView in existingViews)
            {
                if (!newViews.Contains(existingView) &&
                    existingView.HasTag() &&
                    ShouldSendPointerEnterLeaveOverOutEvent(existingView, out var enterOrLeave, out var overOrOut))
                {
                    OnPointerEnteredExited(TouchEventType.Exited, (UIElement)existingView, rootPoint, e, enterOrLeave, overOrOut);
                }
            }

            // Notify the new views that showed up if:
            // - there's a need (driven by handlers hooked on the JS side) to send events
            foreach (var newView in newViews)
            {
                if (!existingViews.Contains(newView) &&
                    ShouldSendPointerEnterLeaveOverOutEvent(newView, out var enterOrLeave, out var overOrOut))
                {
                    OnPointerEnteredExited(TouchEventType.Entered, (UIElement)newView, rootPoint, e, enterOrLeave, overOrOut);
                }
            }

            _pointersInViews[e.Pointer.PointerId] = newViews;
        }

        private ReactPointer CreateReactPointer(UIElement reactView, PointerPoint rootPoint, PointerRoutedEventArgs e, bool detectSubcomponent)
        {
            var viewPoint = e.GetCurrentPoint(reactView);
            var reactTag = detectSubcomponent ?
                reactView.GetReactCompoundView().GetReactTagAtPoint(reactView, viewPoint.Position) :
                reactView.GetTag();
            var pointer = new ReactPointer
            {
                Target = reactTag,
                PointerId = e.Pointer.PointerId,
                Identifier = ++_pointerIDs,
                PointerType = e.Pointer.PointerDeviceType.GetPointerDeviceTypeName(),
                IsLeftButton = viewPoint.Properties.IsLeftButtonPressed,
                IsRightButton = viewPoint.Properties.IsRightButtonPressed,
                IsMiddleButton = viewPoint.Properties.IsMiddleButtonPressed,
                IsHorizontalMouseWheel = viewPoint.Properties.IsHorizontalMouseWheel,
                IsEraser = viewPoint.Properties.IsEraser,
                ReactView = reactView,
            };

            UpdatePointerForEvent(pointer, rootPoint, viewPoint, e.KeyModifiers);
            return pointer;
        }

        private void OnPointerEnteredExited(TouchEventType eventType, UIElement view, PointerPoint rootPoint, PointerRoutedEventArgs e, bool sendDirect, bool sendBubbled)
        {
            var pointer = CreateReactPointer(view, rootPoint, e, false);
            view.GetReactContext()?
                    .GetNativeModule<UIManagerModule>()
                    .EventDispatcher
                    .DispatchEvent(
                        new PointerEnterExitEvent(eventType, view.GetTag(), pointer, sendDirect, sendBubbled));
            // Keeps resetting the identifier if no other non-moving pointer is present
            if (_pointers.Count == 0)
            {
                _pointerIDs = 0;
            }
        }

        private void UpdatePointerForEvent(ReactPointer pointer, PointerRoutedEventArgs e)
        {
            var rootPoint = e.GetCurrentPoint(_view);
            var viewPoint = e.GetCurrentPoint(pointer.ReactView);
            UpdatePointerForEvent(pointer, rootPoint, viewPoint, e.KeyModifiers);
        }

        private void UpdatePointerForEvent(ReactPointer pointer, PointerPoint rootPoint, PointerPoint viewPoint, VirtualKeyModifiers keyModifiers)
        {
            var positionInRoot = rootPoint.Position;
            var positionInView = viewPoint.Position;

            pointer.PageX = (float)positionInRoot.X;
            pointer.PageY = (float)positionInRoot.Y;
            pointer.LocationX = (float)positionInView.X;
            pointer.LocationY = (float)positionInView.Y;
            pointer.Timestamp = rootPoint.Timestamp / 1000; // Convert microseconds to milliseconds;
            pointer.Force = rootPoint.Properties.Pressure;
            pointer.IsBarrelButtonPressed = rootPoint.Properties.IsBarrelButtonPressed;

            pointer.ShiftKey = keyModifiers.HasFlag(VirtualKeyModifiers.Shift);
            pointer.AltKey = keyModifiers.HasFlag(VirtualKeyModifiers.Menu);
            pointer.CtrlKey = keyModifiers.HasFlag(VirtualKeyModifiers.Control);
        }

        private static bool ShouldSendPointerMoveEvent(DependencyObject view)
        {
            // This is a bubbling event, so we have to check if mouse move handler is hooked to any ancestor 
            return RootViewHelper.GetReactViewHierarchy(view).Any(
                v => v.GetMouseHandlerPresent(
                    ViewExtensions.MouseHandlerMask.MouseMove |
                    ViewExtensions.MouseHandlerMask.MouseMoveCapture));
        }

        private static bool ShouldSendPointerEnterLeaveOverOutEvent(DependencyObject view, out bool enterOrLeave, out bool overOrOut)
        {
            // "view" parameter can be any view in the view hierarchy starting from the chosen react target.
            // Due to the way the choosing of the react target works, these views are guaranteed not to be "box-only" nor "none".
            // Still, some can be "box-none", and they should NOT be chosen as enter/leave/over/out targets
            if (view.GetPointerEvents() == PointerEvents.BoxNone)
            {
                enterOrLeave = false;
                overOrOut = false;
                return false;
            }

            enterOrLeave = view.GetMouseHandlerPresent(
                    ViewExtensions.MouseHandlerMask.MouseEnter |
                    ViewExtensions.MouseHandlerMask.MouseLeave);

            overOrOut = RootViewHelper.GetReactViewHierarchy(view).Any(
                v => v.GetMouseHandlerPresent(
                    ViewExtensions.MouseHandlerMask.MouseOver |
                    ViewExtensions.MouseHandlerMask.MouseOverCapture |
                    ViewExtensions.MouseHandlerMask.MouseOut |
                    ViewExtensions.MouseHandlerMask.MouseOutCapture));

            return enterOrLeave || overOrOut;
        }

        private void DispatchTouchEvent(TouchEventType touchEventType, List<ReactPointer> activePointers, int pointerIndex)
        {
            var touches = new JArray();
            foreach (var pointer in activePointers)
            {
                touches.Add(JObject.FromObject(pointer));
            }

            var changedIndices = new JArray
            {
                pointerIndex
            };

            var coalescingKey = activePointers[pointerIndex].PointerId;

            var touchEvent = new TouchEvent(touchEventType, touches, changedIndices, coalescingKey);

            _view.GetReactContext()?
                .GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(touchEvent);
        }
        
        private static Point AdjustPointForStatusBar(Point point)
        {
            var currentOrientation = DisplayInformation.GetForCurrentView().CurrentOrientation;
            if (currentOrientation != DisplayOrientations.Landscape &&
                currentOrientation != DisplayOrientations.LandscapeFlipped &&
                ApiInformation.IsTypePresent("Windows.UI.ViewManagement.StatusBar"))
            {
                var rect = StatusBar.GetForCurrentView().OccludedRect;
                point.Y += rect.Height;
            }

            return point;
        }

        class TouchEvent : Event
        {
            private readonly TouchEventType _touchEventType;
            private readonly JArray _touches;
            private readonly JArray _changedIndices;
            private readonly uint _coalescingKey;

            public TouchEvent(TouchEventType touchEventType, JArray touches, JArray changedIndices, uint coalescingKey)
                : base(-1)
            {
                _touchEventType = touchEventType;
                _touches = touches;
                _changedIndices = changedIndices;
                _coalescingKey = coalescingKey;
            }

            public override string EventName
            {
                get
                {
                    return _touchEventType.GetJavaScriptEventName();
                }
            }

            public override bool CanCoalesce
            {
                get
                {
                    return _touchEventType == TouchEventType.Move || _touchEventType == TouchEventType.PointerMove;
                }
            }

            public override short CoalescingKey
            {
                get
                {
                    unchecked
                    {
                        return (short)_coalescingKey;
                    }
                }
            }

            public override void Dispatch(RCTEventEmitter eventEmitter)
            {
                eventEmitter.receiveTouches(EventName, _touches, _changedIndices);
            }
        }

        class PointerEnterExitEvent : Event
        {
            private readonly TouchEventType _touchEventType;
            private readonly ReactPointer _pointer;
            private readonly bool _sendDirect;
            private readonly bool _sendBubbled;

            public PointerEnterExitEvent(TouchEventType touchEventType, int viewTag, ReactPointer pointer, bool sendDirect, bool sendBubbled)
                : base(viewTag)
            {
                _touchEventType = touchEventType;
                _pointer = pointer;
                _sendDirect = sendDirect;
                _sendBubbled = sendBubbled;
            }

            public override string EventName
            {
                get
                {
                    return _touchEventType.GetJavaScriptEventName();
                }
            }

            public override bool CanCoalesce
            {
                get
                {
                    return false;
                }
            }

            public override void Dispatch(RCTEventEmitter eventEmitter)
            {
                var eventData = JObject.FromObject(_pointer);

                var enterLeaveEventName = default(string);
                if (_touchEventType == TouchEventType.Entered)
                {
                    enterLeaveEventName = "topMouseEnter";
                }
                else if (_touchEventType == TouchEventType.Exited)
                {
                    enterLeaveEventName = "topMouseLeave";
                }

                if (_sendDirect)
                {
                    if (enterLeaveEventName != null)
                    {
                        eventEmitter.receiveEvent(ViewTag, enterLeaveEventName, eventData);
                    }
                }

                if (_sendBubbled)
                {
                    eventEmitter.receiveEvent(ViewTag, EventName, eventData);
                }
            }
        }

        class ReactPointer
        {
            [JsonProperty(PropertyName = "target")]
            public int Target { get; set; }

            [JsonIgnore]
            public uint PointerId { get; set; }

            [JsonProperty(PropertyName = "identifier")]
            public uint Identifier { get; set; }

            [JsonIgnore]
            public UIElement ReactView { get; set; }

            [JsonProperty(PropertyName = "timestamp")]
            public ulong Timestamp { get; set; }

            [JsonProperty(PropertyName = "locationX")]
            public float LocationX { get; set; }

            [JsonProperty(PropertyName = "locationY")]
            public float LocationY { get; set; }

            [JsonProperty(PropertyName = "pageX")]
            public float PageX { get; set; }

            [JsonProperty(PropertyName = "pageY")]
            public float PageY { get; set; }

            [JsonProperty(PropertyName = "pointerType")]
            public string PointerType { get; set; }

            [JsonProperty(PropertyName = "force")]
            public double Force { get; set; }

            [JsonProperty(PropertyName = "isLeftButton", DefaultValueHandling = DefaultValueHandling.Ignore)]
            public bool IsLeftButton { get; set; }

            [JsonProperty(PropertyName = "isRightButton", DefaultValueHandling = DefaultValueHandling.Ignore)]
            public bool IsRightButton { get; set; }

            [JsonProperty(PropertyName = "isMiddleButton", DefaultValueHandling = DefaultValueHandling.Ignore)]
            public bool IsMiddleButton { get; set; }

            [JsonProperty(PropertyName = "isBarrelButtonPressed", DefaultValueHandling = DefaultValueHandling.Ignore)]
            public bool IsBarrelButtonPressed { get; set; }

            [JsonProperty(PropertyName = "isHorizontalScrollWheel", DefaultValueHandling = DefaultValueHandling.Ignore)]
            public bool IsHorizontalMouseWheel { get; set; }

            [JsonProperty(PropertyName = "isEraser", DefaultValueHandling = DefaultValueHandling.Ignore)]
            public bool IsEraser { get; set; }

            [JsonProperty(PropertyName = "shiftKey", DefaultValueHandling = DefaultValueHandling.Ignore)]
            public bool ShiftKey { get; set; }

            [JsonProperty(PropertyName = "ctrlKey", DefaultValueHandling = DefaultValueHandling.Ignore)]
            public bool CtrlKey { get; set; }

            [JsonProperty(PropertyName = "altKey", DefaultValueHandling = DefaultValueHandling.Ignore)]
            public bool AltKey { get; set; }
        }
    }
}
