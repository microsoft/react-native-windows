// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Common;
using ReactNative.Modules.Core;
using ReactNative.Tracing;

namespace ReactNative.Modules.AppState
{
    /// <summary>
    /// Native module for monitoring the state of the application.
    /// </summary>
    public class AppStateModule : ReactContextNativeModuleBase, IBackgroundEventListener
    {
        private const string AppStateActive = "active";
        private const string AppStateBackground = "background";

        // UWP apps are always activated/launched into the background. Afterwards, they
        // may receive a `LeavingBackground` event. This information is described in
        // https://docs.microsoft.com/en-us/windows/uwp/launch-resume/app-lifecycle#app-execution-state.
        // Note that React Native Windows's `LeavingBackground` event behaves similarly even on versions
        // of Windows that don't support `LeavingBackground/EnteredBackground` (i.e. Windows.Foundation.UniversalApiContract v3).
        // On such Windows versions, React Native Windows always fires `LeavingBackground` immediately after
        // `Resume` and `EnteredBackground` immediately before `Suspend`.
        private string _appState = AppStateBackground;

        /// <summary>
        /// Instantiates the <see cref="AppStateModule"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        public AppStateModule(ReactContext reactContext)
            : base(reactContext)
        {
        }

        /// <summary>
        /// The name of the module.
        /// </summary>
        public override string Name
        {
            get
            {
                return "AppState";
            }
        }

        /// <summary>
        /// The constants exported by this module.
        /// </summary>
        public override JObject ModuleConstants
        {
            get
            {
                var currentAppState = _appState;
                RnLog.Info(ReactConstants.RNW, $"AppStateModule: initial appState is {currentAppState}");
                return new JObject
                {
                    { "initialAppState", currentAppState }
                };
            }
        }

        /// <summary>
        /// Initializes the module.
        /// </summary>
        public override void Initialize()
        {
            Context.AddBackgroundEventListener(this);
        }

        /// <summary>
        /// Called when the host entered background mode.
        /// </summary>
        public void OnEnteredBackground()
        {
            _appState = AppStateBackground;
            SendAppStateChangeEvent();
        }

        /// <summary>
        /// Called when the host is leaving background mode.
        /// </summary>
        public void OnLeavingBackground()
        {
            _appState = AppStateActive;
            SendAppStateChangeEvent();
        }

        /// <summary>
        /// Called when the application host is shutdown.
        /// </summary>
        public void OnDestroy()
        {
        }

        /// <summary>
        /// Native method to retrieve the current application state.
        /// </summary>
        /// <param name="success">The success callback.</param>
        /// <param name="error">The error callback.</param>
        [ReactMethod]
        public void getCurrentAppState(ICallback success, ICallback error)
        {
            var currentAppState = _appState;
            RnLog.Info(ReactConstants.RNW, $"AppStateModule: getCurrentAppState returned {currentAppState}");
            success.Invoke(CreateAppStateEventMap(currentAppState));
        }

        private JObject CreateAppStateEventMap(string currentAppState)
        {
            return new JObject
            {
                { "app_state", currentAppState },
            };
        }

        private void SendAppStateChangeEvent()
        {
            var currentAppState = _appState;
            RnLog.Info(ReactConstants.RNW, $"AppStateModule: appStateDidChange to {currentAppState}");
            Context.GetJavaScriptModule<RCTDeviceEventEmitter>()
                .emit("appStateDidChange", CreateAppStateEventMap(currentAppState));
        }
    }
}
