// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace ReactNative.Views.Scroll
{
    /// <summary>
    /// An interface of ScrollView for mock to do unit tests.
    /// Please add more methods or properties if more tests involved.
    /// </summary>
    public interface IScrollView
    {
        /// <summary>
        /// Scrolls the content within the ScrollView to the specified horizontal offset position.
        /// </summary>
        /// <param name="offset">The position that the content scrolls to.</param>
        void ScrollToHorizontalOffset(double offset);

        /// <summary>
        /// Scrolls the content within the ScrollView to the specified vertical offset position.
        /// </summary>
        /// <param name="offset">The position that the content scrolls to.</param>
        void ScrollToVerticalOffset(double offset);

        /// <summary>
        /// Gets a value that contains the horizontal offset of the scrolled content.
        /// </summary>
        double HorizontalOffset { get; }

        /// <summary>
        /// Gets a value that contains the vertical offset of the scrolled content.
        /// </summary>
        double VerticalOffset { get; }
    }
}
