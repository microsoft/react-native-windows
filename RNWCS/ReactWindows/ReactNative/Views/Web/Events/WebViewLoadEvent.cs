// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.UIManager.Events;

namespace ReactNative.Views.Web.Events
{
    class WebViewLoadEvent : Event
    {
        public const string TopLoadingStart = "topLoadingStart";
        public const string TopLoadingFinish = "topLoadingFinish";

        private readonly string _url;
        private readonly bool _loading;
        private readonly string _title;
        private readonly bool _canGoBack;
        private readonly bool _canGoForward;

        public WebViewLoadEvent(
            int viewTag,
            string eventName,
            string url,
            bool loading,
            string title,
            bool canGoBack,
            bool canGoForward)
            : base(viewTag)
        {
            EventName = eventName;
            _url = url;
            _loading = loading;
            _title = title;
            _canGoBack = canGoBack;
            _canGoForward = canGoForward;
        }

        public override string EventName
        {
            get;
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
