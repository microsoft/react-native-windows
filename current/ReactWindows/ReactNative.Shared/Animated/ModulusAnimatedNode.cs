// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using System;

namespace ReactNative.Animated
{
    class ModulusAnimatedNode : ValueAnimatedNode
    {
        private readonly NativeAnimatedNodesManager _manager;
        private int _inputNode;
        private int _modulus;

        public ModulusAnimatedNode(int tag, JObject config, NativeAnimatedNodesManager manager)
            : base(tag)
        {
            _manager = manager;
            _inputNode = config.Value<int>("input");
            _modulus = config.Value<int>("modulus");
        }

        public override void Update()
        {
            var animatedNode = _manager.GetNodeById(_inputNode);
            var valueNode = animatedNode as ValueAnimatedNode;
            if (valueNode == null)
            {
                throw new InvalidOperationException(
                    "Illegal node ID set as an input for Animated.modulus node.");
            }

            RawValue = valueNode.Value % _modulus;
        }
    }
}
