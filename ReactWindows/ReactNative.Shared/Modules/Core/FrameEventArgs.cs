using System;

namespace ReactNative.Modules.Core
{
    /// <summary>
    /// Event arguments for frame callbacks.
    /// </summary>
    public class FrameEventArgs : IMutableFrameEventArgs
    {
        private readonly TimeSpan _initialRenderingTime;
        private readonly DateTimeOffset _initialAbsoluteTime;

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
        }
    }
}
