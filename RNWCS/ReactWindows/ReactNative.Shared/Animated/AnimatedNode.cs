// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using System.Collections.Generic;

namespace ReactNative.Animated
{
    abstract class AnimatedNode
    {
        public const int InitialBfsColor = 0;

        private List<AnimatedNode> _children;

        public AnimatedNode(int tag)
        {
            Tag = tag;
        }

        public int Tag
        {
            get;
        }

        public IReadOnlyList<AnimatedNode> Children
        {
            get
            {
                return _children;
            }
        }

        public long BfsColor
        {
            get;
            set;
        }

        public int ActiveIncomingNodes
        {
            get;
            set;
        }

        public void AddChild(AnimatedNode child)
        {
            if (_children == null)
            {
                _children = new List<AnimatedNode>(1);
            }

            _children.Add(child);
            child.OnAttachedToNode(this);
        }

        public void RemoveChild(AnimatedNode child)
        {
            if (_children == null)
            {
                return;
            }

            child.OnDetachedFromNode(this);
            _children.Remove(child);
        }

        public virtual void Update()
        {
        }

        protected virtual void OnDetachedFromNode(AnimatedNode animatedNode)
        {
        }

        protected virtual void OnAttachedToNode(AnimatedNode animatedNode)
        {
        }
    }
}
