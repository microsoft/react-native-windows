// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

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

            // Fixes a issue on Windows phone where rotating from horizontal to
            // vertical would leave a gap between the modal bottom and the screen edge.
            // This is set to the proper screen height when loading the modal in
            // ReactModalHostView.SetContentSize. The actual screen height cannot be
            // set here due to it being run by the layout manager thread which has no
            // active window and only mimics the main view bounds.
            child.StyleHeight = int.MaxValue;
        }
    }
}
