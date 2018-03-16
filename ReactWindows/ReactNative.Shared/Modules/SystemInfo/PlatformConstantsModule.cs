// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using ReactNative.Bridge;
using System.Collections.Generic;

namespace ReactNative.Modules.SystemInfo
{
    /// <summary>
    /// Module that exposes platform constants to JavaScript.
    /// </summary>
    public class PlatformConstantsModule : NativeModuleBase
    {
        /// <summary>
        /// The native module name.
        /// </summary>
        public override string Name
        {
            get
            {
                return "PlatformConstants";
            }
        }

        /// <summary>
        /// The native module constants.
        /// </summary>
        public override IReadOnlyDictionary<string, object> Constants
        {
            get
            {
                return new Dictionary<string, object>
                {
                    { "reactNativeVersion", ReactNativeVersion.Version },
                };
            }
        }
    }
}
