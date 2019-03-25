// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.UIManager.Events;

namespace ReactNative.Views.TextInput
{
    class ReactTextInputSelectionEvent : Event
    {
        private readonly int _end;
        private readonly int _start;

        public ReactTextInputSelectionEvent(int viewTag, int start, int end)
            : base(viewTag)
        {
            _start = start;
            _end = end;
        }

        public override string EventName
        {
            get
            {
                return "topSelectionChange";
            }
        }

        public override void Dispatch(RCTEventEmitter eventEmitter)
        {
            var selectionData = new JObject
            {
                { "start", _start },
                { "end", _end },
            };

            var eventData = new JObject
            {
                { "selection", selectionData },
            };

            eventEmitter.receiveEvent(ViewTag, EventName, eventData);
        }
    }
}
