using Newtonsoft.Json.Linq;

namespace ReactNative.UIManager.Events
{
    internal class AccessibilityTapEvent : Event
    {
        public const string EventNameValue = "topAccessibilityTap";

        public AccessibilityTapEvent(int viewTag)
            : base(viewTag)
        {
        }

        public override string EventName => EventNameValue;

        public override void Dispatch(RCTEventEmitter eventEmitter)
        {
            eventEmitter.receiveEvent(ViewTag, EventName, new JObject());
        }
    }
}
