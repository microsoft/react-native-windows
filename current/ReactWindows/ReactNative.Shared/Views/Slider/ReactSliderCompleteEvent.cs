// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.UIManager.Events;

namespace ReactNative.Views.Slider
{
    /// <summary>
    /// Event emitted by <see cref="ReactSliderManager"/> native view after 
    /// last value change.
    /// </summary>
    class ReactSliderCompleteEvent : Event
    {
        private readonly double _value;

        /// <summary>
        /// Instantiates a <see cref="ReactSliderCompleteEvent"/>.
        /// </summary>
        /// <param name="viewTag">The view tag.</param>
        /// <param name="value">Slider value.</param>
        public ReactSliderCompleteEvent(int viewTag, double value)
            : base(viewTag)
        {
            _value = value;
        }

        /// <summary>
        /// The event name.
        /// </summary>
        public override string EventName
        {
            get
            {
                return "topSlidingComplete";
            }
        }

        /// <summary>
        /// Dispatch this event to JavaScript using the given event emitter.
        /// </summary>
        /// <param name="eventEmitter">The event emitter.</param>
        public override void Dispatch(RCTEventEmitter eventEmitter)
        {
            var eventData = new JObject
            {
                { "target", ViewTag },
                { "value", _value },
            };

            eventEmitter.receiveEvent(ViewTag, EventName, eventData);
        }
    }
}
