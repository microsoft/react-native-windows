using ReactNative.Bridge;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Threading;
#if WINDOWS_UWP
using Windows.ApplicationModel.Core;
using Windows.UI.Core;
using Windows.UI.Xaml.Media;
#else
using System.Windows.Media;
using System.Windows.Threading;
#endif

namespace ReactNative.Modules.Core
{
    /// <summary>
    /// A simple action queue that allows us to control the order certain
    /// callbacks are executed within a given frame.
    /// </summary>
    public class ReactChoreographer : IDisposable
    {
#if WINDOWS_UWP
        private const CoreDispatcherPriority ActivatePriority = CoreDispatcherPriority.High;
#else
        private const DispatcherPriority ActivatePriority = DispatcherPriority.Send;
#endif
        private const int InactiveFrameCount = 120;

        private static readonly Stopwatch s_stopwatch = Stopwatch.StartNew();
        private static ReactChoreographer s_instance;

        private readonly object _gate = new object();
        private readonly HashSet<string> _callbackKeys = new HashSet<string>();

        private FrameEventArgs _frameEventArgs;
        private IMutableFrameEventArgs _mutableReference;
        private Timer _timer;
        private bool _isSubscribed;
        private bool _isSubscribing;
        private int _currentInactiveCount;

        private ReactChoreographer() { }

        /// <summary>
        /// For use by <see cref="UIManager.UIManagerModule"/>. 
        /// </summary>
        public event EventHandler<FrameEventArgs> DispatchUICallback;

        /// <summary>
        /// For use by <see cref="Animated.NativeAnimatedModule"/>. 
        /// </summary>
        public event EventHandler<FrameEventArgs> NativeAnimatedCallback;

        /// <summary>
        /// For events that make JavaScript do things.
        /// </summary>
        public event EventHandler<FrameEventArgs> JavaScriptEventsCallback;

        /// <summary>
        /// Event used to trigger the idle callback. Called after all UI work has been
        /// dispatched to JavaScript.
        /// </summary>
        public event EventHandler<FrameEventArgs> IdleCallback;

        /// <summary>
        /// The choreographer instance.
        /// </summary>
        public static ReactChoreographer Instance
        {
            get
            {
                if (s_instance == null)
                {
                    throw new InvalidOperationException("ReactChoreographer needs to be initialized.");
                }

                return s_instance;
            }
        }

        private static bool HasCoreWindow
        {
            get
            {
#if WINDOWS_UWP
                return CoreApplication.MainView.CoreWindow != null;
#else
                return true;
#endif
            }
        }

        private bool IsSimulated
        {
            get
            {
                lock (_gate)
                {
                    return _timer != null;
                }
            }
        }

        /// <summary>
        /// Initializes the <see cref="ReactChoreographer"/> instance.
        /// </summary>
        public static void Initialize()
        {
            if (s_instance == null)
            {
                DispatcherHelpers.AssertOnDispatcher();
                s_instance = new ReactChoreographer();
            }
        }

        /// <summary>
        /// Disposes the <see cref="ReactChoreographer"/> instance. 
        /// </summary>
        public static void Dispose()
        {
            if (s_instance != null)
            {
                DispatcherHelpers.AssertOnDispatcher();
                ((IDisposable)s_instance).Dispose();
                s_instance = null;
            }
        }

        /// <summary>
        /// Activate the callback for the given key.
        /// </summary>
        /// <param name="callbackKey">The callback key.</param>
        public void ActivateCallback(string callbackKey)
        {
            bool subscribe;
            lock (_gate)
            {
                var isSubscribed = Volatile.Read(ref _isSubscribed);
                var isSubscribing = Volatile.Read(ref _isSubscribing);
                subscribe = _isSubscribing =
                    _callbackKeys.Add(callbackKey)
                    && _callbackKeys.Count == 1
                    && !isSubscribed
                    && !isSubscribing;
            }

            if (subscribe)
            {
                DispatcherHelpers.RunOnDispatcher(
                    ActivatePriority,
                    () =>
                    {
                        lock (_gate)
                        {
                            Subscribe();
                            _isSubscribing = false;
                        }
                    });
            }
        }

        /// <summary>
        /// Deactivate the callback for the given key.
        /// </summary>
        /// <param name="callbackKey">The callback key.</param>
        public void DeactivateCallback(string callbackKey)
        {
            lock (_gate)
            {
                _callbackKeys.Remove(callbackKey);
            }
        }

        void IDisposable.Dispose()
        {
            if (_isSubscribed)
            {
                Unsubscribe();
            }
        }

        private void Subscribe()
        {
            if (!HasCoreWindow)
            {
                _timer = new Timer(OnTick, null, TimeSpan.Zero, TimeSpan.FromTicks(166666));
            }
            else
            {
                CompositionTarget.Rendering += OnRendering;
            }

            _isSubscribed = true;
        }

        private void Unsubscribe()
        {
            if (IsSimulated)
            {
                _timer.Dispose();
                _timer = null;
            }
            else
            {
                CompositionTarget.Rendering -= OnRendering;
            }

            _isSubscribed = false;
            _mutableReference = _frameEventArgs = null;
        }

        private void OnRendering(object sender, object e)
        {
            var renderingArgs = e as RenderingEventArgs;
            if (renderingArgs == null)
            {
                throw new InvalidOperationException("Expected rendering event arguments.");
            }

            OnRendering(sender, renderingArgs.RenderingTime);
        }

        private void OnTick(object state)
        {
            DispatcherHelpers.RunOnDispatcher(() =>
            {
                bool isSubscribed;
                lock (_gate)
                {
                    isSubscribed = _isSubscribed;
                }

                if (isSubscribed)
                {
                    OnRendering(null, s_stopwatch.Elapsed);
                }
            });
        }

        private void OnRendering(object sender, TimeSpan e)
        {
            var renderingTime = s_stopwatch.Elapsed;
            if (_frameEventArgs == null)
            {
                _mutableReference = _frameEventArgs = new FrameEventArgs(renderingTime);
            }
            else
            {
                _mutableReference.Update(renderingTime);
            }

            DispatchUICallback?.Invoke(this, _frameEventArgs);
            NativeAnimatedCallback?.Invoke(this, _frameEventArgs);
            JavaScriptEventsCallback?.Invoke(this, _frameEventArgs);
            IdleCallback?.Invoke(this , _frameEventArgs);

            lock (_gate)
            {
                if (_callbackKeys.Count == 0)
                {
                    if (++_currentInactiveCount >= InactiveFrameCount)
                    {
                        Unsubscribe();
                    }
                }
                else
                {
                    _currentInactiveCount = 0;
                }
            }
        }
    }
}
