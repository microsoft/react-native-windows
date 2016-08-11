using Newtonsoft.Json.Linq;
using System;

namespace ReactNative.Animated
{
    class ValueAnimatedNode : AnimatedNode
    {
        private Action<double> _valueListener;

        public ValueAnimatedNode(int tag, JObject config)
            : this(tag)
        {
            Value = config.Value<double>("value");
        }

        public ValueAnimatedNode(int tag)
            : base(tag)
        {
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
