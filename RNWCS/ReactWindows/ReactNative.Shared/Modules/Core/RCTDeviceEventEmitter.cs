// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using ReactNative.Bridge;

namespace ReactNative.Modules.Core
{
    /// <summary>
    /// JavaScript module for emitting device events.
    /// </summary>
    public sealed class RCTDeviceEventEmitter : JavaScriptModuleBase
    {
        /// <summary>
        /// Emits an event to the JavaScript instance.
        /// </summary>
        /// <param name="eventName">The event name.</param>
        /// <param name="data">The event data.</param>
        public void emit(string eventName, object data)
        {
            Invoke(eventName, data);
        }
    }
}
