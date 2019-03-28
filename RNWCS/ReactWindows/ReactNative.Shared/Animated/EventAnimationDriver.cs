// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.UIManager.Events;
using System;
using System.Collections.Generic;
using static System.FormattableString;

namespace ReactNative.Animated
{
    class EventAnimationDriver : RCTEventEmitter
    {
        private readonly IList<string> _eventPath;

        public EventAnimationDriver(IList<string> eventPath, ValueAnimatedNode valueNode)
        {
            _eventPath = eventPath;
            ValueNode = valueNode;
        }

        public ValueAnimatedNode ValueNode
        {
            get;
        }

        public override void receiveEvent(int targetTag, string eventName, JObject @event)
        {
            if (@event == null)
            {
                throw new ArgumentNullException(nameof(@event), "Native animated events must have event data.");
            }

            // Get the new value for the node by looking into the event map using the provided event path.
            var current = @event;
            for (var i = 0; i < _eventPath.Count - 1; i++)
            {
                current = (JObject)current[_eventPath[i]];
            }

            ValueNode.RawValue = current.Value<double>(_eventPath[_eventPath.Count - 1]);
        }

        public override void receiveTouches(string eventName, JArray touches, JArray changedIndexes)
        {
            throw new NotSupportedException(
                Invariant($"Method '{nameof(receiveTouches)}' is not support by native animated events."));
        }
    }
}
