// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using ReactNative.UIManager;
using ReactNative.UIManager.Events;
using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Input;
using System.Windows.Media;

namespace ReactNative.Touch
{
    class TouchHandler : IDisposable
    {
        private readonly FrameworkElement _view;
        private readonly List<ReactPointer> _pointers;

        private uint _pointerIDs;

        public TouchHandler(FrameworkElement view)
        {
            _view = view;
            _pointers = new List<ReactPointer>();

            _view.MouseDown += OnMouseDown;
            _view.MouseMove += OnMouseMove;
            _view.MouseUp += OnMouseUp;
            _view.TouchDown += OnTouchPressed;
            _view.TouchMove += OnTouchMoved;
            _view.TouchUp += OnTouchReleased;
        }

        public void Dispose()
        {
            _view.MouseDown -= OnMouseDown;
            _view.MouseMove -= OnMouseMove;
            _view.MouseUp -= OnMouseUp;
            _view.TouchDown -= OnTouchPressed;
            _view.TouchMove -= OnTouchMoved;
            _view.TouchUp -= OnTouchReleased;
        }

        public static void OnPointerEntered(DependencyObject view, MouseEventArgs e)
        {
            if (ShouldSendEnterLeaveEvent(view))
            {
                view.GetReactContext()
                    .GetNativeModule<UIManagerModule>()
                    .EventDispatcher
                    .DispatchEvent(
                        new PointerEnterExitEvent(TouchEventType.Entered, view.GetTag()));
            }
        }

        public static void OnPointerExited(DependencyObject view, MouseEventArgs e)
        {
            if (ShouldSendEnterLeaveEvent(view))
            {
                view.GetReactContext()
                    .GetNativeModule<UIManagerModule>()
                    .EventDispatcher
                    .DispatchEvent(
                        new PointerEnterExitEvent(TouchEventType.Exited, view.GetTag()));
            }
        }

        private void OnTouchPressed(object sender, TouchEventArgs e)
        {
            var originalSource = e.OriginalSource as DependencyObject;
            var rootPoint = e.GetTouchPoint(_view);
            var reactView = GetReactViewTarget(originalSource, rootPoint.Position);
            if (reactView != null && _view.CaptureTouch(e.TouchDevice))
            {
                var viewPoint = rootPoint.Position;
                var reactTag = reactView.GetReactCompoundView().GetReactTagAtPoint(reactView, viewPoint);
                var pointer = new ReactPointer();
                pointer.Target = reactTag;
                pointer.PointerId = (uint)rootPoint.TouchDevice.Id;
                pointer.Identifier = ++_pointerIDs;
                pointer.PointerType = "touch";
                pointer.IsLeftButton = false;
                pointer.IsRightButton = false;
                pointer.IsMiddleButton = false;
                pointer.IsHorizontalMouseWheel = false;
                pointer.IsEraser = false;
                pointer.ReactView = reactView;
                var timestamp = e.Timestamp;
                UpdatePointerForEvent(pointer, rootPoint.Position, viewPoint, timestamp);

                var pointerIndex = _pointers.Count;
                _pointers.Add(pointer);
                DispatchTouchEvent(TouchEventType.Start, _pointers, pointerIndex);
            }
        }

        private void OnMouseDown(object sender, MouseButtonEventArgs e)
        {
            var originalSource = e.OriginalSource as DependencyObject;
            var rootPoint = e.GetPosition(_view);
            var reactView = GetReactViewTarget(originalSource, rootPoint);
            if (reactView != null && _view.CaptureMouse())
            {
                var viewPoint = e.GetPosition(reactView);
                var reactTag = reactView.GetReactCompoundView().GetReactTagAtPoint(reactView, viewPoint);
                var pointer = new ReactPointer();
                pointer.Target = reactTag;
                pointer.PointerId = (uint)e.Device.GetHashCode();
                pointer.Identifier = ++_pointerIDs;
                pointer.PointerType = "mouse";
                pointer.IsLeftButton = e.LeftButton == MouseButtonState.Pressed;
                pointer.IsRightButton = e.RightButton == MouseButtonState.Pressed;
                pointer.IsMiddleButton = e.MiddleButton == MouseButtonState.Pressed;
                pointer.IsHorizontalMouseWheel = false;
                pointer.IsEraser = false;
                pointer.ReactView = reactView;
                var timestamp = e.Timestamp;
                UpdatePointerForEvent(pointer, rootPoint, viewPoint, timestamp);

                var pointerIndex = _pointers.Count;
                _pointers.Add(pointer);
                DispatchTouchEvent(TouchEventType.Start, _pointers, pointerIndex);
            }
        }

