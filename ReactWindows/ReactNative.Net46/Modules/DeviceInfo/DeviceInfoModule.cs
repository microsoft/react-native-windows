using ReactNative.Bridge;
using ReactNative.Modules.Core;
using System.Collections.Generic;
using System.Windows;

namespace ReactNative.Modules.DeviceInfo
{
    /// <summary>
    /// Native module that manages window dimension updates to JavaScript.
    /// </summary>
    public class DeviceInfoModule : NativeModuleBase, ILifecycleEventListener
    {
        private readonly ReactContext _reactContext;
        private readonly Window _window;
        private readonly IReadOnlyDictionary<string, object> _constants;

        /// <summary>
        /// Instantiates the <see cref="DeviceInfoModule"/>. 
        /// </summary>
        /// <param name="context">The React context.</param>
        public DeviceInfoModule(ReactContext context)
        {
            _reactContext = context;
            _window = Application.Current.MainWindow;
            _constants = new Dictionary<string, object>
            {
                { "Dimensions", GetDimensions() },
            };
        }

        /// <summary>
        /// The name of the native module.
        /// </summary>
        public override string Name
        {
            get
            {
                return "DeviceInfo";
            }    
        }

        /// <summary>
        /// Native module constants.
        /// </summary>
        public override IReadOnlyDictionary<string, object> Constants
        {
            get
            {
                return _constants;
            }
        }

        /// <summary>
        /// Called when the application is suspended.
        /// </summary>
        public void OnSuspend()
        {
            _window.SizeChanged -= OnBoundsChanged;
        }

        /// <summary>
        /// Called when the application is resumed.
        /// </summary>
        public void OnResume()
        {
            _window.SizeChanged += OnBoundsChanged;
        }

        /// <summary>
        /// Called when the application is terminated.
        /// </summary>
        public void OnDestroy()
        {
            _window.SizeChanged -= OnBoundsChanged;
        }

        private void OnBoundsChanged(object sender, SizeChangedEventArgs args)
        {
            _reactContext.GetJavaScriptModule<RCTDeviceEventEmitter>()
                .emit("didUpdateDimensions", GetDimensions());
        }

        private IDictionary<string, object> GetDimensions()
        {
            var content = (FrameworkElement)_window.Content;
            double scale = 1.0;

            return new Dictionary<string, object>
            {
                {
                    "window",
                    new Dictionary<string, object>
                    {
                        { "width", content?.ActualWidth ?? 0.0 },
                        { "height", content?.ActualHeight ?? 0.0 },
                        { "scale", scale },
                        /* TODO: density and DPI needed? */
                    }
                },
            };
        }
    }
}
