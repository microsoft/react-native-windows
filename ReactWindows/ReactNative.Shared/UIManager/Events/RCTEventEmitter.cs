// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Bridge;

namespace ReactNative.UIManager.Events
{
    /// <summary>
    /// JavaScript event emitter.
    /// </summary>
    public class RCTEventEmitter : JavaScriptModuleBase
    {
        /// <summary>
        /// Receive an event.
        /// </summary>
        /// <param name="targetTag">The target tag.</param>
        /// <param name="eventName">The event name.</param>
        /// <param name="event">The event data.</param>
        public virtual void receiveEvent(int targetTag, string eventName, JObject @event)
        {
            Invoke(targetTag, eventName, @event);
        }

        /// <summary>
        /// Receives touches.
        /// </summary>
        /// <param name="eventName">The event name.</param>
        /// <param name="touches">The touches.</param>
        /// <param name="changedIndexes">The changed indices.</param>
        public virtual void receiveTouches(string eventName, JArray touches, JArray changedIndexes)
        {
            Invoke(eventName, touches, changedIndexes);
        }
    }
}
