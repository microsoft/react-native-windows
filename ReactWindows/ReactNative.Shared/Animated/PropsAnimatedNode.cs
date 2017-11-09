using Newtonsoft.Json.Linq;
using ReactNative.Common;
using ReactNative.Tracing;
using ReactNative.UIManager;
using System;
using System.Collections.Generic;
using static System.FormattableString;

namespace ReactNative.Animated
{
    class PropsAnimatedNode : AnimatedNode
    {
        private readonly NativeAnimatedNodesManager _manager;
        private readonly UIImplementation _uiImplementation;
        private readonly Dictionary<string, int> _propNodeMapping;

        // This is the backing map for `_diffMap` we can mutate this to update
        // it instead of having to create a new one for each update.
        private readonly JObject _propMap;
        private readonly ReactStylesDiffMap _diffMap;

        private int _connectedViewTag = -1;

        public PropsAnimatedNode(int tag, JObject config, NativeAnimatedNodesManager manager, UIImplementation uiImplementation)
            : base(tag)
        {
            var props = (JObject)config.GetValue("props", StringComparison.Ordinal);
            _propNodeMapping = new Dictionary<string, int>(props.Count);
            foreach (var entry in props)
            {
                _propNodeMapping.Add(entry.Key, entry.Value.Value<int>());
            }

            _propMap = new JObject();
            _diffMap = new ReactStylesDiffMap(_propMap);
            _manager = manager;
            _uiImplementation = uiImplementation;
        }

        public void ConnectToView(int viewTag)
        {
            if (_connectedViewTag != -1)
            {
                throw new InvalidOperationException(
                    Invariant($"Animated node {Tag} has already been attached to a view."));
            }

            _connectedViewTag = viewTag;
        }

        public void DisconnectFromView(int viewTag)
        {
            if (_connectedViewTag != viewTag)
            {
                throw new InvalidOperationException(
                    Invariant($"Attempting to disconnect view that has not been connected with the given animated node."));
            }

            _connectedViewTag = -1;
        }

        public void RestoreDefaultValues()
        {
            foreach (var entry in _propMap)
            {
                _propMap[entry.Key] = null;
            }

            _uiImplementation.SynchronouslyUpdateViewOnDispatcherThread(
                _connectedViewTag,
                _diffMap);
        }

        public void UpdateView()
        {
            if (_connectedViewTag == -1)
            {
                return;
            }

            foreach (var entry in _propNodeMapping)
            {
                var node = _manager.GetNodeById(entry.Value);
                var styleNode = node as StyleAnimatedNode;
                var valueNode = default(ValueAnimatedNode);
                if (styleNode != null)
                {
                    styleNode.CollectViewUpdates(_propMap);
                }
                else if ((valueNode = node as ValueAnimatedNode) != null)
                {
                    _propMap[entry.Key] = valueNode.Value;
                }
                else
                {
                    throw new InvalidOperationException(
                        Invariant($"Unsupported type of node used in property node '{node.GetType()}'."));
                }
            }

            var updated = _uiImplementation.SynchronouslyUpdateViewOnDispatcherThread(
                _connectedViewTag,
                _diffMap);

            if (!updated)
            {
                Tracer.Error(ReactConstants.Tag, "Native animation workaround, frame lost as result of race condition.", null);
            }
        }
    }
}
