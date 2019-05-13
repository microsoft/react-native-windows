// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

namespace ReactNative.UIManager.LayoutAnimation
{
    /// <summary>
    /// Layout animation for deleted views.
    /// </summary>
    class LayoutDeleteAnimation : BaseLayoutAnimation
    {
        /// <summary>
        /// Signals if the animation should be performed in reverse.
        /// </summary>
        protected override bool IsReverse
        {
            get
            {
                return true;
            }
        }
    }
}
