// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Modules.Core;
using ReactNative.UIManager;
using System.Globalization;

namespace ReactNative.Modules.I18N
{
    /// <summary>
    /// A module that allows JS to get/set right to left preferences.
    /// </summary>
    class I18NModule : ReactContextNativeModuleBase
    {
        private const string ModuleName = "I18nManager";
        private const string IsRtl = "isRTL";
        private const string LocalIdentifier = "localeIdentifier";

        private bool _latestRightToLeft = false;

        /// <summary>
        /// Instantiates the <see cref="I18NModule"/>. 
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        public I18NModule(ReactContext reactContext)
            : base(reactContext)
        {
        }

        /// <summary>
        /// Gets the module name.
        /// </summary>
        public override string Name
        {
            get
            {
                return ModuleName;
            }
        }

        /// <summary>
        /// The constants exported by this module.
        /// </summary>
        public override JObject ModuleConstants
        {
            get
            {
                // The reading of the constants is a good initialization point for _latestRightToLeft
                _latestRightToLeft = I18NUtil.IsRightToLeft;
                return new JObject
                {
                    { IsRtl, _latestRightToLeft },
                    { LocalIdentifier, CultureInfo.CurrentCulture.Name }
                };
            }
        }

        /// <summary>
        /// Sets whether to allow right to left.
        /// </summary>
        /// <param name="value">true to allow right to left; else false.</param>
        [ReactMethod]
        public void allowRTL(bool value)
        {
            I18NUtil.IsRightToLeftAllowed = value;

            NotifyIfNeeded();
        }

        /// <summary>
        /// Sets whether to force right to left.  This is used for development purposes to force a language such as English to be RTL.
        /// </summary>
        /// <param name="value">true to force right to left; else false.</param>
        [ReactMethod]
        public void forceRTL(bool value)
        {
            I18NUtil.IsRightToLeftForced = value;

            NotifyIfNeeded();
        }

        /// <summary>
        /// Gets the current state of right to left consolidated value
        /// </summary>
        /// <param name="promise">Promise for the result.</param>
        [ReactMethod]
        public void isCurrentRTL(IPromise promise)
        {
            promise.Resolve(I18NUtil.IsRightToLeft);
        }

        private void NotifyIfNeeded()
        {
            var currentRightToLeft = I18NUtil.IsRightToLeft;
            if (currentRightToLeft != _latestRightToLeft)
            {
                _latestRightToLeft = currentRightToLeft;

                Context.GetJavaScriptModule<RCTDeviceEventEmitter>()
                    .emit("isRTLChanged", new JObject
                    {
                        { "isRTL", _latestRightToLeft },
                    });

                Context.GetNativeModule<UIManagerModule>().UpdateLayoutDirection();
            }
        }
    }
}
