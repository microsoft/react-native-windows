#if WINDOWS_UWP
using Windows.ApplicationModel.Resources.Core;
using Windows.Storage;
#else
using System.Configuration;
using System.Globalization;
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
                var result = default(bool);
                var parsed = bool.TryParse(ConfigurationManager.AppSettings[AllowRTL], out result);
                return (bool)(object)(parsed && result);
#endif
            }
            set
            {
#if WINDOWS_UWP
                ApplicationData.Current.LocalSettings.Values[AllowRTL] = value;
#else
                ConfigurationManager.AppSettings[AllowRTL] = value.ToString();
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
                var result = default(bool);
                var parsed = bool.TryParse(ConfigurationManager.AppSettings[ForceRTL], out result);
                return (bool)(object)(parsed && result);
#endif
            }
            set
            {
#if WINDOWS_UWP
                ApplicationData.Current.LocalSettings.Values[ForceRTL] = value;
#else
                ConfigurationManager.AppSettings[AllowRTL] = value.ToString();
#endif
            }
        }

        private static bool IsDeviceRightToLeft
        {
            get
            {
#if WINDOWS_UWP
                return ResourceContext.GetForCurrentView().QualifierValues["LayoutDirection"] == "RTL";
#else
                return CultureInfo.CurrentCulture.TextInfo.IsRightToLeft;
#endif
            }
        }
    }
}
