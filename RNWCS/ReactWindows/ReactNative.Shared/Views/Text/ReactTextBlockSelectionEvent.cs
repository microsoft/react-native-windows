// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.UIManager.Events;

namespace ReactNative.Views.Text
{
    class ReactTextBlockSelectionEvent : Event
    {
        private readonly string _selectedText;

        public ReactTextBlockSelectionEvent(int viewTag, string selectedText)
            : base(viewTag)
        {
            _selectedText = selectedText;
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
            var eventData = new JObject
            {
                { "selectedText", _selectedText },
            };
            eventEmitter.receiveEvent(ViewTag, EventName, eventData);
        }
    }
}
