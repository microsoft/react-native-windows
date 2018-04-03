// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using System;
#if WINDOWS_UWP
using Windows.System;
#else
using System.Windows.Input;
#endif

namespace ReactNative.UIManager.Events
{
#if WINDOWS_UWP
    using Key = VirtualKey;
#endif

    static class KeyHelpers
    {
        public static int GetKeyCode(this Key key)
        {
            return (int)key;
        }

        public static JObject GetKeyConstants()
        {
            var keys = new JObject();
            foreach (var key in Enum.GetNames(typeof(Key)))
            {
                keys.Add(key, (int)Enum.Parse(typeof(Key), key));
            }

            return keys;
        }
    }
}
