using Newtonsoft.Json.Linq;
using System;

namespace ReactNative.UIManager.Events
{
    class KeyEvent : Event
    {
        public const string KeyDownEventString = "topKeyDown";
        public const string KeyUpEventString = "topKeyUp";

        private readonly int _key;

        public KeyEvent(string eventName, int viewTag, int key)
            : base(viewTag)
        {
            EventName = eventName;
            _key = key;
        }

        public override string EventName
        {
            get;
        }

        public override bool CanCoalesce
        {
            get
            {
                return false;
            }
        }

        public override void Dispatch(RCTEventEmitter eventEmitter)
        {
            var eventData = new JObject
                {
                    { "target", ViewTag },
                    { "key", _key },
               };

            eventEmitter.receiveEvent(ViewTag, EventName, eventData);
        }
    }
}
