// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace ReactNative.UIManager
{
    /// <summary>
    /// Shadow node subclass for WindowsControl.
    /// </summary>
    public class ReactControlShadowNode : LayoutShadowNode
    {
        /// <summary>
        /// Instantiates a <see cref="ReactControlShadowNode"/>.
        /// </summary>
        public ReactControlShadowNode() :
            base(false, true)
        {
        }
    }
}
