// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace ReactNative.Bridge
{
    /// <summary>
    /// Listener for application life cycle events.
    /// </summary>
    public interface IBackgroundEventListener
    {
        /// <summary>
        /// Called when the host entered background mode.
        /// </summary>
        void OnEnteredBackground();

        /// <summary>
        /// Called when the host is leaving background mode.
        /// </summary>
        void OnLeavingBackground();
    }
}
