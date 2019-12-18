// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.ReactNative.Managed;
using System.Diagnostics;

namespace SampleAppCS
{
    [ReactModule]
    class DebugConsole
    {
        [ReactMethod]
        public void Log(string message)
        {
            Debug.WriteLine(message);
        }
    }
}
