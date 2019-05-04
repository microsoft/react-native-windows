// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.UIManager.Events;

namespace ReactNative.Views.Split.Events
{
    class SplitViewOpenedEvent : Event
    {
        public const string EventNameValue = "topSplitViewOpened";

        public SplitViewOpenedEvent(int viewTag)
            : base(viewTag)
        {
        }

        public override string EventName
        {
            get
            {
                return EventNameValue;
            }
        }

        public override void Dispatch(RCTEventEmitter eventEmitter)
        {
            eventEmitter.receiveEvent(ViewTag, EventName, new JObject());
        }
    }
}
