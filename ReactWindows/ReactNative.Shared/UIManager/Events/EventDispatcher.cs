// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using ReactNative.Bridge;
using ReactNative.Modules.Core;
using ReactNative.Tracing;
using System;
using System.Collections.Generic;
using System.Threading;
using static System.FormattableString;

namespace ReactNative.UIManager.Events
{
    /// <summary>
    /// Class responsible for dispatching UI events to JavaScript. The main
    /// purpose of this class is to act as an intermediary between UI code
    /// generating events and JavaScript, making sure we don't send more events
    /// than JavaScript can process.
    /// 
    /// To use it, create a subclass of <see cref="Event"/> and call
    /// <see cref="DispatchEvent(Event)"/> whenever there is a UI event to
    /// dispatch.
    /// 
    /// This class differs from the Android implementation of React as there is
    /// no analogy to the choreographer in UWP. Instead, there is a self-managed
    /// callback that coalesces events on the JavaScript thread.
    /// 
    /// If JavaScript is taking a long time processing events, then the UI
    /// events generated on the dispatcher thread can be coalesced into fewer
    /// events so that, when the dispatch occurs, we do not overload JavaScript
    /// with a ton of events and cause it to get even farther behind.
    /// 
    /// Ideally, this is unnecessary and JavaScript is fast enough to process
    /// all the events each frame, but this is a reasonable precautionary 
    /// measure.
    /// </summary>
    /// <remarks>
    /// Event cookies are used to coalesce events. They are made up of the
    /// event type ID, view tag, and a custom coalescing key.
    /// 
    /// Event Cookie Composition:
    /// VIEW_TAG_MASK =       0x00000000ffffffff
    /// EVENT_TYPE_ID_MASK =  0x0000ffff00000000
    /// COALESCING_KEY_MASK = 0xffff000000000000
    /// </remarks>
    public class EventDispatcher : ILifecycleEventListener
    {
        private static IComparer<Event> s_eventComparer = Comparer<Event>.Create((x, y) =>
        {
            if (x == null && y == null)
            {
                return 0;
            }

            if (x == null)
            {
                return -1;
            }

            if (y == null)
            {
                return 1;
            }

            var value = x.Timestamp.CompareTo(y.Timestamp);
            if (value == 0)
            {
                return x.SortingKey.CompareTo(y.SortingKey);
            }

            return value;
        });

        private readonly object _eventsStagingLock = new object();
        private readonly object _eventsToDispatchLock = new object();

        private readonly IDictionary<long, int> _eventCookieToLastEventIndex = new Dictionary<long, int>();
        private readonly IDictionary<string, short> _eventNameToEventId = new Dictionary<string, short>();
        private readonly IList<Event> _eventStaging = new List<Event>();
        private readonly IList<IEventDispatcherListener> _listeners = new List<IEventDispatcherListener>();

        private readonly ReactContext _reactContext;

        private Event[] _eventsToDispatch = new Event[16];
        private int _eventsToDispatchSize = 0;
        private RCTEventEmitter _rctEventEmitter;
        private bool _hasDispatchScheduled = false;

        /// <summary>
        /// Instantiates the <see cref="EventDispatcher"/>.
        /// </summary>
        /// <param name="reactContext">The context.</param>
        public EventDispatcher(ReactContext reactContext)
        {
            if (reactContext == null)
                throw new ArgumentNullException(nameof(reactContext));

            _reactContext = reactContext;
            _reactContext.AddLifecycleEventListener(this);
        }

        /// <summary>
        /// Sends the given <see cref="Event"/> to JavaScript, coalescing
        /// events if JavaScript is backed up.
        /// </summary>
        /// <param name="event">The event.</param>
        public void DispatchEvent(Event @event)
        {
            if (@event == null)
                throw new ArgumentNullException(nameof(@event));

            foreach (var listener in _listeners)
            {
                listener.OnEventDispatch(@event);
            }
            
            lock (_eventsStagingLock)
            {
                _eventStaging.Add(@event);
            }

            ReactChoreographer.Instance.ActivateCallback(nameof(EventDispatcher));
        }

        /// <summary>
        /// Adds a listener to this <see cref="EventDispatcher"/>. 
        /// </summary>
        /// <param name="listener">The listener.</param>
        public void AddListener(IEventDispatcherListener listener)
        {
            _listeners.Add(listener);
        }

        /// <summary>
        /// Removes a listener from this <see cref="EventDispatcher"/>. 
        /// </summary>
        /// <param name="listener">The listener.</param>
        public void RemoveListener(IEventDispatcherListener listener)
        {
            _listeners.Remove(listener);
        }

        /// <summary>
        /// Called when the host receives the resume event.
        /// </summary>
        public void OnResume()
        {
            if (_rctEventEmitter == null)
            {
                _rctEventEmitter = _reactContext.GetJavaScriptModule<RCTEventEmitter>();
            }

            ReactChoreographer.Instance.JavaScriptEventsCallback += ScheduleDispatcherSafe;
        }

        /// <summary>
        /// Called when the host is shutting down.
        /// </summary>
        public void OnDestroy()
        {
            ClearCallback();
        }

        /// <summary>
        /// Called when the host receives the suspend event.
        /// </summary>
        public void OnSuspend()
        {
            ClearCallback();
        }

        /// <summary>
        /// Called before the React instance is disposed.
        /// </summary>
        public void OnReactInstanceDispose()
        {
            ClearCallback();
        }

