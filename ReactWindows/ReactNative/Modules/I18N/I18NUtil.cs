using Windows.ApplicationModel.Resources.Core;
using Windows.Storage;

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
                return (bool?)ApplicationData.Current.LocalSettings.Values[AllowRTL] ?? false;
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
                return (bool?)ApplicationData.Current.LocalSettings.Values[ForceRTL] ?? false;
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
                return ResourceContext.GetForCurrentView().QualifierValues["LayoutDirection"] == "RTL";
            }
        }
    }
}
