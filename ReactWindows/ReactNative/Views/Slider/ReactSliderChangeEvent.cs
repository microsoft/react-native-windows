using Newtonsoft.Json.Linq;
using ReactNative.UIManager.Events;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ReactNative.Views.Slider
{
    class ReactSliderChangeEvent : Event
    {
        private readonly double _value;

        public ReactSliderChangeEvent(int viewTag, double value)
                : base(viewTag, TimeSpan.FromTicks(Environment.TickCount))
            {
            _value = value;
        }

        public override string EventName
        {
            get
            {
                return "topValueChange";
            }
        }

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
