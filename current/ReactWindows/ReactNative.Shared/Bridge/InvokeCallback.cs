// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;

namespace ReactNative.Bridge
{
    /// <summary>
    /// Delegate used to invoke JavaScript callbacks.
    /// </summary>
    /// <param name="callbackId">The callback identifier.</param>
    /// <param name="arguments">The callback arguments.</param>
    public delegate void InvokeCallback(int callbackId, JArray arguments);
}
