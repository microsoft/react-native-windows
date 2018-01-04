using Newtonsoft.Json.Linq;
using ReactNative.UIManager.Events;

namespace ReactNative.UIManager
{
    class DragDropEvent : Event
    {
        private readonly string _name;
        private readonly JObject _data;

        public override string EventName => _name;

        public DragDropEvent(int viewTag, string name, JObject data)
            : base(viewTag)
        {
            _name = name;
            _data = data;
        }

        public override void Dispatch(RCTEventEmitter eventEmitter)
        {
            eventEmitter.receiveEvent(ViewTag, EventName, new JObject
            {
                { "target", ViewTag },
                { "dataTransfer", _data }
            });
        }
    }
}
