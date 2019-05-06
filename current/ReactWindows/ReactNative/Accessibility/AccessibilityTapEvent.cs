// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.UIManager.Events;

namespace ReactNative.Accessibility
{
    internal class AccessibilityTapEvent : Event
    {
        public const string EventNameValue = "topAccessibilityTap";

        public AccessibilityTapEvent(int viewTag)
            : base(viewTag)
        {
        }

        public override string EventName => EventNameValue;

        public override void Dispatch(RCTEventEmitter eventEmitter)
        {
            eventEmitter.receiveEvent(ViewTag, EventName, new JObject());
        }
    }
}
