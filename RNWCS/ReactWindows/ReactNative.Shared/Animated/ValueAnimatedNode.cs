// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

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
            RawValue = config.Value<double>("value");
            Offset = config.Value<double>("offset");
        }

        public ValueAnimatedNode(int tag)
            : base(tag)
        {
        }

        public double RawValue
        {
            get;
            set;
        }

        public double Value
        {
            get
            {
                return Offset + RawValue;
            }
        }

        public double Offset
        {
            get;
            set;
        }

        public void ExtractOffset()
        {
            Offset += RawValue;
            RawValue = 0;
        }

        public void FlattenOffset()
        {
            RawValue += Offset;
            Offset = 0;
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
