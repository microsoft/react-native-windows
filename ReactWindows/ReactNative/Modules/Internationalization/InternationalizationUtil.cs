using System.Globalization;
using Windows.Storage;

namespace ReactNative.Modules.Internationalization
{
    /// <summary>
    /// Class used for right to left preferences.
    /// </summary>
    static class InternationalizationUtil
    {
        private const string AllowRTL = "RCTI18nUtil_allowRTL";
        private const string ForceRTL = "RCTI18nUtil_forceRTL";

        static InternationalizationUtil()
        {
            if (ApplicationData.Current.LocalSettings.Values[AllowRTL] == null)
                ApplicationData.Current.LocalSettings.Values[AllowRTL] = false;

            if (ApplicationData.Current.LocalSettings.Values[ForceRTL] == null)
                ApplicationData.Current.LocalSettings.Values[ForceRTL] = false;
        }

        /// <summary>
        /// Check if the system is using Right to Left. This only happens when the app:
        /// - is forcing RTL layout regardless of language
        /// - allows RTL layout when using RTL locale.
        /// </summary>
        public static bool IsRightToLeft
        {
            get
            {
                if (IsRightToLeftForced) return true;
                return IsRightToLeftAllowed && IsDeviceRightToLeft;
            }
        }

        /// <summary>
        /// Gets or sets whether right to left is allowed.  Should be used very early during startup before the bridge is initialized.
        /// </summary>
        public static bool IsRightToLeftAllowed
        {
            get
            {
                return (bool)ApplicationData.Current.LocalSettings.Values[AllowRTL];
            }
            set
            {
                ApplicationData.Current.LocalSettings.Values[AllowRTL] = value;
            }
        }

        /// <summary>
        /// Gets or sets whether right to left is forced, used for development purposes.
        /// </summary>
        public static bool IsRightToLeftForced
        {
            get
            {
                return (bool)ApplicationData.Current.LocalSettings.Values[ForceRTL];
            }
            set
            {
                ApplicationData.Current.LocalSettings.Values[ForceRTL] = value;
            }
        }

        private static bool IsDeviceRightToLeft
        {
            get
            {
                return CultureInfo.CurrentCulture.TextInfo.IsRightToLeft;
            }
        }
    }
}
