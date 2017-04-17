using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using static System.FormattableString;

namespace ReactNative.Animated
{
    class TransformAnimatedNode : AnimatedNode
    {
        private readonly NativeAnimatedNodesManager _manager;
        private readonly List<TransformConfig> _transformConfigs;

        public TransformAnimatedNode(int tag, JObject config, NativeAnimatedNodesManager manager)
            : base(tag)
        {
            _manager = manager;

            var transforms = (JArray)config.GetValue("transforms", StringComparison.Ordinal);
            _transformConfigs = new List<TransformConfig>(transforms.Count);
            for (var i = 0; i < transforms.Count; ++i)
            {
                var transformConfigMap = transforms[i];
                var property = transformConfigMap.Value<string>("property");
                var type = transformConfigMap.Value<string>("type");
                if (type == "animated")
                {
                    _transformConfigs.Add(new AnimatedTransformConfig
                    {
                        Property = property,
                        NodeTag = transformConfigMap.Value<int>("nodeTag"),
                    });
                }
                else
                {
                    _transformConfigs.Add(new StaticTransformConfig
                    {
                        Property = property,
                        Value = transformConfigMap.Value<double>("value"),
                    });
                }
            }
        }

        public void CollectViewUpdates(JObject propsMap)
        {
            var transforms = new JArray();
            foreach (var transformConfig in _transformConfigs)
            {
                var value = default(double);
                var animatedConfig = transformConfig as AnimatedTransformConfig;
                if (animatedConfig != null)
                {
                    var node = _manager.GetNodeById(animatedConfig.NodeTag);
                    var valueNode = node as ValueAnimatedNode;
                    if (valueNode != null)
                    {
                        value = valueNode.Value;
                    }
                    else
                    {
                        throw new InvalidOperationException(
                            Invariant($"Unsupported type of node used as transform child node: '{node.GetType()}'"));
                    }
                }
                else
                {
                    value = ((StaticTransformConfig)transformConfig).Value;
                }

                transforms.Add(new JObject
                {
                    { transformConfig.Property, value },
                });
            }

            propsMap.Add("transform", transforms);
        }

        class TransformConfig
        {
            public string Property;
        }

        class AnimatedTransformConfig : TransformConfig
        {
            public int NodeTag;
        }

        class StaticTransformConfig : TransformConfig
        {
            public double Value;
        }
    }
}