        private void OnTouchMoved(object sender, TouchEventArgs e)
        {
            if (_pointers != null && _pointers.Count > 0)
            {
                var pointer = _pointers[0];
                UpdatePointerForEvent(pointer, e);
                DispatchTouchEvent(TouchEventType.Move, _pointers, 0);
            }
        }

        private void OnMouseMove(object sender, MouseEventArgs e)
        {
            var pointerIndex = IndexOfPointerWithId((uint)e.Device.GetHashCode());
            if (_pointers != null && pointerIndex >= 0)
            {
                var pointer = _pointers[pointerIndex];
                UpdatePointerForEvent(pointer, e);
                DispatchTouchEvent(TouchEventType.Move, _pointers, pointerIndex);
            }
        }

        private void OnTouchReleased(object sender, TouchEventArgs e)
        {
            OnTouchConcluded(TouchEventType.End, e);
        }

        private void OnMouseUp(object sender, MouseButtonEventArgs e)
        {
            OnPointerConcluded(TouchEventType.End, e);
        }

        private void OnTouchConcluded(TouchEventType touchEventType, TouchEventArgs e)
        {
            if (_pointers != null && _pointers.Count > 0)
            {
                var pointer = _pointers[0];
                UpdatePointerForEvent(pointer, e);
                DispatchTouchEvent(touchEventType, _pointers, 0);

                _pointers.RemoveAt(0);

                if (_pointers.Count == 0)
                {
                    _pointerIDs = 0;
                }

                _view.ReleaseAllTouchCaptures();
            }
        }

