// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using ReactNative.UIManager;

namespace ReactNative.Views.Modal
{
    /// <summary>
    /// The shadow node implementation for Picker views.
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
    }
}
