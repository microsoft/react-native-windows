using Newtonsoft.Json.Linq;
using ReactNative.UIManager;
using System;
using System.Collections.Generic;

namespace ReactNative.Animated
{
    class PropsAnimatedNode : AnimatedNode
    {
        private readonly NativeAnimatedNodesManager _manager;
        private readonly Dictionary<string, int> _propMapping;

        public PropsAnimatedNode(int tag, JObject config, NativeAnimatedNodesManager manager)
            : base(tag)
        {
            _manager = manager;

            var props = (JObject)config.GetValue("props");
            _propMapping = new Dictionary<string, int>(props.Count);
            foreach (var entry in props)
            {
                _propMapping.Add(entry.Key, entry.Value.Value<int>());
            }
        }

        public int ConnectedViewTag
        {
            get;
            set;
        } = -1;

        public void UpdateView(UIImplementation uiImplementation)
        {
            if (ConnectedViewTag == -1)
            {
                throw new InvalidOperationException("Node has not been attached to a view.");
            }

            var propsMap = new JObject();
            foreach (var entry in _propMapping)
            {
                var node = _manager.GetNodeById(entry.Value);
                var styleNode = node as StyleAnimatedNode;
                var valueNode = default(ValueAnimatedNode);
                if (styleNode != null)
                {
                    styleNode.CollectViewUpdates(propsMap);
                }
                else if ((valueNode = node as ValueAnimatedNode) != null)
                {
                    propsMap.Add(entry.Key, valueNode.Value);
                }
                else
                {
                    throw new InvalidOperationException(
                        $"Unsupported type of node used in property node '{node.GetType()}'.");
                }
            }

            // TODO: Reuse propsMap and stylesDiffMap objects - note that in
            // subsequent animation steps for a given node most of the time
            // will be creating the same set of props (just with different
            // values). We can take advantage on that and optimize the way we
            // allocate property maps (we also know that updating view props
            // doesn't retain a reference to the styles object).
            uiImplementation.SynchronouslyUpdateViewOnDispatcherThread(
                ConnectedViewTag,
                new ReactStylesDiffMap(propsMap));
        }
    }
}
