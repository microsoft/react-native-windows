// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Linq;
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

        public static IReadOnlyDictionary<string, int> GetKeyConstants()
        {
            return Enum.GetNames(typeof(Key))
                .Cast<string>()
                .ToDictionary(
                    key => key,
                    key => (int)Enum.Parse(typeof(Key), key));
        }
    }
}
