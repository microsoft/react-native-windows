// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace ReactNative.Modules.Image
{
    /// <summary>
    /// Image load status enum.
    /// </summary>
    /// <remarks>
    /// Keep in sync with public constants in <see cref="Views.Image.ReactImageLoadEvent"/>. 
    /// </remarks>
    enum ImageLoadStatus
    {
        /// <summary>
        /// Invalid status.
        /// </summary>
        None = 0,

        /// <summary>
        /// Image loading started status.
        /// </summary>
        OnLoadStart = 1,

        /// <summary>
        /// Image loaded status.
        /// </summary>
        OnLoad = 2,

        /// <summary>
        /// Image loading finished status.
        /// </summary>
        OnLoadEnd = 3,
    }
}
