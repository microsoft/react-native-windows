// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.UIManager.Events;
using System;

namespace ReactNative.Tests
{
    class MockEvent : Event
    {
        private readonly string _eventName;
        private readonly JObject _eventArgs;
        private readonly Action _onDispose;

        public MockEvent(int viewTag, string eventName)
            : this(viewTag, eventName, new JObject())
        {
        }

        public MockEvent(int viewTag, string eventName, JObject eventArgs)
            : this(viewTag, eventName, eventArgs, () => { })
        {
        }

        public MockEvent(int viewTag, string eventName, JObject eventArgs, Action onDispose)
            : base(viewTag)
        {
            _eventName = eventName;
            _eventArgs = eventArgs;
            _onDispose = onDispose;
        }

        public override string EventName
        {
            get
            {
                return _eventName;
            }
        }

        public override void Dispatch(RCTEventEmitter eventEmitter)
        {
            eventEmitter.receiveEvent(ViewTag, EventName, _eventArgs);
        }

        protected override void OnDispose()
        {
            _onDispose();
        }
    }
}
