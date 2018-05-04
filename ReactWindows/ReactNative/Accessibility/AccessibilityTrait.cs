// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace ReactNative.Accessibility
{
    /// <summary>
    /// Accessibility traits supported by React Native Windows.
    /// </summary>
    public enum AccessibilityTrait
    {
        /// <summary>
        /// Corresponds to Invoke UI Automation pattern: https://msdn.microsoft.com/en-us/library/windows/desktop/ee671279.aspx
        /// </summary>
        Button,
        /// <summary>
        /// Denotes item in a ListView.
        /// ListItem Control Type properties/patterns/events as described here: https://msdn.microsoft.com/en-us/library/windows/desktop/ee671635(v=vs.85).aspx
        /// TODO Consider implementing SelectionItem Control Pattern.
        /// </summary>
        ListItem,
    }
}
