using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Modules.Core;
using System.Collections.Generic;
using Windows.Graphics.Display;
using Windows.UI.ViewManagement;

namespace ReactNative.Modules.DeviceInfo
{
    /// <summary>
    /// Native module that manages window dimension updates to JavaScript.
    /// </summary>
    public class DeviceInfoModule : NativeModuleBase, ILifecycleEventListener
    {
        private readonly ReactContext _reactContext;
        private readonly IReadOnlyDictionary<string, object> _constants;

        /// <summary>
        /// Instantiates the <see cref="DeviceInfoModule"/>. 
        /// </summary>
        /// <param name="context">The React context.</param>
        public DeviceInfoModule(ReactContext context)
        {
            _reactContext = context;
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
            get { return "DeviceInfo"; }    
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
            ApplicationView.GetForCurrentView().VisibleBoundsChanged -= OnBoundsChanged;
            DisplayInformation.GetForCurrentView().OrientationChanged -= OnOrientationChanged;
        }

        /// <summary>
        /// Called when the application is resumed.
        /// </summary>
        public void OnResume()
        {
            ApplicationView.GetForCurrentView().VisibleBoundsChanged += OnBoundsChanged;
            DisplayInformation.GetForCurrentView().OrientationChanged += OnOrientationChanged;
        }

        /// <summary>
        /// Called when the application is terminated.
        /// </summary>
        public void OnDestroy()
        {
        }

        private void OnBoundsChanged(ApplicationView sender, object args)
        {
            _reactContext.GetJavaScriptModule<RCTDeviceEventEmitter>()
                .emit("didUpdateDimensions", GetDimensions());
        }

        private void OnOrientationChanged(DisplayInformation displayInformation, object args)
        {
            var name = default(string);
            var degrees = default(double);
            var isLandscape = false;

            switch (displayInformation.CurrentOrientation)
            {
                case DisplayOrientations.Landscape:
                    name = "landscape-primary";
                    degrees = -90.0;
                    isLandscape = true;
                    break;
                case DisplayOrientations.Portrait:
                    name = "portrait-primary";
                    degrees = 0.0;
                    break;
                case DisplayOrientations.LandscapeFlipped:
                    name = "landscape-secondary";
                    degrees = 90.0;
                    isLandscape = true;
                    break;
                case DisplayOrientations.PortraitFlipped:
                    name = "portraitSecondary";
                    degrees = 180.0;
                    break;
            }

            if (name != null)
            {
                _reactContext.GetJavaScriptModule<RCTDeviceEventEmitter>()
                    .emit("namedOrientationDidChange", new JObject
                    {
                        { "name", name },
                        { "rotationDegrees", degrees },
                        { "isLandscape", isLandscape },
                    });
            }
        }

        private static IDictionary<string, object> GetDimensions()
        {
            var bounds = ApplicationView.GetForCurrentView().VisibleBounds;
            var scale = DisplayInformation.GetForCurrentView().RawPixelsPerViewPixel;

            return new Dictionary<string, object>
            {
                {
                    "window",
                    new Dictionary<string, object>
                    {
                        { "width", bounds.Width },
                        { "height", bounds.Height },
                        { "scale", scale },
                        /* TODO: density and DPI needed? */
                    }
                },
            };
        }
    }
}
