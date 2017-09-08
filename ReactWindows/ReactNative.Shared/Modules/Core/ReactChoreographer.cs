using ReactNative.Bridge;
using System;
#if WINDOWS_UWP
using Windows.UI.Xaml.Media;
#else
using System.Windows.Media;
#endif

namespace ReactNative.Modules.Core
{
    /// <summary>
    /// A simple action queue that allows us to control the order certain
    /// callbacks are executed within a given frame.
    /// </summary>
    public class ReactChoreographer : IDisposable
    {
        private static ReactChoreographer s_instance;

        private FrameEventArgs _frameEventArgs;
        private IMutableFrameEventArgs _mutableReference;

        private ReactChoreographer()
        {
            CompositionTarget.Rendering += OnRendering;
        }

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

        void IDisposable.Dispose()
        {
            CompositionTarget.Rendering -= OnRendering;
        }

        private void OnRendering(object sender, object e)
        {
            var renderingArgs = e as RenderingEventArgs;
            if (renderingArgs == null)
            {
                throw new InvalidOperationException("Expected rendering event arguments.");
            }

            var renderingTime = renderingArgs.RenderingTime;
            if (_frameEventArgs == null)
            {
                _mutableReference = _frameEventArgs = new FrameEventArgs(renderingTime);
            }
            else
            {
                _mutableReference.Update(renderingTime);
            }

            DispatchUICallback?.Invoke(sender, _frameEventArgs);
            NativeAnimatedCallback?.Invoke(sender, _frameEventArgs);
            JavaScriptEventsCallback?.Invoke(sender, _frameEventArgs);
        }
    }
}
