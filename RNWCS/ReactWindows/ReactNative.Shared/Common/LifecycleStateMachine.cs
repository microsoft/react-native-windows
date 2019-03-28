// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using ReactNative.Bridge;
#if WINDOWS_UWP
using Windows.Foundation.Metadata;
#endif

namespace ReactNative.Common
{
    internal class LifecycleStateMachine
    {
#if WINDOWS_UWP
        private static bool s_isBackroundModeSupported = ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 3);
#else
        private static bool s_isBackroundModeSupported = false;
#endif
        private readonly object _lifecycleStateLock = new object();
        private LifecycleState _lifecycleState;
        private ReactContext _currentReactContext;

        public LifecycleStateMachine(LifecycleState initialLifecycleState)
        {
            _lifecycleState = initialLifecycleState;
        }

        public void SetContext(ReactContext _reactContext)
        {
            lock (_lifecycleStateLock)
            {
                if (_currentReactContext != _reactContext)
                {
                    if (_currentReactContext != null)
                    {
                        // Suspend old context if needed. We don't touch the current lifecycle state.
                        if (_lifecycleState == LifecycleState.Foreground ||
                              _lifecycleState == LifecycleState.Background)
                        {
                            if (_lifecycleState == LifecycleState.Foreground)
                            {
                                _currentReactContext.OnEnteredBackground();
                            }
                            _currentReactContext.OnSuspend();
                        }
                    }

                    _currentReactContext = _reactContext;

                    if (_currentReactContext != null)
                    {
                        // Bring new context in sync with current lifecycle state
                        if (_lifecycleState == LifecycleState.Foreground ||
                            _lifecycleState == LifecycleState.Background)
                        {
                            _currentReactContext.OnResume();

                            if (_lifecycleState == LifecycleState.Foreground)
                            {
                                _currentReactContext.OnLeavingBackground();
                            }
                        }
                   }
                }
            }
        }

        public void OnSuspend()
        {
            MoveToBeforeResumeLifecycleState();
        }

        public void OnEnteredBackground()
        {
            MoveToBackgroundLifecycleState();
        }

        public void OnLeavingBackground()
        {
            MoveToForegroundLifecycleState();
        }
        public void OnResume()
        {
            // An application is resumed in "background" mode.
            MoveToBackgroundLifecycleState();

            if (!s_isBackroundModeSupported)
            {
                // Simulate leaving background if OS doesn't support the LeavingBackground event
                MoveToForegroundLifecycleState();
            }
        }

        public void OnDestroy()
        {
            MoveToBeforeCreateLifecycleState();
        }

        private void MoveToBeforeResumeLifecycleState()
        {
            lock (_lifecycleStateLock)
            {
                if (_currentReactContext != null)
                {
                    if (_lifecycleState == LifecycleState.BeforeCreate)
                    {
                        // Starting in suspended mode, do a resume/suspend pulse without exiting background
                        _currentReactContext.OnResume();
                        _currentReactContext.OnSuspend();
                    }
                    else if (_lifecycleState == LifecycleState.Foreground)
                    {
                        // Shouldn't really happen, but I've seen it once. 
                        _currentReactContext.OnEnteredBackground();
                        _currentReactContext.OnSuspend();
                    }
                    else if (_lifecycleState == LifecycleState.Background)
                    {
                        _currentReactContext.OnSuspend();
                    }
                }

                _lifecycleState = LifecycleState.Suspended;
            }
        }

        private void MoveToForegroundLifecycleState()
        {
            lock (_lifecycleStateLock)
            {
                if (_currentReactContext != null)
                {
                    // We currently don't have an OnCreate callback so we call OnResume for both transitions
                    if (_lifecycleState == LifecycleState.Suspended ||
                        _lifecycleState == LifecycleState.BeforeCreate)
                    {
                        _currentReactContext.OnResume();
                        _currentReactContext.OnLeavingBackground();
                    }
                    else if (_lifecycleState == LifecycleState.Background)
                    {
                        _currentReactContext.OnLeavingBackground();
                    }
                }

                _lifecycleState = LifecycleState.Foreground;
            }
        }

        private void MoveToBeforeCreateLifecycleState()
        {
            lock (_lifecycleStateLock)
            {
                if (_currentReactContext != null)
                {
                    if (_lifecycleState == LifecycleState.Foreground)
                    {
                        _currentReactContext.OnEnteredBackground();
                        _lifecycleState = LifecycleState.Background;
                    }
                    if (_lifecycleState == LifecycleState.Background)
                    {
                        _currentReactContext.OnSuspend();
                        _lifecycleState = LifecycleState.Suspended;
                    }
                    if (_lifecycleState == LifecycleState.Suspended)
                    {
                        _currentReactContext.OnDestroy();
                    }
                }

                _lifecycleState = LifecycleState.BeforeCreate;
            }
        }

        private void MoveToBackgroundLifecycleState()
        {
            lock (_lifecycleStateLock)
            {
                if (_currentReactContext != null)
                {
                    if (_lifecycleState == LifecycleState.Foreground)
                    {
                        _currentReactContext.OnEnteredBackground();
                    }
                    else if (_lifecycleState == LifecycleState.Suspended ||
                             _lifecycleState == LifecycleState.BeforeCreate)
                    {
                        _currentReactContext.OnResume();
                    }
                }

                _lifecycleState = LifecycleState.Background;
            }
        }
    }
}
