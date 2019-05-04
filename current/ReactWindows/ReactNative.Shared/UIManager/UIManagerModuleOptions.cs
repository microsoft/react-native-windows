// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace ReactNative.UIManager
{
    /// <summary>
    /// Options for the <see cref="UIManagerModule"/>.
    /// </summary>
    [Flags]
    public enum UIManagerModuleOptions
    {
        /// <summary>
        /// None.
        /// </summary>
        None = 0,

        /// <summary>
        /// Load view manager constants on-demand.
        /// </summary>
        LazyViewManagers,
    }
}