        private void OnPointerConcluded(TouchEventType touchEventType, MouseButtonEventArgs e)
        {
            var pointerIndex = IndexOfPointerWithId((uint)e.Device.GetHashCode());
            if (_pointers != null && pointerIndex >= 0)
            {
                var pointer = _pointers[pointerIndex];
                UpdatePointerForEvent(pointer, e);
                DispatchTouchEvent(touchEventType, _pointers, pointerIndex);

                _pointers.RemoveAt(pointerIndex);

                if (_pointers.Count == 0)
                {
                    _pointerIDs = 0;
                }

                _view.ReleaseMouseCapture();
            }
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

        private UIElement GetReactViewTarget(DependencyObject originalSource, Point point)
        {
            // If the target is not a child of the root view, then this pointer
            // event does not belong to React.
            if (!RootViewHelper.IsReactSubview(originalSource))
            {
                return null;
            }

            // population of sources provided by:
            // http://stackoverflow.com/questions/2059475/what-is-the-silverlights-findelementsinhostcoordinates-equivalent-in-wpf
            var sources = new List<DependencyObject>();
            //changed from external edits, because VisualHit is
            //only a DependencyObject and may not be a UIElement
            //this could cause exceptions or may not be compiling at all
            //simply filter the result for class UIElement and
            //cast it to IEnumerable<UIElement> if you need
            //the very exact same result including type

            VisualTreeHelper.HitTest(
                _view,
                null,
                new HitTestResultCallback(
                    hit =>
                    {
                        var source = hit.VisualHit;
                        if (!source.HasTag())
                        {
                            return HitTestResultBehavior.Continue;
                        }
                        var pointerEvents = source.GetPointerEvents();
                        if (pointerEvents == PointerEvents.None || pointerEvents == PointerEvents.BoxNone)
                        {
                            return HitTestResultBehavior.Continue;
                        }

                        sources.Add(hit.VisualHit);
                        return HitTestResultBehavior.Continue;
                    }),
                new PointHitTestParameters(point));

            // Get the first React view that does not have pointer events set
            // to 'none' or 'box-none', and is not a child of a view with 
            // 'box-only' or 'none' settings for pointer events.

            // TODO: use pooled data structure
            var isBoxOnlyCache = new Dictionary<DependencyObject, bool>();
            foreach (var source in sources)
            {
                var viewHierarchy = RootViewHelper.GetReactViewHierarchy(source);
                var isBoxOnly = IsBoxOnlyWithCache(viewHierarchy, isBoxOnlyCache);
                if (!isBoxOnly)
                {
                    return (UIElement)source;
                }
            }

            return null;
        }

        private void UpdatePointerForEvent(ReactPointer pointer, TouchEventArgs e)
        {
            var rootPoint = e.GetTouchPoint(_view);
            var viewPoint = e.GetTouchPoint(pointer.ReactView);
            var timestamp = e.Timestamp;
            UpdatePointerForEvent(pointer, rootPoint.Position, viewPoint.Position, timestamp);
        }

        private void UpdatePointerForEvent(ReactPointer pointer, MouseButtonEventArgs e)
        {
            var rootPoint = e.GetPosition(_view);
            var viewPoint = e.GetPosition(pointer.ReactView);
            var timestamp = e.Timestamp;
            UpdatePointerForEvent(pointer, rootPoint, viewPoint, timestamp);
        }

        private void UpdatePointerForEvent(ReactPointer pointer, MouseEventArgs e)
        {
            var rootPoint = e.GetPosition(_view);
            var viewPoint = e.GetPosition(pointer.ReactView);
            var timestamp = e.Timestamp;
            UpdatePointerForEvent(pointer, rootPoint, viewPoint, timestamp);
        }

        private void UpdatePointerForEvent(ReactPointer pointer, Point rootPoint, Point viewPoint, int timestamp)
        {
            var positionInRoot = rootPoint;
            var positionInView = viewPoint;

            pointer.PageX = (float)positionInRoot.X;
            pointer.PageY = (float)positionInRoot.Y;
            pointer.LocationX = (float)positionInView.X;
            pointer.LocationY = (float)positionInView.Y;
            pointer.Timestamp = (ulong) timestamp;

            pointer.ShiftKey = (Keyboard.Modifiers & ModifierKeys.Shift) == ModifierKeys.Shift;
            pointer.AltKey = (Keyboard.Modifiers & ModifierKeys.Alt) == ModifierKeys.Alt;
            pointer.CtrlKey = (Keyboard.Modifiers & ModifierKeys.Control) == ModifierKeys.Control;
        }

        private void DispatchTouchEvent(TouchEventType touchEventType, List<ReactPointer> activePointers, int pointerIndex)
        {
            var touches = new JArray();
            foreach (var pointer in activePointers)
            {
                touches.Add(JObject.FromObject(pointer));
            }

            var changedIndices = new JArray();
            changedIndices.Add(JToken.FromObject(pointerIndex));

            var coalescingKey = activePointers[pointerIndex].PointerId;

            var touchEvent = new TouchEvent(touchEventType, touches, changedIndices, coalescingKey);

            _view.GetReactContext()?
                .GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(touchEvent);
        }

        private static bool IsBoxOnlyWithCache(IEnumerable<DependencyObject> hierarchy, IDictionary<DependencyObject, bool> cache)
        {
            var enumerator = hierarchy.GetEnumerator();

            // Skip the first element (only checking ancestors)
            if (!enumerator.MoveNext())
            {
                return false;
            }

            return IsBoxOnlyWithCacheRecursive(enumerator, cache);
        }

        private static bool IsBoxOnlyWithCacheRecursive(IEnumerator<DependencyObject> enumerator, IDictionary<DependencyObject, bool> cache)
        {
            if (!enumerator.MoveNext())
            {
                return false;
            }

            var currentView = enumerator.Current;
            var isBoxOnly = default(bool);
            if (!cache.TryGetValue(currentView, out isBoxOnly))
            {
                var pointerEvents = currentView.GetPointerEvents();

                isBoxOnly = pointerEvents == PointerEvents.BoxOnly 
                    || pointerEvents == PointerEvents.None
                    || IsBoxOnlyWithCacheRecursive(enumerator, cache);

                cache.Add(currentView, isBoxOnly);
            }

            return isBoxOnly;
        }

        private static bool ShouldSendEnterLeaveEvent(DependencyObject view)
        {
            // If the target is not a child of the root view, then this pointer
            // event does not belong to React.
            if (!RootViewHelper.IsReactSubview(view))
            {
                return false;
            }

            var viewHierarchy = RootViewHelper.GetReactViewHierarchy(view);
            foreach (var ancestor in viewHierarchy)
            {
                var pointerEvents = ancestor.GetPointerEvents();
                if (pointerEvents == PointerEvents.None || pointerEvents == PointerEvents.BoxNone)
                {
                    return false;
                }
            }

            return true;
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

            public override string EventName => _touchEventType.GetJavaScriptEventName();

            public override bool CanCoalesce => _touchEventType == TouchEventType.Move;

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

            public PointerEnterExitEvent(TouchEventType touchEventType, int viewTag)
                : base(viewTag)
            {
                _touchEventType = touchEventType;
            }

            public override string EventName => _touchEventType.GetJavaScriptEventName();

            public override bool CanCoalesce => false;

            public override void Dispatch(RCTEventEmitter eventEmitter)
            {
                var eventData = new JObject
                {
                    { "target", ViewTag },
                };

                var enterLeaveEventName = default(string);
                switch (_touchEventType)
                {
                    case TouchEventType.Entered:
                        enterLeaveEventName = "topMouseEnter";
                        break;
                    case TouchEventType.Exited:
                        enterLeaveEventName = "topMouseLeave";
                        break;
                }

                if (enterLeaveEventName != null)
                {
                    eventEmitter.receiveEvent(ViewTag, enterLeaveEventName, eventData);
                }

                eventEmitter.receiveEvent(ViewTag, EventName, eventData);
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
