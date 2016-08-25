using Newtonsoft.Json.Linq;
using System;

namespace ReactNative.Animated
{
    class InterpolationAnimatedNode : ValueAnimatedNode
    {
        private readonly double[] _inputRange;
        private readonly double[] _outputRange;

        private ValueAnimatedNode _parent;

        public InterpolationAnimatedNode(int tag, JObject config)
            : base(tag)
        {
            _inputRange = config.GetValue("inputRange").ToObject<double[]>();
            _outputRange = config.GetValue("outputRange").ToObject<double[]>();
        }

        protected override void OnAttachedToNode(AnimatedNode parent)
        {
            if (_parent != null)
            {
                throw new InvalidOperationException("Parent already attached.");
            }

            var valueNode = parent as ValueAnimatedNode;
            if (valueNode == null)
            {
                throw new InvalidOperationException("Parent is not a value node.");
            }

            _parent = valueNode;
        }

        protected override void OnDetachedFromNode(AnimatedNode parent)
        {
            if (parent != _parent)
            {
                throw new InvalidOperationException("Invalid parent node provided.");
            }

            _parent = null;
        }

        public override void Update()
        {
            if (_parent == null)
            {
                throw new InvalidOperationException(
                    "Trying to update interpolation node that has not been attached to the parent.");
            }

            Value = Interpolate(_parent.Value, _inputRange, _outputRange);
        }

        private static double Interpolate(
            double value,
            double inputMin,
            double inputMax,
            double outputMin,
            double outputMax)
        {
            return outputMin + (outputMax - outputMin) *
                (value - inputMin) / (inputMax - inputMin);
        }

        private static double Interpolate(double value, double[] inputRange, double[] outputRange)
        {
            var rangeIndex = FindRangeIndex(value, inputRange);
            return Interpolate(
                value,
                inputRange[rangeIndex],
                inputRange[rangeIndex + 1],
                outputRange[rangeIndex],
                outputRange[rangeIndex + 1]);
        }

        private static int FindRangeIndex(double value, double[] ranges)
        {
            var index = 1;
            for (; index < ranges.Length; ++index)
            {
                if (ranges[index] >= value)
                {
                    break;
                }
            }

            return index - 1;
        }
    }
}
