// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace ReactNative.Modules.Core
{
    /// <summary>
    /// Event arguments for frame callbacks.
    /// </summary>
    public class FrameEventArgs : IMutableFrameEventArgs
    {
        private static readonly TimeSpan s_frameDuration = TimeSpan.FromTicks(166666);

        private readonly TimeSpan _initialRenderingTime;
        private DateTimeOffset _initialAbsoluteTime;

        internal FrameEventArgs(TimeSpan renderingTime)
        {
            RenderingTime = _initialRenderingTime = renderingTime;
            FrameTime = _initialAbsoluteTime = DateTimeOffset.UtcNow;
        }

        /// <summary>
        /// The relative frame time.
        /// </summary>
        public TimeSpan RenderingTime
        {
            get;
            private set;
        }

        /// <summary>
        /// The absolute frame time.
        /// </summary>
        public DateTimeOffset FrameTime
        {
            get;
            private set;
        }

        void IMutableFrameEventArgs.Update(TimeSpan renderingTime)
        {
            RenderingTime = renderingTime;
            FrameTime = _initialAbsoluteTime + renderingTime - _initialRenderingTime;

            // Self-adjust initial absolute time to better approximate CompositionTarget frame boundary
            _initialAbsoluteTime -= FrameTime - DateTimeOffset.UtcNow - s_frameDuration;
        }
    }
}
