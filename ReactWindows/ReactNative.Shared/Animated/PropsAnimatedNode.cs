// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

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

        // We can mutate this to update instead of creating a new one for each update.
        private readonly JObject _propMap;

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
                _propMap);
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
                if (node is StyleAnimatedNode styleNode)
                {
                    styleNode.CollectViewUpdates(_propMap);
                }
                else if (node is ValueAnimatedNode valueNode)
                {
                    _propMap[entry.Key] = valueNode.Value;
                }
                else
                {
                    throw new InvalidOperationException(
                        Invariant($"Unsupported type of node used in prop node '{node.GetType()}'."));
                }
            }

            var updated = _uiImplementation.SynchronouslyUpdateViewOnDispatcherThread(
                _connectedViewTag,
                _propMap);

            if (!updated)
            {
                RnLog.Warn(ReactConstants.RNW, $"Native animation workaround, frame lost as result of race condition.");
            }
        }
    }
}
