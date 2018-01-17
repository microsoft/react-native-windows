using Newtonsoft.Json.Linq;
using System;

namespace ReactNative.UIManager.Events
{
    class BlurEvent : Event
    {
        public BlurEvent(int viewTag)
            : base(viewTag)
        {
        }

        public override string EventName
        {
            get
            {
                return "topBlur";
            }
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
                };

            eventEmitter.receiveEvent(ViewTag, EventName, eventData);
        }
    }
}
