using ReactNative.Bridge;
using System.Collections.Generic;
using System.Globalization;

namespace ReactNative.Modules.Internationalization
{
    /// <summary>
    /// A module that allows JS to get/set right to left preferences.
    /// </summary>
    class InternationalizationModule : NativeModuleBase
    {
        private const string ModuleName = "I18nManager";
        private const string IsRtl = "isRTL";
        private const string LocalIdentifier = "localeIdentifier";


        internal InternationalizationModule()
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

        public override IReadOnlyDictionary<string, object> Constants
        {
            get
            {
                return new Dictionary<string, object>
                {
                    { IsRtl, InternationalizationUtil.IsRightToLeft },
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
            InternationalizationUtil.IsRightToLeftAllowed = value;
        }

        /// <summary>
        /// Sets whether to force right to left.  This is used for development purposes to force a language such as English to be RTL.
        /// </summary>
        /// <param name="value">true to force right to left; else false.</param>
        [ReactMethod]
        public void forceRTL(bool value)
        {
            InternationalizationUtil.IsRightToLeftForced = true;
        }
    }
}
