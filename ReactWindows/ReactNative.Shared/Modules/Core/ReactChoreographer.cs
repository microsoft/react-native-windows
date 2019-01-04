// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using ReactNative.Bridge;
using System;
using System.Collections.Concurrent;
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
    /// <inheritdoc />
    public class ReactChoreographer : IReactChoreographer
    {
#if WINDOWS_UWP
        private const CoreDispatcherPriority ActivatePriority = CoreDispatcherPriority.High;
        private readonly CoreApplicationView _applicationView;
        private readonly CoreDispatcher _coreDispatcher;
        private static ConcurrentDictionary<CoreDispatcher, ReactChoreographer> s_threadInstanceMap =
            new ConcurrentDictionary<CoreDispatcher, ReactChoreographer>();
#else
        private const DispatcherPriority ActivatePriority = DispatcherPriority.Send;
#endif
        private const int InactiveFrameCount = 120;

        private static readonly Stopwatch _stopwatch = Stopwatch.StartNew();
        private static IReactChoreographer s_instance;

        private readonly object _gate = new object();
        private readonly HashSet<string> _callbackKeys = new HashSet<string>();

        private FrameEventArgs _frameEventArgs;
        private IMutableFrameEventArgs _mutableReference;
        private Timer _timer;
        private bool _isSubscribed;
        private bool _isSubscribing;
        private bool _isDisposed;
        private int _currentInactiveCount;

#if WINDOWS_UWP
        private ReactChoreographer() : this(CoreApplication.MainView) { }

        private ReactChoreographer(CoreApplicationView applicationView)
        {
            _applicationView = applicationView;

            // Corner case: UWP scenarios that start with no main window.
            // This may look confusing, but _applicationView.Dispatcher seems to be accessible here (a thread corresponding to
            // main dispatcher or layout manager), yet it may not be yet accessible from the thread pool threads that'll soon
            // call ActivateCallback.
            _coreDispatcher = applicationView.Dispatcher;

            if (!s_threadInstanceMap.TryAdd(_coreDispatcher, this))
            {
                throw new InvalidOperationException("Dispatcher thread already has a choreogrpaher");
            }
        }

        /// <summary>
        /// The choreographer instance corresponding to the calling dispatcher thread
        /// </summary>
        public static IReactChoreographer ThreadInstance
        {
            get
            {
                var currentView = CoreApplication.GetCurrentView();
                if (currentView == null)
                {
                    throw new InvalidOperationException("ThreadInstance called from a non-dispatcher thread");
                }

                if (!s_threadInstanceMap.TryGetValue(currentView.Dispatcher, out var instance))
                {
                    throw new InvalidOperationException("Dispatcher thread has no choreographer");
                }
                return instance;
            }
        }
#else
        private ReactChoreographer() { }

        /// <summary>
        /// The choreographer instance (returns same as Instance for WPF)
        /// </summary>
       public static IReactChoreographer ThreadInstance => s_instance;
#endif

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
        /// The choreographer main instance.
        /// </summary>
        public static IReactChoreographer Instance
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

        /// <summary>
        /// Returns true if the choreographer is associated with main UI thread.
        /// </summary>
        /// <returns>true for the main choreographer</returns>
        public bool IsMainChoreographer()
        {
            return this == s_instance;
        }

#if WINDOWS_UWP
        /// <summary>
        /// Factory for choreographer instances associated with non main-view dispatchers.
        /// </summary>
        public static IReactChoreographer CreateSecondaryInstance(CoreApplicationView view)
        {
            return new ReactChoreographer(view);
        }
#endif

        private bool HasCoreWindow
        {
            get
            {
#if WINDOWS_UWP
                return _applicationView.CoreWindow != null;
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
                var isDisposed = Volatile.Read(ref _isDisposed);
                subscribe = _isSubscribing =
                    !isDisposed
                    && _callbackKeys.Add(callbackKey)
                    && _callbackKeys.Count == 1
                    && !isSubscribed
                    && !isSubscribing;
            }

            if (subscribe)
            {
                DispatcherHelpers.RunOnDispatcher(
#if WINDOWS_UWP
                    _coreDispatcher,
#else
                    DispatcherHelpers.MainDispatcher,
#endif
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
            _isDisposed = true;
            if (_isSubscribed)
            {
                Unsubscribe();
            }
#if WINDOWS_UWP
            s_threadInstanceMap.TryRemove(_coreDispatcher, out _);
#endif
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
            DispatcherHelpers.RunOnDispatcher(
#if WINDOWS_UWP
                _coreDispatcher,
#else
                DispatcherHelpers.MainDispatcher,
#endif
                () =>
                {
                    bool isSubscribed;
                    lock (_gate)
                    {
                        isSubscribed = _isSubscribed;
                    }

                    if (isSubscribed)
                    {
                        OnRendering(null, _stopwatch.Elapsed);
                    }
                });
        }

        private void OnRendering(object sender, TimeSpan e)
        {
            if (_frameEventArgs == null)
            {
                _mutableReference = _frameEventArgs = new FrameEventArgs(e);
            }
            else
            {
                _mutableReference.Update(e);
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
