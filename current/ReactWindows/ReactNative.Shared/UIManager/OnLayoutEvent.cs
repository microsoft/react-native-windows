// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.UIManager.Events;
using System;

namespace ReactNative.UIManager
{
    class OnLayoutEvent : Event
    {
        private double _x;
        private double _y;
        private double _width;
        private double _height;

        private OnLayoutEvent(int viewTag, double x, double y, double width, double height)
            : base(viewTag)
        {
            _x = x;
            _y = y;
            _width = width;
            _height = height;
        }

        public override string EventName
        {
            get
            {
                return "topLayout";
            }
        }

        public override void Dispatch(RCTEventEmitter eventEmitter)
        {
            var eventArgs = new JObject
            {
                { "target", ViewTag },
                {
                    "layout",
                    new JObject
                    {
                        { "x", _x },
                        { "y", _y },
                        { "width", _width },
                        { "height", _height },
                    }
                },
            };

            eventEmitter.receiveEvent(ViewTag, EventName, eventArgs);
        }

        public static OnLayoutEvent Obtain(int viewTag, double x, double y, double width, double height)
        {
            // TODO: Introduce pooling mechanism
            return new OnLayoutEvent(viewTag, x, y, width, height);
        }
    }
}
