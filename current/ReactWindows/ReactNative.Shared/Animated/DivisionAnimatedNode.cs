// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using System;

namespace ReactNative.Animated
{
    class DivisionAnimatedNode : ValueAnimatedNode
    {
        private readonly NativeAnimatedNodesManager _manager;
        private readonly int[] _inputNodes;

        public DivisionAnimatedNode(int tag, JObject config, NativeAnimatedNodesManager manager) 
            : base(tag, config)
        {
            _manager = manager;
            _inputNodes = config.GetValue("input", StringComparison.Ordinal).ToObject<int[]>();
        }

        public override void Update()
        {
            for (int i = 0; i < _inputNodes.Length; ++i)
            {
                var valueNode = _manager.GetNodeById(_inputNodes[i]) as ValueAnimatedNode;
                if (valueNode == null)
                {
                    throw new InvalidOperationException(
                        "Illegal node ID set as an input for Animated.divide node.");
                }

                var value = valueNode.Value;
                if (i == 0)
                {
                    RawValue = value;
                    continue;
                }

                if (value == 0)
                {
                    throw new DivideByZeroException(
                        "Detected a division by zero in Animated.divide node.");
                }

                RawValue /= value;
            }
        }
    }
}
