// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace ReactNative.Modules.Core
{
    public interface IMutableFrameEventArgs
    {
        /// <summary>
        /// The relative frame time.
        /// </summary>
        TimeSpan RenderingTime
        {
            get;
        }

        /// <summary>
        /// The absolute frame time.
        /// </summary>
        DateTimeOffset FrameTime
        {
            get;
        }

        void Update(TimeSpan renderingTime);
    }
}
