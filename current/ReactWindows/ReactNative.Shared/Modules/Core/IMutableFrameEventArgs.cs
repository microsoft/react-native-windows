// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;

namespace ReactNative.Modules.Core
{
    interface IMutableFrameEventArgs
    {
        void Update(TimeSpan renderingTime);
    }
}
