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

            _view.MouseDown += OnPointerPressed;
            _view.MouseMove += OnPointerMoved;
            _view.MouseUp += OnPointerReleased;
        }

        public void Dispose()
        {
            _view.MouseDown -= OnPointerPressed;
            _view.MouseMove -= OnPointerMoved;
            _view.MouseUp -= OnPointerReleased;
        }

        private void OnPointerPressed(object sender, MouseButtonEventArgs e)
        {
            var pointerId = e.Pointer.PointerId;
            if (IndexOfPointerWithId(pointerId) != -1)
            {
                throw new InvalidOperationException("A pointer with this ID already exists.");
            }

            var originalSource = e.OriginalSource as DependencyObject;
            var rootPoint = e.GetCurrentPoint(_view);
            var reactView = GetReactViewTarget(originalSource, rootPoint.Position);
            if (reactView != null && _view.CapturePointer(e.Pointer))
            {
                var viewPoint = e.GetCurrentPoint(reactView);
                var reactTag = reactView.GetReactCompoundView().GetReactTagAtPoint(reactView, viewPoint.Position);
                var pointer = new ReactPointer();
                pointer.Target = reactTag;
                pointer.PointerId = e.Pointer.PointerId;
                pointer.Identifier = ++_pointerIDs;
                pointer.PointerType = e.Pointer.PointerDeviceType.GetPointerDeviceTypeName();
                pointer.IsLeftButton = viewPoint.Properties.IsLeftButtonPressed;
                pointer.IsRightButton = viewPoint.Properties.IsRightButtonPressed;
                pointer.IsMiddleButton = viewPoint.Properties.IsMiddleButtonPressed;
                pointer.IsHorizontalMouseWheel = viewPoint.Properties.IsHorizontalMouseWheel;
                pointer.IsEraser = viewPoint.Properties.IsEraser;
                pointer.ReactView = reactView;
                UpdatePointerForEvent(pointer, rootPoint, viewPoint);

                var pointerIndex = _pointers.Count;
                _pointers.Add(pointer);
                DispatchTouchEvent(TouchEventType.Start, _pointers, pointerIndex);
            }
        }

        private void OnPointerMoved(object sender, MouseEventArgs e)
        {
            var pointerIndex = IndexOfPointerWithId(e.Pointer.PointerId);
            if (pointerIndex != -1)
            {
                var pointer = _pointers[pointerIndex];
                UpdatePointerForEvent(pointer, e);
                DispatchTouchEvent(TouchEventType.Move, _pointers, pointerIndex);
            }
        }

        private void OnPointerReleased(object sender, MouseButtonEventArgs e)
        {
            OnPointerConcluded(TouchEventType.End, e);
        }

        private void OnPointerCanceled(object sender, MouseButtonEventArgs e)
        {
            OnPointerConcluded(TouchEventType.Cancel, e);
        }

        private void OnPointerCaptureLost(object sender, MouseButtonEventArgs e)
        {
            OnPointerConcluded(TouchEventType.Cancel, e);
        }

        private void OnPointerConcluded(TouchEventType touchEventType, MouseButtonEventArgs e)
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

            var sources = VisualTreeHelper.FindElementsInHostCoordinates(point, _view);

            // Get the first React view that does not have pointer events set
            // to 'none' or 'box-none', and is not a child of a view with 
            // 'box-only' or 'none' settings for pointer events.

            // TODO: use pooled data structure
            var isBoxOnlyCache = new Dictionary<DependencyObject, bool>();
            foreach (var source in sources)
            {
                if (!source.HasTag())
                {
                    continue;
                }

                var pointerEvents = source.GetPointerEvents();
                if (pointerEvents == PointerEvents.None || pointerEvents == PointerEvents.BoxNone)
                {
                    continue;
                }

                var viewHierarchy = RootViewHelper.GetReactViewHierarchy(source);
                var isBoxOnly = IsBoxOnlyWithCache(viewHierarchy, isBoxOnlyCache);
                if (!isBoxOnly)
                {
                    return source;
                }
            }

            return null;
        }

        private void UpdatePointerForEvent(ReactPointer pointer, MouseButtonEventArgs e)
        {
            var rootPoint = e.GetCurrentPoint(_view);
            var viewPoint = e.GetCurrentPoint(pointer.ReactView);
            UpdatePointerForEvent(pointer, rootPoint, viewPoint);
        }

        private void UpdatePointerForEvent(ReactPointer pointer, PointerPoint rootPoint, PointerPoint viewPoint)
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

            _view.GetReactContext()
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

        class TouchEvent : Event
        {
            private readonly TouchEventType _touchEventType;
            private readonly JArray _touches;
            private readonly JArray _changedIndices;
            private readonly uint _coalescingKey;

            public TouchEvent(TouchEventType touchEventType, JArray touches, JArray changedIndices, uint coalescingKey)
                : base(-1, TimeSpan.FromTicks(Environment.TickCount))
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
                    return _touchEventType == TouchEventType.Move;
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
        }
    }
}
