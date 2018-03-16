// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

namespace ReactNative.Bridge
{
    /// <summary>
    /// Listener for application life cycle events.
    /// </summary>
    public interface ILifecycleEventListener
    {
        /// <summary>
        /// Called when the host receives the suspend event.
        /// </summary>
        void OnSuspend();

        /// <summary>
        /// Called when the host receives the resume event.
        /// </summary>
        void OnResume();

        /// <summary>
        /// Called when the host is shutting down.
        /// </summary>
        void OnDestroy();
    }
}
