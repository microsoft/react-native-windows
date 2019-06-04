// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using System;

namespace ReactNative.Animated
{
    class AdditionAnimatedNode : ValueAnimatedNode
    {
        private readonly NativeAnimatedNodesManager _manager;
        private readonly int[] _inputNodes;

        public AdditionAnimatedNode(int tag, JObject config, NativeAnimatedNodesManager manager)
            : base(tag)
        {
            _manager = manager;
            _inputNodes = config.GetValue("input", StringComparison.Ordinal).ToObject<int[]>();
        }

        public override void Update()
        {
            RawValue = 0;
            foreach (var tag in _inputNodes)
            {
                var animatedNode = _manager.GetNodeById(tag);
                var valueNode = animatedNode as ValueAnimatedNode;
                if (valueNode == null)
                {
                    throw new InvalidOperationException(
                        "Illegal node ID set as an input for Animated.Add node.");
                }

                RawValue += valueNode.Value;
            }
        }
    }
}
