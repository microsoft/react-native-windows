// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Modules.Core;
using ReactNative.UIManager;
using System.Collections.Concurrent;
using System.Linq;
using Windows.ApplicationModel.Core;
using Windows.Graphics.Display;
using Windows.UI.ViewManagement;

namespace ReactNative.Modules.DeviceInfo
{
    /// <summary>
    /// Native module that manages window dimension updates to JavaScript (with support for multi window application).
    /// </summary>
    /// <remarks>
    /// Dimensions are tracked for each application window right after creation of its ReactRootView.
    /// After any change in dimensions of any tracked window whole list of dimensions is emited and dimensions are available by 3 kinds of ids:
    ///
    ///  "window" - dimensions of main window
    ///  "{rootViewTag}" - dimensions of window with specified rootViewTag (each view has assigned one)
    ///  "{rootViewId}" - dimensions of window initialized with unique value of "reactxp_rootViewId" property
    ///
    /// <example>
    ///  {
    ///     "window": { /* main window dimensions */ },
    ///     "1": { /* alt-window-dimensions */ },
    ///     "41": { /* alt-window dimensions */ },
    ///     "03391df9-750d-465c-b0b0-66ee9dcf4a86": { /* alt-window dimensions */ }
    ///  }
    /// </example>
    /// </remarks>
    class DeviceInfoModule : ReactContextNativeModuleBase
    {
        private readonly JObject _constants;

        private readonly ConcurrentDictionary<ApplicationView, DeviceViewInfo> _registeredViews = new ConcurrentDictionary<ApplicationView, DeviceViewInfo>();

        /// <summary>
        /// Instantiates the <see cref="DeviceInfoModule"/>. 
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        public DeviceInfoModule(ReactContext reactContext)
            : base(reactContext)
        {
            _constants = new JObject
            {
                { "Dimensions", GetAllDimensions() },
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
        public override JObject ModuleConstants
        {
            get
            {
                return _constants;
            }
        }

        /// <summary>
        /// Register <paramref name="rootView"/> to keep track of his dimensions
        /// </summary>
        /// <param name="rootView">The react root view</param>
        /// <param name="tag">The react root view tag</param>
        public void RegisterRootView(ReactRootView rootView, int tag)
        {
            DispatcherHelpers.AssertOnDispatcher(rootView);

            var appView = ApplicationView.GetForCurrentView();

            _registeredViews.AddOrUpdate(appView, (v) =>
            {
                // Register new view info and hook up for events
                var displayInformation = DisplayInformation.GetForCurrentView();

                var info = new DeviceViewInfo(appView, rootView, displayInformation, tag);

                appView.VisibleBoundsChanged += OnVisibleBoundsChanged;
                appView.Consolidated += AppViewOnConsolidated;
                displayInformation.OrientationChanged += OnOrientationChanged;

                return info;
            },
                (view, info) =>
                {
                    // View is already registered, just update tag in case view is being reregistered
                    info.RootViewTag = tag;
                    return info;
                });

            SendUpdateDimensionsEvent();
        }

        /// <summary>
        /// Removes <paramref name="rootView"/> from registration, stop keeping track of his dimensions
        /// </summary>
        /// <param name="rootView">The react root view</param>
        public void RemoveRootView(ReactRootView rootView)
        {
            var info = _registeredViews.Values.SingleOrDefault(i => i.RootView == rootView);

            if (info != null)
            {
                CleanupViewInfo(info.ApplicationView);
            }
        }

        public void OnVisibleBoundsChanged(ApplicationView sender, object args)
        {
            if (_registeredViews.ContainsKey(sender))
            {
                _registeredViews[sender].UpdateDisplayMetrics();
            }

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
                .emit("didUpdateDimensions", GetAllDimensions());
        }

        private JObject GetAllDimensions()
        {
            var dimensions = new JObject();

            // Default metric for main window
            // TODO It would make sense to make default actively focused window and not the main in the future
            var mainView = _registeredViews.Values.FirstOrDefault(v => v.RootView.Dispatcher == CoreApplication.MainView.Dispatcher);
            var defaultMetric = mainView == null ? DisplayMetrics.Empty : mainView.CurrentDisplayMetrics;
            dimensions.Add("window", GetDimensions(defaultMetric));

            foreach (var info in _registeredViews.Values)
            {
                dimensions.Add($"{info.RootViewTag}", GetDimensions(info.CurrentDisplayMetrics));

                if (info.RootViewId != null)
                {
                    dimensions.Add($"{info.RootViewId}", GetDimensions(info.CurrentDisplayMetrics));
                }
            }

            return dimensions;
        }

        private static JObject GetDimensions(DisplayMetrics displayMetrics)
        {
            return new JObject
            {
                { "width", displayMetrics.Width },
                { "height", displayMetrics.Height },
                { "scale", displayMetrics.Scale },
                /* TODO: density and DPI needed? */
            };
        }

        private void AppViewOnConsolidated(ApplicationView sender, ApplicationViewConsolidatedEventArgs args)
        {
            CleanupViewInfo(sender);
        }

        private void CleanupViewInfo(ApplicationView appView)
        {
            DeviceViewInfo info = null;

            if (_registeredViews.TryRemove(appView, out info))
            {
                DispatcherHelpers.CallOnDispatcher(info.RootView.Dispatcher, () =>
                {
                    info.ApplicationView.VisibleBoundsChanged -= OnVisibleBoundsChanged;
                    info.ApplicationView.Consolidated -= AppViewOnConsolidated;
                    info.DisplayInformation.OrientationChanged -= OnOrientationChanged;

                    return true;
                }, true).Wait();
            }
        }
    }
}
