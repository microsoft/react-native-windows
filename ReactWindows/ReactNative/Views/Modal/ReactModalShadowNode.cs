// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using Windows.UI.ViewManagement;
using ReactNative.UIManager;

namespace ReactNative.Views.Modal
{
    /// <summary>
    /// The shadow node implementation for modal views.
    /// </summary>
    public class ReactModalShadowNode : LayoutShadowNode
    {
        /// <summary>
        /// Instantiates the <see cref="ReactModalShadowNode"/>.
        /// </summary>
        public ReactModalShadowNode()
            : base(false, true)
        {
        }

        /// <summary>
        /// Insert a child at the given index.
        /// </summary>
        /// <param name="child">The child.</param>
        /// <param name="index">The index.</param>
        public override void AddChildAt(ReactShadowNode child, int index)
        {
            base.AddChildAt(child, index);

            // Set both the width & height to the longest dimension, this lets the modal
            // fill the screen after a device rotation
            var screenBounds = ApplicationView.GetForCurrentView().VisibleBounds;
            var longestDimension = Math.Max(screenBounds.Width, screenBounds.Height);
            child.StyleHeight = (float)longestDimension;
            child.StyleWidth = (float)longestDimension;
        }
    }
}
