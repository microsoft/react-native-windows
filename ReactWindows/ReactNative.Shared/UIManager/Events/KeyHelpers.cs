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
            return Enum.GetValues(typeof(Key))
                .OfType<Key>()
                .Distinct() // Distinct() required*
                .ToDictionary(
                    key => key.ToString(),
                    key => GetKeyCode(key));
            // *The following keys are duplicated in Enum.GetValues(Type):
            // `Hangul`, `Kanji`
        }
    }
}
