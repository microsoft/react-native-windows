// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

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
                    { "minor", 0 },
                    { "patch", 0 },
                    { "prerelease", null }
                };
            }
        }
    }
}