        private void ClearCallback()
        {
            ReactChoreographer.Instance.JavaScriptEventsCallback -= ScheduleDispatcherSafe;
        }

        private void MoveStagedEventsToDispatchQueue()
        {
            lock (_eventsStagingLock)
            {
                lock (_eventsToDispatchLock)
                {
                    foreach (var @event in _eventStaging)
                    {
                        if (!@event.CanCoalesce)
                        {
                            AddEventToEventsToDispatch(@event);
                            continue;
                        }

                        var eventCookie = GetEventCookie(@event.ViewTag, @event.EventName, @event.CoalescingKey);
                        var eventToAdd = default(Event);
                        var eventToDispose = default(Event);

                        if (!_eventCookieToLastEventIndex.TryGetValue(eventCookie, out var lastEventIdx))
                        {
                            eventToAdd = @event;
                            _eventCookieToLastEventIndex.Add(eventCookie, _eventsToDispatchSize);
                        }
                        else
                        {
                            var lastEvent = _eventsToDispatch[lastEventIdx];
                            var coalescedEvent = @event.Coalesce(lastEvent);
                            if (coalescedEvent != lastEvent)
                            {
                                eventToAdd = coalescedEvent;
                                _eventCookieToLastEventIndex[eventCookie] = _eventsToDispatchSize;
                                eventToDispose = lastEvent;
                                _eventsToDispatch[lastEventIdx] = null;
                            }
                            else
                            {
                                eventToDispose = @event;
                            }
                        }

                        if (eventToAdd != null)
                        {
                            AddEventToEventsToDispatch(eventToAdd);
                        }

                        if (eventToDispose != null)
                        {
                            eventToDispose.Dispose();
                        }
                    }
                }

                _eventStaging.Clear();
                ReactChoreographer.Instance.DeactivateCallback(nameof(EventDispatcher));
            }
        }

        private long GetEventCookie(int viewTag, string eventName, short coalescingKey)
        {
            if (!_eventNameToEventId.TryGetValue(eventName, out var eventTypeId))
            {
                if (_eventNameToEventId.Count == short.MaxValue)
                {
                    throw new InvalidOperationException("Overflow of event type IDs.");
                }

                eventTypeId = (short)_eventNameToEventId.Count;
                _eventNameToEventId.Add(eventName, eventTypeId);
            }

            return GetEventCookie(viewTag, eventTypeId, coalescingKey);
        }

        private long GetEventCookie(int viewTag, short eventTypeId, short coalescingKey)
        {
            return ((long)viewTag) |
                (((long)eventTypeId) & 0xffff) << 32 |
                (((long)coalescingKey) & 0xffff) << 48;
        }

        private void ScheduleDispatcherSafe(object sender, object e)
        {
            try
            {
                ScheduleDispatch(sender, e);
            }
            catch (Exception ex)
            {
                _reactContext.HandleException(ex);
            }
        }

        private void ScheduleDispatch(object sender, object e)
        {
            DispatcherHelpers.AssertOnDispatcher();

            var activity = Tracer.Trace(Tracer.TRACE_TAG_REACT_BRIDGE, "ScheduleDispatch").Start();

            MoveStagedEventsToDispatchQueue();

            bool shouldDispatch;
            lock (_eventsToDispatchLock)
            {
                shouldDispatch = _eventsToDispatchSize > 0;
            }

            if (shouldDispatch && !Volatile.Read(ref _hasDispatchScheduled))
            {
                _hasDispatchScheduled = true;
                _reactContext.RunOnJavaScriptQueueThread(() =>
                {
                    DispatchEvents(activity);
                    activity?.Dispose();
                });

                return;
            }

            activity?.Dispose();
        }

        private void DispatchEvents(IDisposable activity)
        {
            using (Tracer.Trace(Tracer.TRACE_TAG_REACT_BRIDGE, "DispatchEvents").Start())
            {
                _hasDispatchScheduled = false;

                if (_rctEventEmitter == null)
                {
                    throw new InvalidOperationException(Invariant($"The '{nameof(RCTEventEmitter)}' must not be null."));
                }

                lock (_eventsToDispatchLock)
                {
                    if (_eventsToDispatchSize > 1)
                    {
                        Array.Sort(_eventsToDispatch, 0, _eventsToDispatchSize, s_eventComparer);
                    }

                    for (var eventIdx = 0; eventIdx < _eventsToDispatchSize; ++eventIdx)
                    {
                        var @event = _eventsToDispatch[eventIdx];
                        if (@event == null)
                        {
                            continue;
                        }

                        @event.Dispatch(_rctEventEmitter);
                        @event.Dispose();
                    }

                    ClearEventsToDispatch();
                    _eventCookieToLastEventIndex.Clear();
                }
            }
        }

        private void AddEventToEventsToDispatch(Event @event)
        {
            if (_eventsToDispatchSize == _eventsToDispatch.Length)
            {
                var temp = _eventsToDispatch;
                _eventsToDispatch = new Event[_eventsToDispatch.Length * 2];
                Array.Copy(temp, _eventsToDispatch, _eventsToDispatchSize);
            }

            _eventsToDispatch[_eventsToDispatchSize++] = @event;
        }

        private void ClearEventsToDispatch()
        {
            Array.Clear(_eventsToDispatch, 0, _eventsToDispatchSize);
            _eventsToDispatchSize = 0;
        }
    }
}
