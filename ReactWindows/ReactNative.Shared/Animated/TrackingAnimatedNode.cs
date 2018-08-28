// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;

namespace ReactNative.Animated
{

    class TrackingAnimatedNode : AnimatedNode
    {
        private readonly NativeAnimatedNodesManager _manager;

        private readonly int _animationId;
        private readonly int _toValueNode;
        private readonly int _valueNode;
        private readonly JObject _animationConfig;

        public TrackingAnimatedNode(int tag, JObject config, NativeAnimatedNodesManager manager)
            : base(tag)
        {
            _manager = manager; 
            _animationId = config.Value<int>("animationId");
            _toValueNode = config.Value<int>("toValue");
            _valueNode = config.Value<int>("value");

            var animationConfig = config.Value<JObject>("animationConfig");
            _animationConfig = (JObject)animationConfig.DeepClone();
        }

        public override void Update()
        {
            AnimatedNode toValue = _manager.GetNodeById(_toValueNode);
            _animationConfig["toValue"] = ((ValueAnimatedNode)toValue).Value;
            _manager.StartAnimatingNode(_animationId, _valueNode, _animationConfig, null);
        }
    }
}

