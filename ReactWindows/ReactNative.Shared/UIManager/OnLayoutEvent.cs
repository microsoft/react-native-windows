using Newtonsoft.Json.Linq;
using ReactNative.Pooling;
using ReactNative.UIManager.Events;

namespace ReactNative.UIManager
{
    class OnLayoutEvent : Event
    {
        private static readonly ObjectPool<OnLayoutEvent> s_eventsPool =
            new ObjectPool<OnLayoutEvent>(() => new OnLayoutEvent(), 20);

        private double _x;
        private double _y;
        private double _width;
        private double _height;

        private OnLayoutEvent() { }

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

        private void Init(int viewTag, double x, double y, double width, double height)
        {
            Init(viewTag);
            _x = x;
            _y = y;
            _width = width;
            _height = height;
        }

        protected override void OnDispose()
        {
            base.OnDispose();
            s_eventsPool.Free(this);
        }

        public static OnLayoutEvent Obtain(int viewTag, double x, double y, double width, double height)
        {
            var @event = s_eventsPool.Allocate();
            @event.Init(viewTag, x, y, width, height);
            return @event;
        }
    }
}
