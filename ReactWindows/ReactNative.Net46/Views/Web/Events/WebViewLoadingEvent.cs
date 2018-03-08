// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.UIManager.Events;

namespace ReactNative.Views.Web.Events
{
    class WebViewLoadingEvent : Event
    {
        private readonly string _type;

        private readonly string _url;
        private readonly bool _loading;
        private readonly string _title;
        private readonly bool _canGoBack;
        private readonly bool _canGoForward;

        public WebViewLoadingEvent(
            int viewTag, 
            string type, 
            string url, 
            bool loading, 
            string title, 
            bool canGoBack, 
            bool canGoForward)
            : base(viewTag)
        {
            _type = type;
            _url = url;
            _loading = loading;
            _title = title;
            _canGoBack = canGoBack;
            _canGoForward = canGoForward;
        }

        public override string EventName
        {
            get
            {
                if (_type.Equals("Start"))
                {
                    return "topLoadingStart";
                }
                else
                {
                    return "topLoadingFinish";
                }

            }
        }

        public override void Dispatch(RCTEventEmitter eventEmitter)
        {
            var eventData = new JObject
            {
                { "target", ViewTag },
                { "url", _url },
                { "loading", _loading },
                { "title", _title },
                { "canGoBack", _canGoBack },
                { "canGoForward", _canGoForward }
            };

            eventEmitter.receiveEvent(ViewTag, EventName, eventData);
        }
    }
}
