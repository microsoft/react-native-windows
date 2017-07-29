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

        private static bool? _isRightToLeftAllowed;
        private static bool? _isRightToLeftForced;
        private static bool? _isDeviceLeftToRight;

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
                if (!_isRightToLeftAllowed.HasValue)
                {
                    _isRightToLeftAllowed =
#if WINDOWS_UWP
                        (bool?)ApplicationData.Current.LocalSettings.Values[AllowRTL] ?? false;
#else
                        GetSetting(AllowRTL, false);
#endif
                }

                return _isRightToLeftAllowed.Value;
            }
            set
            {
                _isRightToLeftAllowed = value;
                SetSetting(AllowRTL, value);
            }
        }

        /// <summary>
        /// Gets or sets whether right to left is forced, used for development purposes.
        /// </summary>
        public static bool IsRightToLeftForced
        {
            get
            {
                if (!_isRightToLeftForced.HasValue)
                {
                    _isRightToLeftForced =
#if WINDOWS_UWP
                        (bool?)ApplicationData.Current.LocalSettings.Values[ForceRTL] ?? false;
#else
                        GetSetting(ForceRTL, false);
#endif
                }

                return _isRightToLeftForced.Value;
            }
            set
            {
                _isRightToLeftForced = value;
                SetSetting(ForceRTL, value);
            }
        }

        private static bool IsDeviceRightToLeft
        {
            get
            {
                if (!_isDeviceLeftToRight.HasValue)
                {
                    _isDeviceLeftToRight =
#if WINDOWS_UWP
                        ResourceContext.GetForCurrentView().QualifierValues["LayoutDirection"] == "RTL";
#else
                        CultureInfo.CurrentCulture.TextInfo.IsRightToLeft;
#endif
                }

                return _isDeviceLeftToRight.Value;
            }
        }

#if !WINDOWS_UWP
        private static T GetSetting<T>(string key, T defaultValue)
        {
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
        }
#endif

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
