// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using static System.FormattableString;

namespace ReactNative.Animated
{
    class StyleAnimatedNode : AnimatedNode
    {
        private readonly NativeAnimatedNodesManager _manager;
        private readonly Dictionary<string, int> _propMapping;

        public StyleAnimatedNode(int tag, JObject config, NativeAnimatedNodesManager manager)
            : base(tag)
        {
            _manager = manager;

            var style = (JObject)config.GetValue("style", StringComparison.Ordinal);
            _propMapping = new Dictionary<string, int>(style.Count);
            foreach (var entry in style)
            {
                _propMapping.Add(entry.Key, entry.Value.Value<int>());
            }
        }

        public void CollectViewUpdates(JObject propsMap)
        {
            foreach (var entry in _propMapping)
            {
                var node = _manager.GetNodeById(entry.Value);
                if (node is TransformAnimatedNode transformNode)
                {
                    transformNode.CollectViewUpdates(propsMap);
                }
                else if (node is ValueAnimatedNode valueNode)
                {
                    propsMap[entry.Key] = valueNode.Value;
                }
                else
                {
                    throw new InvalidOperationException(
                        Invariant($"Unsupported type of node used in prop node: '{node.GetType()}'"));
                }
            }
        }
    }
}
