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
        /// Platform decides if the view and/or its children are visible to screen reader.
        /// </summary>
        Auto = 0,

        /// <summary>
        /// This view is visible to screen reader.
        /// All children of this view are NOT visible to screen reader.
        /// If the accessibilityLabel is specified, it is used by screen reader; 
        /// otherwise, a text generated from the children using algorithm described 
        /// at https://www.w3.org/TR/accname-1.1/#mapping_additional_nd_te is used by screen reader.
        /// </summary>
        Yes,

        /// <summary>
        /// This view is NOT visible to screen reader.
        /// Decision on visibility of children to screen reader is deferred to the children.
        /// </summary>
        No,

        /// <summary>
        /// This view is NOT visible to screen reader.
        /// All children of this view are NOT visible to screen reader.
        /// </summary>
        NoHideDescendants,

        /// <summary>
        /// This view is visible to screen reader.
        /// Decision on visibility of children to screen reader is deferred to the children.
        /// If the accessibilityLabel is specified, it is used by screen reader; 
        /// otherwise, a text generated from the children using algorithm described 
        /// at https://www.w3.org/TR/accname-1.1/#mapping_additional_nd_te is used by screen reader.
        /// </summary>
        YesDontHideDescendants,
    }
}
