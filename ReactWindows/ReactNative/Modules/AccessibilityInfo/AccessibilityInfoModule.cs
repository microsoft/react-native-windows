// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Modules.Core;
using System.Collections.Generic;
using Windows.ApplicationModel.Core;
using Windows.UI.ViewManagement;

namespace ReactNative.Modules.Accessibilityinfo
{
    class AccessibilityInfoModule : ReactContextNativeModuleBase, ILifecycleEventListener, IBackgroundEventListener
    {
        private readonly AccessibilitySettings _accessibility = new AccessibilitySettings();
        private readonly UISettings _settings = new UISettings();

        private bool _isSubscribed;

        private string GetRgbaString(UIElementType type)
        {
            var color = _settings.UIElementColor(type);
            return "rgba(" + color.R + "," + color.G + "," + color.B + "," + color.A + ")";
        }

        // TODO: These are the 8 high contrast colors in Windows. Eventually they will be exposed to JS.
        private IDictionary<string, string> GetHighContrastColors()
        {
            return new Dictionary<string, string>
            {
                {  "windowText", GetRgbaString(UIElementType.WindowText) },
                {  "hotlight", GetRgbaString(UIElementType.Hotlight) },
                {  "grayText", GetRgbaString(UIElementType.GrayText) },
                {  "highlightText", GetRgbaString(UIElementType.HighlightText) },
                {  "highlight", GetRgbaString(UIElementType.Highlight) },
                {  "buttonText", GetRgbaString(UIElementType.ButtonText) },
                {  "buttonFace", GetRgbaString(UIElementType.ButtonFace) },
                {  "window", GetRgbaString(UIElementType.Window) },
            };
        }

        public AccessibilityInfoModule(ReactContext reactContext)
            : base(reactContext)
        {

        }

        public override string Name => "AccessibilityInfo";

        public override JObject ModuleConstants
        {
            get
            {
                return new JObject
                {
                    // TODO: It would be better to have a sync GethIsHighContrast,
                    // but this is not supported by the framework at the moment.
                    { "initialHighContrast", _accessibility.HighContrast },
                };
            }
        }

        private static bool HasCoreWindow => CoreApplication.MainView.CoreWindow != null;

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
                _accessibility.HighContrastChanged += OnHighContrastChanged;
            }
        }

        private void Unsubscribe()
        {
            if (_isSubscribed && HasCoreWindow)
            {
                _accessibility.HighContrastChanged -= OnHighContrastChanged;
                _isSubscribed = false;
            }
        }

        /// <summary>
        /// Called when the application is terminated.
        /// </summary>
        public void OnDestroy()
        {
        }

        private void OnHighContrastChanged(AccessibilitySettings sender, object args)
        {
            Context.GetJavaScriptModule<RCTDeviceEventEmitter>()
                .emit("highContrastDidChange", sender.HighContrast);
        }

        [ReactMethod]
        public void fetchIsHighContrast(IPromise promise)
        {
            promise.Resolve(_accessibility.HighContrast);
        }
    }
}
