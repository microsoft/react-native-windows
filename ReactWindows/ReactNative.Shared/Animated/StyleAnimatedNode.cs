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
                var transformNode = node as TransformAnimatedNode;
                var valueNode = default(ValueAnimatedNode);
                if (transformNode != null)
                {
                    transformNode.CollectViewUpdates(propsMap);
                }
                else if ((valueNode = node as ValueAnimatedNode) != null)
                {
                    propsMap.Add(entry.Key, valueNode.Value);
                }
                else
                {
                    throw new InvalidOperationException(
                        Invariant($"Unsupported type of node used in property node: '{node.GetType()}'"));
                }
            }
        }
    }
}
