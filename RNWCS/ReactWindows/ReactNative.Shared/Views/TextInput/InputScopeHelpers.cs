// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#if WINDOWS_UWP
using Windows.UI.Xaml.Input;
#else
using System.Windows.Input;
#endif

namespace ReactNative.Views.TextInput
{
    static class InputScopeHelpers
    {
        public static InputScopeNameValue FromString(string inputScope)
        {
            switch (inputScope)
            {
                case "url":
                    return InputScopeNameValue.Url;
                case "number-pad":
#if WINDOWS_UWP
                    return InputScopeNameValue.NumericPin;
#else
                    return InputScopeNameValue.Number;
#endif
                case "phone-pad":
                    return InputScopeNameValue.TelephoneNumber;
                case "name-phone-pad":
#if WINDOWS_UWP
                    return InputScopeNameValue.NameOrPhoneNumber;
#else
                    return InputScopeNameValue.PersonalFullName;
#endif
                case "email-address":
#if WINDOWS_UWP
                    return InputScopeNameValue.EmailNameOrAddress;
#else
                    return InputScopeNameValue.EmailUserName;
#endif
                case "decimal-pad":
                    return InputScopeNameValue.Digits;
                case "web-search":
#if WINDOWS_UWP
                    return InputScopeNameValue.Search;
#else
                    return InputScopeNameValue.AlphanumericFullWidth;
#endif
                case "numeric":
                    return InputScopeNameValue.Number;
                default:
                    return InputScopeNameValue.Default;
            }
        }

        public static InputScopeNameValue FromStringForPasswordBox(string inputScope)
        {
            switch (inputScope)
            {
                case "number-pad":
#if WINDOWS_UWP
                    return InputScopeNameValue.NumericPin;
#else
                    return InputScopeNameValue.Number;
#endif
                default:
                    return InputScopeNameValue.Password;
            }
        }
    }
}
