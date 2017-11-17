using Newtonsoft.Json.Linq;
using ReactNative.UIManager.Events;

namespace ReactNative.Views.TextInput
{
    class ReactTextInputContentSizeChangedEvent : Event
    {
        private readonly double _width;
        private readonly double _height;

        public ReactTextInputContentSizeChangedEvent(
            int viewTag,
            double width,
            double height)
            : base(viewTag)
        {
            _height = height;
            _width = width;
        }

        public override string EventName
        {
            get
            {
                return "topContentSizeChange";
            }
        }

        public override void Dispatch(RCTEventEmitter eventEmitter)
        {
            eventEmitter.receiveEvent(ViewTag, EventName, new JObject
            {
                { "target", ViewTag },
                {
                    "contentSize",
                    new JObject
                    {
                        { "width", _width },
                        { "height", _height },
                    }
                },
            });
        }
    }
}
