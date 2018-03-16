// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;

namespace ReactNative.UIManager
{
    abstract class ReactShadowNodeVisitor<T>
    {
        public T Visit(ReactShadowNode node)
        {
            if (node == null)
            {
                throw new ArgumentNullException(nameof(node));
            }

            return VisitCore(node);
        }

        protected virtual T VisitCore(ReactShadowNode node)
        {
            var n = node.ChildCount;
            if (n == 0)
            {
                return Make(node, Array.Empty<T>());
            }
            else
            {
                var children = new List<T>(n);
                for (var i = 0; i < node.ChildCount; ++i)
                {
                    var child = node.GetChildAt(i);
                    children.Add(Visit(child));
                }

                return Make(node, children);
            }
        }

        protected abstract T Make(ReactShadowNode node, IList<T> children);
    }
}
