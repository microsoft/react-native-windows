// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace ReactNative.Accessibility
{
    /// <summary>
    /// Accessibility importance mode.
    /// </summary>
    public enum ImportantForAccessibility
    {
        /// <summary>
        /// Platform decides which views are important for accessibility and brings
        /// the screen reader focus on those views
        /// </summary>
        Auto = 0,

        /// <summary>
        /// Groups all subviews under the view, allowing the screen reader to focus
        /// just this view; if the accessibilityLabel is specified, it is announced; 
        /// otherwise, the labels of its children are used
        /// </summary>
        Yes,

        /// <summary>
        /// Tells the screen reader that it can focus the subviews of this view
        /// </summary>
        No,

        /// <summary>
        /// Hides the view and its subviews from the screen reader
        /// </summary>
        NoHideDescendants,
    }
}
