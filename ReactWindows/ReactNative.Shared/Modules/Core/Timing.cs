using ReactNative.Bridge;
using ReactNative.Collections;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Threading;

namespace ReactNative.Modules.Core
{
    /// <summary>
    /// Native module for JavaScript timer execution.
    /// </summary>
    public class Timing : ReactContextNativeModuleBase, ILifecycleEventListener
    {
        private const string IdleChoreographerKey = nameof(Timing) + "_Idle";

        private static readonly TimeSpan FrameDuration = TimeSpan.FromTicks(166666);
        private static readonly TimeSpan IdleCallbackFrameDeadline = TimeSpan.FromMilliseconds(1);

        private readonly object _gate = new object();

        private readonly HeapBasedPriorityQueue<TimerData> _timers;

        private JSTimers _jsTimersModule;
        private bool _suspended;
        private bool _sendIdleEvents;

        /// <summary>
        /// Instantiates the <see cref="Timing"/> module.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        public Timing(ReactContext reactContext)
            : base(reactContext)
        {
            _timers = new HeapBasedPriorityQueue<TimerData>(
                Comparer<TimerData>.Create((x, y) => 
                    x.TargetTime.CompareTo(y.TargetTime)));
        }

        /// <summary>
        /// The name of the module.
        /// </summary>
        public override string Name
        {
            get
            {
                return "RCTTiming";
            }
        }

        /// <summary>
        /// Initializes the module.
        /// </summary>
        public override void Initialize()
        {
            _jsTimersModule = Context.GetJavaScriptModule<JSTimers>();
            Context.AddLifecycleEventListener(this);
        }

        /// <summary>
        /// Called when the host application suspends.
        /// </summary>
        public void OnSuspend()
        {
            _suspended = true;
            ReactChoreographer.Instance.JavaScriptEventsCallback -= DoFrameSafe;

            if (_sendIdleEvents)
            {
                ReactChoreographer.Instance.IdleCallback -= DoIdleCallbackSafe;
            }
        }

        /// <summary>
        /// Called when the host application resumes.
        /// </summary>
        public void OnResume()
        {
            _suspended = false;
            ReactChoreographer.Instance.JavaScriptEventsCallback += DoFrameSafe;

            if (_sendIdleEvents)
            {
                ReactChoreographer.Instance.IdleCallback += DoIdleCallbackSafe;
            }
        }

        /// <summary>
        /// Called when the host application is destroyed.
        /// </summary>
        public void OnDestroy()
        {
            ReactChoreographer.Instance.JavaScriptEventsCallback -= DoFrameSafe;

            if (_sendIdleEvents)
            {
                ReactChoreographer.Instance.IdleCallback -= DoIdleCallbackSafe;
            }
        }

        /// <summary>
        /// Creates a timer with the given properties.
        /// </summary>
        /// <param name="callbackId">The timer identifier.</param>
        /// <param name="duration">The duration in milliseconds.</param>
        /// <param name="jsSchedulingTime">
        /// The Unix timestamp when the timer was created.
        /// </param>
        /// <param name="repeat">
        /// A flag signaling if the timer should fire at intervals.
        /// </param>
        [ReactMethod]
        public void createTimer(
            int callbackId,
            int duration,
            double jsSchedulingTime,
            bool repeat)
        {
            if (duration == 0 && !repeat)
            {
                _jsTimersModule.callTimers(new[] { callbackId });
                return;
            }

            var period = TimeSpan.FromMilliseconds(duration);
            var scheduledTime = DateTimeOffset.FromUnixTimeMilliseconds((long)jsSchedulingTime);
            var initialTargetTime = (scheduledTime + period);

            var timer = new TimerData(callbackId, initialTargetTime, period, repeat);
            lock (_gate)
            {
                _timers.Enqueue(timer);
            }

            ReactChoreographer.Instance.ActivateCallback(nameof(Timing));
        }

        /// <summary>
        /// Removes a timer.
        /// </summary>
        /// <param name="timerId">The timer identifier.</param>
        [ReactMethod]
        public void deleteTimer(int timerId)
        {
            lock (_gate)
            {
                _timers.Remove(new TimerData(timerId));
                if (_timers.Count == 0)
                {
                    ReactChoreographer.Instance.DeactivateCallback(nameof(Timing));
                }
            }
        }

