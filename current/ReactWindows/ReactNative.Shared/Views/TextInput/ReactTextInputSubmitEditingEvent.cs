// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.UIManager.Events;

namespace ReactNative.Views.TextInput
{
    class ReactTextInputSubmitEditingEvent : Event
    {
        private readonly string _text;

        public ReactTextInputSubmitEditingEvent(int viewTag, string text)
            : base(viewTag)
        {
            _text = text;
        }

        public override string EventName
        {
            get
            {
                return "topSubmitEditing";
            }
        }

        public override bool CanCoalesce
        {
            get
            {
                return false;
            }
        }

        public override void Dispatch(RCTEventEmitter eventEmitter)
        {
            var eventData = new JObject
            {
                { "target", ViewTag },
                { "text", _text },
            };

            eventEmitter.receiveEvent(ViewTag, EventName, eventData);
        }
    }
}
