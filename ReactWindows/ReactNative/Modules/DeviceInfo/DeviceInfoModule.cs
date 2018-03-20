// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Modules.Core;
using ReactNative.UIManager;
using System.Collections.Generic;
using Windows.ApplicationModel.Core;
using Windows.Graphics.Display;
using Windows.UI.ViewManagement;

namespace ReactNative.Modules.DeviceInfo
{
    /// <summary>
    /// Native module that manages window dimension updates to JavaScript.
    /// </summary>
    class DeviceInfoModule : ReactContextNativeModuleBase, ILifecycleEventListener, IBackgroundEventListener
    {
        private readonly IReadOnlyDictionary<string, object> _constants;

        private bool _isSubscribed;

        /// <summary>
        /// Instantiates the <see cref="DeviceInfoModule"/>. 
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        public DeviceInfoModule(ReactContext reactContext)
            : base(reactContext)
        {
            var displayMetrics = HasCoreWindow
                ? DisplayMetrics.GetForCurrentView()
                : DisplayMetrics.Empty;

            _constants = new Dictionary<string, object>
            {
                { "Dimensions", GetDimensions(displayMetrics) },
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

        private static bool HasCoreWindow
        {
            get
            {
                return CoreApplication.MainView.CoreWindow != null;
            }
        }

        /// <summary>
        /// Called after the creation of a <see cref="IReactInstance"/>,
        /// </summary>
        public override void Initialize()
        {
            Context.AddLifecycleEventListener(this);
            Context.AddBackgroundEventListener(this);
        }

        /// <summary>
        /// Called when the application is suspended.
        /// </summary>
        public void OnSuspend()
        {
            Unsubscribe();
        }

        /// <summary>
        /// Called when the application is resumed.
        /// </summary>
        public void OnResume()
        {
            Subscribe();
        }

        /// <summary>
        /// Called when the host entered background mode.
        /// </summary>
        public void OnEnteredBackground()
        {
            Unsubscribe();
        }

        /// <summary>
        /// Called when the host is leaving background mode.
        /// </summary>
        public void OnLeavingBackground()
        {
            Subscribe();
        }

        private void Subscribe()
        {
            if (!_isSubscribed && HasCoreWindow)
            {
                _isSubscribed = true;
                ApplicationView.GetForCurrentView().VisibleBoundsChanged += OnVisibleBoundsChanged;
                DisplayInformation.GetForCurrentView().OrientationChanged += OnOrientationChanged;
                SendUpdateDimensionsEvent();
            }
        }

        private void Unsubscribe()
        {
            if (_isSubscribed && HasCoreWindow)
            {
                _isSubscribed = false;
                ApplicationView.GetForCurrentView().VisibleBoundsChanged -= OnVisibleBoundsChanged;
                DisplayInformation.GetForCurrentView().OrientationChanged -= OnOrientationChanged;
            }
        }

        /// <summary>
        /// Called when the application is terminated.
        /// </summary>
        public void OnDestroy()
        {
        }

        private void OnVisibleBoundsChanged(ApplicationView sender, object args)
        {
            SendUpdateDimensionsEvent();
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
                Context.GetJavaScriptModule<RCTDeviceEventEmitter>()
                    .emit("namedOrientationDidChange", new JObject
                    {
                        { "name", name },
                        { "rotationDegrees", degrees },
                        { "isLandscape", isLandscape },
                    });
            }
        }

        private void SendUpdateDimensionsEvent()
        {
            Context.GetJavaScriptModule<RCTDeviceEventEmitter>()
                .emit("didUpdateDimensions", GetDimensions());
        }

        private static IDictionary<string, object> GetDimensions()
        {
            return GetDimensions(DisplayMetrics.GetForCurrentView());
        }

        private static IDictionary<string, object> GetDimensions(DisplayMetrics displayMetrics)
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
