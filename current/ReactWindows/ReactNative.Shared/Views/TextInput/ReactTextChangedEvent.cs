// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.UIManager.Events;

namespace ReactNative.Views.TextInput
{
    /// <summary>
    /// Event emitted by <see cref="ReactTextInputManager"/> native view when
    /// text changes.
    /// </summary>
    class ReactTextChangedEvent : Event
    {
        private readonly string _text;
        private readonly int _eventCount;

        /// <summary>
        /// Instantiates a <see cref="ReactTextChangedEvent"/>.
        /// </summary>
        /// <param name="viewTag">The view tag.</param>
        /// <param name="text">The text.</param>
        /// <param name="eventCount">The event count.</param>
        public ReactTextChangedEvent(int viewTag, string text, int eventCount) 
            : base(viewTag)
        {
            _text = text;
            _eventCount = eventCount;
        }

        /// <summary>
        /// The name of the event.
        /// </summary>
        public override string EventName
        {
            get
            {
                return "topChange";
            }
        }

        /// <summary>
        /// Text change events cannot be coalesced.
        /// </summary>
        /// <remarks>
        /// Return <code>false</code> if the event can never be coalesced.
        /// </remarks>
        public override bool CanCoalesce
        {
            get
            {
                return false;
            }
        }

        /// <summary>
        /// Push the event up to the event emitter.
        /// </summary>
        /// <param name="rctEventEmitter">The event emitter.</param>
        public override void Dispatch(RCTEventEmitter rctEventEmitter)
        {
            var eventData = new JObject
            {
                { "text", _text },
                { "eventCount", _eventCount },
                { "target", ViewTag },
            };

            rctEventEmitter.receiveEvent(ViewTag, EventName, eventData);
        }
    }
}