        /// <summary>
        /// Enable or disable idle events.
        /// </summary>
        /// <param name="sendIdleEvents">
        /// <code>true</code> if idle events should be enabled, otherwise
        /// <code>false</code>.
        /// </param>
        [ReactMethod]
        public void setSendIdleEvents(bool sendIdleEvents)
        {
            _sendIdleEvents = sendIdleEvents;
            if (_sendIdleEvents)
            {
                ReactChoreographer.Instance.IdleCallback += DoIdleCallbackSafe;
                ReactChoreographer.Instance.ActivateCallback(IdleChoreographerKey);
            }
            else
            {
                ReactChoreographer.Instance.IdleCallback -= DoIdleCallbackSafe;
                ReactChoreographer.Instance.DeactivateCallback(IdleChoreographerKey);
            }
        }

        private void DoFrameSafe(object sender, object e)
        {
            try
            {
                DoFrame(sender, e);
            }
            catch (Exception ex)
            {
                Context.HandleException(ex);
            }
        }

        private void DoFrame(object sender, object e)
        {
            if (Volatile.Read(ref _suspended))
            {
                return;
            }

            var ready = new List<int>();
            var now = DateTimeOffset.Now;

            lock (_gate)
            {
                while (_timers.Count > 0 && _timers.Peek().TargetTime < now)
                {
                    var next = _timers.Dequeue();
                    ready.Add(next.CallbackId);
                    var nextInterval = next.Increment(now);

                    if (nextInterval.CanExecute)
                    {
                        _timers.Enqueue(nextInterval);
                    }
                }

                if (_timers.Count == 0)
                {
                    ReactChoreographer.Instance.DeactivateCallback(nameof(Timing));
                }
            }

            if (ready.Count > 0)
            {
                _jsTimersModule.callTimers(ready);
            }
        }

        private void DoIdleCallbackSafe(object sender, FrameEventArgs e)
        {
            try
            {
                DoIdleCallback(sender, e);
            }
            catch (Exception ex)
            {
                Context.HandleException(ex);
            }
        }

        private void DoIdleCallback(object sender, FrameEventArgs e)
        {
            if (_sendIdleEvents)
            {
                var frameTime = e.FrameTime;
                var utcNow = DateTimeOffset.UtcNow;
                var remainingFrameTime = frameTime - utcNow;
                if (remainingFrameTime > IdleCallbackFrameDeadline)
                {
                    Context.GetJavaScriptModule<JSTimers>()
                        .callIdleCallbacks(frameTime.ToUnixTimeMilliseconds());
                }
            }
        }

        struct TimerData : IEquatable<TimerData>
        {
            private readonly bool _repeat;

            public TimerData(int callbackId)
            {
                CallbackId = callbackId;
                TargetTime = DateTimeOffset.MaxValue;
                Period = default(TimeSpan);
                _repeat = false;
                CanExecute = false;
            }

            public TimerData(int callbackId, DateTimeOffset initialTargetTime, TimeSpan period, bool repeat)
            {
                CallbackId = callbackId;
                TargetTime = initialTargetTime;
                Period = period;
                _repeat = repeat;

                CanExecute = true;
            }

            public int CallbackId { get; }

            public DateTimeOffset TargetTime { get; private set; }

            public TimeSpan Period { get; }

            public bool CanExecute { get; private set; }

            public TimerData Increment(DateTimeOffset now)
            {
                return new TimerData(CallbackId, now + Period, Period, _repeat)
                {
                    CanExecute = _repeat,
                };
            }

            public bool Equals(TimerData other)
            {
                return CallbackId == other.CallbackId &&
                    (TargetTime == other.TargetTime ||
                    TargetTime == DateTimeOffset.MaxValue ||
                    other.TargetTime == DateTimeOffset.MaxValue);
            }

            public override bool Equals(object obj)
            {
                if (obj is TimerData)
                {
                    return Equals((TimerData)obj);
                }

                return false;
            }

            public override int GetHashCode()
            {
                return CallbackId.GetHashCode();
            }
        }
    }
}
