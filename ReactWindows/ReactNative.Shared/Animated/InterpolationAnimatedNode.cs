// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using System;
using static System.FormattableString;

namespace ReactNative.Animated
{
    class InterpolationAnimatedNode : ValueAnimatedNode
    {
        public const string ExtrapolateTypeIdentity = "identity";
        public const string ExtrapolateTypeClamp = "clamp";
        public const string ExtrapolateTypeExtend = "extend";
        
        private readonly double[] _inputRange;
        private readonly double[] _outputRange;
        private readonly string _extrapolateLeft;
        private readonly string _extrapolateRight;

        private ValueAnimatedNode _parent;

        public InterpolationAnimatedNode(int tag, JObject config)
            : base(tag)
        {
            _inputRange = config.GetValue("inputRange", StringComparison.Ordinal).ToObject<double[]>();
            _outputRange = config.GetValue("outputRange", StringComparison.Ordinal).ToObject<double[]>();
            _extrapolateLeft = config.Value<string>("extrapolateLeft");
            _extrapolateRight = config.Value<string>("extrapolateRight");
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
                // The graph is in the middle of being created, just skip this
                // unattached node.
                return;
            }

            RawValue = Interpolate(_parent.Value, _inputRange, _outputRange, _extrapolateLeft, _extrapolateRight);
        }

        private static double Interpolate(
            double value,
            double inputMin,
            double inputMax,
            double outputMin,
            double outputMax,
            string extrapolateLeft,
            string extrapolateRight)
        {
            var result = value;

            // Extrapolate
            if (result < inputMin)
            {
                switch (extrapolateLeft)
                {
                    case ExtrapolateTypeIdentity:
                        return result;
                    case ExtrapolateTypeClamp:
                        result = inputMin;
                        break;
                    case ExtrapolateTypeExtend:
                        break;
                    default:
                        throw new InvalidOperationException(
                            Invariant($"Invalid extrapolation type '{extrapolateLeft}' for left extrapolation."));
                }
            }

            if (result > inputMax)
            {
                switch (extrapolateRight)
                {
                    case ExtrapolateTypeIdentity:
                        return result;
                    case ExtrapolateTypeClamp:
                        result = inputMax;
                        break;
                    case ExtrapolateTypeExtend:
                        break;
                    default:
                        throw new InvalidOperationException(
                            Invariant($"Invalid extrapolation type '{extrapolateRight} for right extrapolation"));
                }
            }

            return outputMin + (outputMax - outputMin) *
                (result - inputMin) / (inputMax - inputMin);
        }

        private static double Interpolate(
            double value, 
            double[] inputRange, 
            double[] outputRange,
            string extrapolateLeft,
            string extrapolateRight)
        {
            var rangeIndex = FindRangeIndex(value, inputRange);
            return Interpolate(
                value,
                inputRange[rangeIndex],
                inputRange[rangeIndex + 1],
                outputRange[rangeIndex],
                outputRange[rangeIndex + 1],
                extrapolateLeft,
                extrapolateRight);
        }

        private static int FindRangeIndex(double value, double[] ranges)
        {
            var index = 1;
            for (; index < ranges.Length - 1; ++index)
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
