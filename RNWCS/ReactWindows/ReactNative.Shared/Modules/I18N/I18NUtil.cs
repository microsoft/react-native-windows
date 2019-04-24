// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

#if WINDOWS_UWP
using Windows.ApplicationModel.Resources.Core;
using Windows.Storage;
#else
using System;
using System.Collections.Generic;
using System.Configuration;
using System.Globalization;
using static System.FormattableString;
#endif

namespace ReactNative.Modules.I18N
{
    /// <summary>
    /// Class used for right to left preferences.
    /// </summary>
    static class I18NUtil
    {
        private const string AllowRTL = "RCTI18nUtil_allowRTL";
        private const string ForceRTL = "RCTI18nUtil_forceRTL";

#if !WINDOWS_UWP
        private static IDictionary<string, object> _localSettings = new Dictionary<string, object>();
#endif

        /// <summary>
        /// Check if the system is using Right to Left. This only happens when the app:
        /// - is forcing RTL layout regardless of language
        /// - allows RTL layout when using RTL locale.
        /// </summary>
        public static bool IsRightToLeft
        {
            get
            {
                return IsRightToLeftForced || (IsRightToLeftAllowed && IsDeviceRightToLeft);
            }
        }

        /// <summary>
        /// Gets or sets whether right to left is allowed.  Should be used very early during startup before the bridge is initialized.
        /// </summary>
        public static bool IsRightToLeftAllowed
        {
            get
            {
#if WINDOWS_UWP
                return (bool?)ApplicationData.Current.LocalSettings.Values[AllowRTL] ?? false;
#else
                return GetSetting(AllowRTL, false);
#endif
            }
            set
            {
#if WINDOWS_UWP
                ApplicationData.Current.LocalSettings.Values[AllowRTL] = value;
#else
                SetSetting(AllowRTL, value);
#endif
            }
        }

        /// <summary>
        /// Gets or sets whether right to left is forced, used for development purposes.
        /// </summary>
        public static bool IsRightToLeftForced
        {
            get
            {
#if WINDOWS_UWP
                return (bool?)ApplicationData.Current.LocalSettings.Values[ForceRTL] ?? false;
#else
                return GetSetting(ForceRTL, false);
#endif
            }
            set
            {
                SetSetting(ForceRTL, value);
            }
        }

        private static bool IsDeviceRightToLeft
        {
            get
            {
#if WINDOWS_UWP
                // We use GetForViewIndependentUse because the customary GetForCurrentView throws exception when
                // an associated CoreWindow is not present, condition that happens during background activation scenarios.
                // GetForViewIndependentUse is good enough for the LayoutDirection resource (it's not view or display dependent)

                // More, sometimes the retrieval of the "LayoutDirection" values fails with "UnauthorizedAccessException"
                // because of a peculiarity of the QualifierValues on some OS'es (it keeps just a weak reference to the ResourceContext)
                // We mitigate by pinning ResourceContext locally
                var context = ResourceContext.GetForViewIndependentUse();
                return context.QualifierValues["LayoutDirection"] == "RTL";
#else
                return CultureInfo.CurrentCulture.TextInfo.IsRightToLeft;
#endif
            }
        }

        //TODO: Git Issue #878
        private static T GetSetting<T>(string key, T defaultValue)
        {
#if WINDOWS_UWP
            var values = ApplicationData.Current.LocalSettings.Values;
            if (values.ContainsKey(key))
            {
                var data = values[key];
                if (data is T)
                {
                    return (T)data;
                }
            }

            return defaultValue;
#else
            if (_localSettings.ContainsKey(key))
            {
                var data = _localSettings[key];

                if (data is T)
                {
                    return (T)data;
                }
                else
                {
                    throw new NotSupportedException(Invariant($"Configuration values of type '{typeof(T)}' are not supported."));
                }
            }

            return defaultValue;
#endif
        }

        private static void SetSetting<T>(string key, T value)
        {
#if WINDOWS_UWP
            var values = ApplicationData.Current.LocalSettings.Values;
            values[key] = value;
#else
            _localSettings[key] = value;
#endif
        }
    }
}
