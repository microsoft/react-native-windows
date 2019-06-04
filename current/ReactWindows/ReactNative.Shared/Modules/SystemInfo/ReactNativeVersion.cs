using Newtonsoft.Json.Linq;

namespace ReactNative.Modules.SystemInfo
{
    /// <summary>
    /// Maintains the current NPM build version.
    /// </summary>
    public static class ReactNativeVersion
    {
        /// <summary>
        /// The React Native NPM build version.
        /// </summary>
        public static JObject Version
        {
            get
            {
                return new JObject
                {
                    { "major", 0 },
                    { "minor", 58 },
                    { "patch", 0 },
                    { "prerelease", "rc.1" },
                };
            }
        }
    }
}
