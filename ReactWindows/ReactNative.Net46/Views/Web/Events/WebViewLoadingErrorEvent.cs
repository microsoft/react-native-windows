// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.UIManager.Events;

namespace ReactNative.Views.Web.Events
{
    class WebViewLoadingErrorEvent : Event
    {
        private readonly string _description;

        public WebViewLoadingErrorEvent(int viewTag, string error, string description)
            : base(viewTag)
        {
            if (description == null)
            {
                _description = error;
            }
            else
            {
                _description = description;
            }
        }

        public override string EventName
        {
            get
            {
                return "topLoadingError";
            }
        }

        public override void Dispatch(RCTEventEmitter eventEmitter)
        {
            var eventData = new JObject
            {
                { "target", ViewTag },
                { "description", _description },
            };

            eventEmitter.receiveEvent(ViewTag, EventName, eventData);
        }
    }
}
