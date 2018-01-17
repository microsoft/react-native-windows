using ReactNative.Bridge;
using ReactNative.Modules.Core;
using ReactNative.UIManager;
using System.Collections.Generic;
using System.Windows;

namespace ReactNative.Modules.DeviceInfo
{
    /// <summary>
    /// Native module that manages window dimension updates to JavaScript.
    /// </summary>
    class DeviceInfoModule : ReactContextNativeModuleBase, ILifecycleEventListener
    {
        private readonly Window _window;
        private readonly IReadOnlyDictionary<string, object> _constants;

        /// <summary>
        /// Instantiates the <see cref="DeviceInfoModule"/>. 
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        public DeviceInfoModule(ReactContext reactContext)
            : base(reactContext)
        {
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
        /// Called after the creation of a <see cref="IReactInstance"/>,
        /// </summary>
        public override void Initialize()
        {
            Context.AddLifecycleEventListener(this);
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
            Context.GetJavaScriptModule<RCTDeviceEventEmitter>()
                .emit("didUpdateDimensions", GetDimensions());
        }

        private IDictionary<string, object> GetDimensions()
        {
            return GetDimensions(DisplayMetrics.GetForCurrentView());
        }

        private IDictionary<string, object> GetDimensions(DisplayMetrics displayMetrics)
        {
            return new Dictionary<string, object>
            {
                {
                    "window",
                    new Dictionary<string, object>
                    {
                        { "width", displayMetrics.Width },
                        { "height", displayMetrics.Height },
                        { "scale", displayMetrics.Scale },
                        /* TODO: density and DPI needed? */
                    }
                },
            };
        }
    }
}
