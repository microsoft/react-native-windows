using System;
using Newtonsoft.Json.Linq;

namespace ReactNative.Animated
{
    class ValueAnimatedNode : AnimatedNode
    {
        private Action<double> _valueListener;

        public ValueAnimatedNode(int tag)
            : base(tag)
        {
        }

        public ValueAnimatedNode(int tag, JObject config)
            : base(tag)
        {
            Value = config.Value<double>("value");
        }

        public double Value
        {
            get;
            set;
        }

        public void OnValueUpdate()
        {
            _valueListener?.Invoke(Value);
        }

        public void SetValueListener(Action<double> listener)
        {
            _valueListener = listener;
        }
    }
}