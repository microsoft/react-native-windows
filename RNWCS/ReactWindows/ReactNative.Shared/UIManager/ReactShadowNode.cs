// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Facebook.Yoga;
using Newtonsoft.Json.Linq;
using ReactNative.Json;
using System;
using System.Collections.Generic;
using static System.FormattableString;

namespace ReactNative.UIManager
{
    /// <summary>
    /// Base node class for representing the virtual tree of React nodes.
    /// Shadow nodes are used primarily for layout, therefore it encapsulates
    /// <see cref="YogaNode"/> to allow that. Instances of this class receive 
    /// prop updates from JavaScript via the <see cref="UIManagerModule"/>.
    /// 
    /// This class allows for the native view hierarchy not to be an exact copy
    /// of the hierarchy received from JavaScript by keeping track of both
    /// JavaScript children (e.g., <see cref="ChildCount"/>) and
    /// separately native children (e.g., <see cref="NativeChildCount"/>). See
    /// <see cref="NativeViewHierarchyOptimizer"/> for more information.
    /// </summary>
    public class ReactShadowNode : IDisposable
    {
        private ReactShadowNode _rootNode;
        private ThemedReactContext _themedContext;
        private bool _nodeUpdated = true;
        private IList<ReactShadowNode> _children;
        private ReactShadowNode _parent;

        private readonly EdgeSpacing _defaultPadding = new EdgeSpacing(0);
        private readonly EdgeSpacing _padding = new EdgeSpacing(YogaConstants.Undefined);
        private YogaNode _yogaNode;
        private YogaNode _initialYogaNode;

        private bool _isLayoutOnly;
        private int _totalNativeChildren;
        private ReactShadowNode _nativeParent;
        private IList<ReactShadowNode> _nativeChildren;
        private bool _hasChildLayoutChanged;
        private bool _forceLayoutUpdate;

        /// <summary>
        /// Instantiates a <see cref="ReactShadowNode"/>. 
        /// </summary>
        public ReactShadowNode()
            : this(false)
        {
        }

        /// <summary>
        /// Instantiates a <see cref="ReactShadowNode"/>.
        /// </summary>
        /// <param name="isVirtual">Signals if the shadow node is virtual.</param>
        /// <param name="isDelegatedLayout">Signals if the shadow node is delegating the layouting to its child.</param>
        public ReactShadowNode(bool isVirtual, bool isDelegatedLayout = false)
        {
            IsVirtual = isVirtual;
            IsDelegatedLayout = isDelegatedLayout;

            if (!isVirtual)
            {
                _yogaNode = YogaNodePool.Instance.Allocate();
            }
            else
            {
                _yogaNode = null;
            }
        }

        /// <summary>
        /// The layout direction.
        /// </summary>
        public YogaDirection LayoutDirection 
        {
            get
            {
                return _yogaNode.LayoutDirection;
            }
            set
            {
                _yogaNode.StyleDirection = value;
            }
        }

        /// <summary>
        /// The width.
        /// </summary>
        public YogaValue StyleWidth
        {
            get
            {
                return _yogaNode.Width;
            }
            set
            {
                _yogaNode.Width = value;
            }
        }

        /// <summary>
        /// The minimum width.
        /// </summary>
        public YogaValue StyleMinWidth
        {
            set
            {
                _yogaNode.MinWidth = value;
            }
        }

        /// <summary>
        /// The maximum width.
        /// </summary>
        public YogaValue StyleMaxWidth
        {
            set
            {
                _yogaNode.MaxWidth = value;
            }
        }

        /// <summary>
        /// The height.
        /// </summary>
        public YogaValue StyleHeight
        {
            get
            {
                return _yogaNode.Height;
            }
            set
            {
                _yogaNode.Height = value;
            }
        }

        /// <summary>
        /// The minimum height.
        /// </summary>
        public YogaValue StyleMinHeight
        {
            set
            {
                _yogaNode.MinHeight = value;
            }
        }

        /// <summary>
        /// The maximum height.
        /// </summary>
        public YogaValue StyleMaxHeight
        {
            set
            {
                _yogaNode.MaxHeight = value;
            }
        }

        /// <summary>
        /// The flex value.
        /// </summary>
        public float Flex
        {
            set
            {
                _yogaNode.Flex = value;
            }
        }

        /// <summary>
        /// The flex grow value.
        /// </summary>
        public float FlexGrow
        {
            set
            {
                _yogaNode.FlexGrow = value;
            }
        }

        /// <summary>
        /// The flex shrink value.
        /// </summary>
        public float FlexShrink
        {
            set
            {
                _yogaNode.FlexShrink = value;
            }
        }

        /// <summary>
        /// The flex basis.
        /// </summary>
        public float FlexBasis
        {
            set
            {
                _yogaNode.FlexBasis = value;
            }
        }

        /// <summary>
        /// The aspect ratio.
        /// </summary>
        public float StyleAspectRatio
        {
            set
            {
                _yogaNode.AspectRatio = value;
            }
        }

        /// <summary>
        /// The flex direction.
        /// </summary>
        public YogaFlexDirection FlexDirection
        {
            set
            {
                _yogaNode.FlexDirection = value;
            }
        }

        /// <summary>
        /// The flex wrap value.
        /// </summary>
        public YogaWrap FlexWrap
        {
            set
            {
                _yogaNode.Wrap = value;
            }
        }

        /// <summary>
        /// The self alignment.
        /// </summary>
        public YogaAlign AlignSelf
        {
            set
            {
                _yogaNode.AlignSelf = value;
            }
        }

        /// <summary>
        /// The item alignment.
        /// </summary>
        public YogaAlign AlignItems
        {
            set
            {
                _yogaNode.AlignItems = value;
            }
        }

        /// <summary>
        /// The content alignment.
        /// </summary>
        public YogaAlign AlignContent
        {
            set
            {
                _yogaNode.AlignContent = value;
            }
        }

        /// <summary>
        /// The justify content value.
        /// </summary>
        public YogaJustify JustifyContent
        {
            set
            {
                _yogaNode.JustifyContent = value;
            }
        }

        /// <summary>
        /// The overflow value.
        /// </summary>
        public YogaOverflow Overflow
        {
            set
            {
                _yogaNode.Overflow = value;
            }
        }

        /// <summary>
        /// The display mode.
        /// </summary>
        public YogaDisplay Display
        {
            set
            {
                _yogaNode.Display = value;
            }
        }

        /// <summary>
        /// The position type.
        /// </summary>
        public YogaPositionType PositionType
        {
            set
            {
                _yogaNode.PositionType = value;
            }
        }

        /// <summary>
        /// Nodes that return <code>true</code> will be treated as "virtual"
        /// nodes. That is, nodes that are not mapped into native views (e.g.,
        /// nested text node).
        /// </summary>
        /// <remarks>
        /// By default this method returns <code>false</code>.
        /// </remarks>
        public bool IsVirtual { get; }

        /// <summary>
        /// Nodes that return <code>true</code> will be treated as a root view
        /// for the virtual nodes tree. It means that 
        /// <see cref="NativeViewHierarchyManager"/> will not try to perform
        /// manage children operations on such views.
        /// </summary>
        public virtual bool IsVirtualAnchor => false;

        /// <summary>
        /// Nodes that return <code>true</code> will be treated as relying on the child
        /// for layout purposes.
        /// </summary>
        /// <remarks>
        /// By default this method returns <code>false</code>.
        /// </remarks>
        public bool IsDelegatedLayout { get; }

        /// <summary>
        /// Signals that the node has updates.
        /// </summary>
        public bool HasUpdates => _nodeUpdated || HasNewLayout || IsDirty;

        /// <summary>
        /// Signals that the node has unseen updates.
        /// </summary>
        public bool HasUnseenUpdates => _nodeUpdated;

        /// <summary>
        /// Signals if the node is dirty.
        /// </summary>
        public bool IsDirty => _yogaNode != null && _yogaNode.IsDirty;

        /// <summary>
        /// Signals layout updates must be applied even if various optimizations suggest the contrary
        /// </summary>
        public bool MustForceLayout => _forceLayoutUpdate;
        
        /// <summary>
        /// The number of children.
        /// </summary>
        public int ChildCount => _children?.Count ?? 0;

        /// <summary>
        /// The tag for the node.
        /// </summary>
        public int ReactTag
        {
            get;
            internal set;
        }

        /// <summary>
        /// The root node.
        /// </summary>
        public ReactShadowNode RootNode
        {
            get
            {
                var rootNode = _rootNode;
                if (rootNode == null)
                {
                    throw new InvalidOperationException("Root node has not been set.");
                }

                return rootNode;
            }
            internal set
            {
                _rootNode = value;
            }
        }

        /// <summary>
        /// The view class.
        /// </summary>
        public string ViewClass
        {
            get;
            internal set;
        }

        /// <summary>
        /// The node parent.
        /// </summary>
        public ReactShadowNode Parent => _parent;

        /// <summary>
        /// The themed context.
        /// </summary>
        public ThemedReactContext ThemedContext
        {
            get
            {
                var themedContext = _themedContext;
                if (themedContext == null)
                {
                    throw new InvalidOperationException("Themed context has not been set.");
                }

                return themedContext;
            }
            protected internal set
            {
                _themedContext = value;
            }
        }

        /// <summary>
        /// Sets whether the node should notify on layout.
        /// </summary>
        public bool ShouldNotifyOnLayout
        {
            internal get;
            set;
        }

        /// <summary>
        /// Signals if a new layout is available.
        /// </summary>
        public bool HasNewLayout
        {
            get
            {
                if (IsDelegatedLayout)
                {
                    return _hasChildLayoutChanged;
                }
                return _yogaNode?.HasNewLayout ?? false;
            }
        }

        /// <summary>
        /// The number of native children.
        /// </summary>
        public int NativeChildCount => _nativeChildren?.Count ?? 0;

        /// <summary>
        /// The native parent.
        /// </summary>
        public ReactShadowNode NativeParent => _nativeParent;

        /// <summary>
        /// Signals whether the node is layout-only.
        /// </summary>
        public bool IsLayoutOnly
        {
            get
            {
                return _isLayoutOnly;
            }
            set
            {
                if (Parent != null)
                {
                    throw new InvalidOperationException("Must remove from parent first.");
                }

                if (_nativeParent != null)
                {
                    throw new InvalidOperationException("Must from from native parent first.");
                }

                if (NativeChildCount != 0)
                {
                    throw new InvalidOperationException("Must remove all native children first.");
                }

                _isLayoutOnly = value;
            }
        }

        /// <summary>
        /// Gets the total number of native children in the node hierarchy.
        /// </summary>
        public int TotalNativeChildren
        {
            get
            {
                return _totalNativeChildren;
            }
        }

        /// <summary>
        /// The layout horizontal position.
        /// </summary>
        /// <remarks>
        /// Children of IsDelegatedLayout nodes return a fake 0 for X since it's the parent that is responsible for exposing this
        /// </remarks>
        public float LayoutX => (_parent != null && _parent.IsDelegatedLayout) ? 0 : _yogaNode.LayoutX;

        /// <summary>
        /// The layout vertical position.
        /// </summary>
        /// <remarks>
        /// Children of IsDelegatedLayout nodes return a fake 0 for Y since it's the parent that is responsible for exposing this
        /// </remarks>
        public float LayoutY => (_parent != null && _parent.IsDelegatedLayout) ? 0 : _yogaNode.LayoutY;

        /// <summary>
        /// The layout width.
        /// </summary>
        public float LayoutWidth => _yogaNode.LayoutWidth;

        /// <summary>
        /// The layout height.
        /// </summary>
        public float LayoutHeight => _yogaNode.LayoutHeight;

        /// <summary>
        /// The screen horizontal position.
        /// </summary>
        public int ScreenX { get; private set; }

        /// <summary>
        /// The screen vertical position.
        /// </summary>
        public int ScreenY { get; private set; }

        /// <summary>
        /// The screen width.
        /// </summary>
        public int ScreenWidth { get; private set; }

        /// <summary>
        /// The screen height.
        /// </summary>
        public int ScreenHeight { get; private set; }

        /// <summary>
        /// The measure function.
        /// </summary>
        public MeasureFunction MeasureFunction
        {
            set
            {
                if ((value == null ^ _yogaNode.IsMeasureDefined) && ChildCount != 0)
                {
                    throw new InvalidOperationException(
                        "Since a node with a measure function does not add any native yoga children, it's " +
                        "not safe to transition to/from having a measure function unless a node has no children");
                }

                _yogaNode.SetMeasureFunction(value);
            }
        }

        /// <summary>
        /// Sets the margin for the node.
        /// </summary>
        /// <param name="spacingType">The spacing type.</param>
        /// <param name="margin">The margin.</param>
        public void SetMargin(int spacingType, YogaValue margin)
        {
            var yogaEdge = (YogaEdge)spacingType;
            switch (yogaEdge)
            {
                case YogaEdge.Left:
                    _yogaNode.MarginLeft = margin;
                    break;
                case YogaEdge.Top:
                    _yogaNode.MarginTop = margin;
                    break;
                case YogaEdge.Right:
                    _yogaNode.MarginRight = margin;
                    break;
                case YogaEdge.Bottom:
                    _yogaNode.MarginBottom = margin;
                    break;
                case YogaEdge.Start:
                    _yogaNode.MarginStart = margin;
                    break;
                case YogaEdge.End:
                    _yogaNode.MarginEnd = margin;
                    break;
                case YogaEdge.Horizontal:
                    _yogaNode.MarginHorizontal = margin;
                    break;
                case YogaEdge.Vertical:
                    _yogaNode.MarginVertical = margin;
                    break;
                case YogaEdge.All:
                    _yogaNode.Margin = margin;
                    break;
                default:
                    throw new NotSupportedException(
                        Invariant($"Unsupported margin type '{yogaEdge}'."));
            }
        }

        /// <summary>
        /// Sets the padding for the node.
        /// </summary>
        /// <param name="spacingType">The spacing type.</param>
        /// <returns>The padding.</returns>
        public float GetPadding(YogaEdge spacingType)
        {
            switch (spacingType)
            {
                case YogaEdge.Left:
                    return _yogaNode.LayoutPaddingLeft;
                case YogaEdge.Top:
                    return _yogaNode.LayoutPaddingTop;
                case YogaEdge.Right:
                    return _yogaNode.LayoutPaddingRight;
                case YogaEdge.Bottom:
                    return _yogaNode.LayoutPaddingBottom;
                case YogaEdge.Start:
                    return _yogaNode.LayoutPaddingStart;
                case YogaEdge.End:
                    return _yogaNode.LayoutPaddingEnd;
                default:
                    throw new NotSupportedException(
                        Invariant($"Unsupported padding type '{spacingType}'."));
            }
        }

        /// <summary>
        /// Sets the default padding for the node.
        /// </summary>
        /// <param name="spacingType">The spacing type.</param>
        /// <param name="padding">The padding.</param>
        public void SetDefaultPadding(int spacingType, YogaValue padding)
        {
            _defaultPadding.Set(spacingType, padding);
            UpdatePadding();
        }

        /// <summary>
        /// Sets the padding for the node.
        /// </summary>
        /// <param name="spacingType">The spacing type.</param>
        /// <param name="padding">The padding.</param>
        public void SetPadding(int spacingType, YogaValue padding)
        {
            _padding.Set(spacingType, padding);
            UpdatePadding();
        }

        /// <summary>
        /// Gets the border for the node.
        /// </summary>
        /// <param name="spacingType">The spacing type.</param>
        public float GetBorder(YogaEdge spacingType)
        {
            switch (spacingType)
            {
                case YogaEdge.Left:
                    return _yogaNode.BorderLeftWidth;
                case YogaEdge.Top:
                    return _yogaNode.BorderTopWidth;
                case YogaEdge.Right:
                    return _yogaNode.BorderRightWidth;
                case YogaEdge.Bottom:
                    return _yogaNode.BorderBottomWidth;
                case YogaEdge.Start:
                    return _yogaNode.BorderStartWidth;
                case YogaEdge.End:
                    return _yogaNode.BorderEndWidth;
                case YogaEdge.All:
                    return _yogaNode.BorderWidth;
                default:
                    throw new NotSupportedException(
                        Invariant($"Unsupported border type '{spacingType}'."));
            }
        }

        /// <summary>
        /// Sets the border for the node.
        /// </summary>
        /// <param name="spacingType">The spacing type.</param>
        /// <param name="borderWidth">The border width.</param>
        public void SetBorder(int spacingType, float borderWidth)
        {
            var yogaEdge = (YogaEdge)spacingType;
            switch (yogaEdge)
            {
                case YogaEdge.Left:
                    _yogaNode.BorderLeftWidth = borderWidth;
                    break;
                case YogaEdge.Top:
                    _yogaNode.BorderTopWidth = borderWidth;
                    break;
                case YogaEdge.Right:
                    _yogaNode.BorderRightWidth = borderWidth;
                    break;
                case YogaEdge.Bottom:
                    _yogaNode.BorderBottomWidth = borderWidth;
                    break;
                case YogaEdge.Start:
                    _yogaNode.BorderStartWidth = borderWidth;
                    break;
                case YogaEdge.End:
                    _yogaNode.BorderEndWidth = borderWidth;
                    break;
                case YogaEdge.All:
                    _yogaNode.BorderWidth = borderWidth;
                    break;
                default:
                    throw new NotSupportedException(
                        Invariant($"Unsupported border type '{yogaEdge}'."));
            }
        }

        /// <summary>
        /// Sets the position for the node.
        /// </summary>
        /// <param name="spacingType">The spacing type.</param>
        /// <param name="position">The position.</param>
        public void SetPosition(int spacingType, YogaValue position)
        {
            var yogaEdge = (YogaEdge)spacingType;
            switch (yogaEdge)
            {
                case YogaEdge.Left:
                    _yogaNode.Left = position;
                    break;
                case YogaEdge.Top:
                    _yogaNode.Top = position;
                    break;
                case YogaEdge.Right:
                    _yogaNode.Right = position;
                    break;
                case YogaEdge.Bottom:
                    _yogaNode.Bottom = position;
                    break;
                case YogaEdge.Start:
                    _yogaNode.Start = position;
                    break;
                case YogaEdge.End:
                    _yogaNode.End = position;
                    break;
                default:
                    throw new NotSupportedException(
                        Invariant($"Unsupported position type '{yogaEdge}'."));
            }
        }

        /// <summary>
        /// Calculate layout for the node.
        /// </summary>
        public void CalculateLayout()
        {
            _yogaNode.CalculateLayout();
        }

        /// <summary>
        /// Forces the further applying of layout updates bypassing optimizations
        /// </summary>
        public void MarkForceLayout()
        {
            _forceLayoutUpdate = true;
        }

        /// <summary>
        /// Marks that an update has been seen.
        /// </summary>
        public void MarkUpdateSeen()
        {
            _nodeUpdated = false;

            if (HasNewLayout)
            {
                MarkLayoutSeen();
            }
        }

        /// <summary>
        /// Marks layout seen.
        /// </summary>
        public void MarkLayoutSeen()
        {
            if (IsDelegatedLayout)
            {
                // We reset just this cached flag, the Yoga node is the one of the child's, and that one
                // has been already marked as seen by the one level deeper recursion.
                _hasChildLayoutChanged = false;
            }
            else
            {
                _yogaNode?.MarkLayoutSeen();
            }

            _forceLayoutUpdate = false;
        }

        /// <summary>
        /// Insert a child at the given index.
        /// </summary>
        /// <param name="child">The child.</param>
        /// <param name="index">The index.</param>
        public virtual void AddChildAt(ReactShadowNode child, int index)
        {
            if (child._parent != null)
            {
                throw new InvalidOperationException(
                  "Tried to add child that already has a parent! Remove it from its parent first.");
            }

            if (IsDelegatedLayout && (ChildCount > 0 || index != 0))
            {
                throw new InvalidOperationException(
                  "Cannot add more than one child to a CSS node that delegates the layouting!");
            }

            if (_children == null)
            {
                _children = new List<ReactShadowNode>(4);
            }

            _children.Insert(index, child);
            child._parent = this;

            // If a CSS node has measure defined, the layout algorithm will not visit its children. Even
            // more, it asserts that you don't add children to nodes with measure functions.
            if (_yogaNode != null && !_yogaNode.IsMeasureDefined)
            {
                var childYogaNode = child._yogaNode;
                if (childYogaNode == null)
                {
                    throw new InvalidOperationException(
                      "Cannot add a child that doesn't have a CSS node to a node without a measure function!");
                }

                if (IsDelegatedLayout)
                {
                    // Piggyback on child's Yoga node by keeping a reference to it
                    _initialYogaNode = _yogaNode;
                    _yogaNode = child._yogaNode;

                    // If parent has already been told about a Yoga node, make it aware of the new one
                    if (_parent != null)
                    {
                        _parent.ReplaceYogaNode(_initialYogaNode, _yogaNode);
                    }

                    _hasChildLayoutChanged = true;
                }
                else
                {
                    _yogaNode.Insert(index, childYogaNode);
                }
            }

            MarkUpdated();

            var increase = child._isLayoutOnly ? child._totalNativeChildren : 1;
            _totalNativeChildren += increase;

            UpdateNativeChildrenCountInParent(increase);
        }

        /// <summary>
        /// Removes the child at the given index.
        /// </summary>
        /// <param name="index">The index.</param>
        public ReactShadowNode RemoveChildAt(int index)
        {
            if (_children == null)
            {
                throw new IndexOutOfRangeException(
                  "Index " + index + " out of bounds: node has no children");
            }

            var removed = _children[index];
            _children.RemoveAt(index);
            removed._parent = null;

            if (_yogaNode != null && (!_yogaNode.IsMeasureDefined || IsDelegatedLayout))
            {
                if (IsDelegatedLayout)
                {
                    // Notify parent, if needed
                    if (_parent != null)
                    {
                        _parent.ReplaceYogaNode(_yogaNode, _initialYogaNode);
                    }

                    _yogaNode = _initialYogaNode;
                    _initialYogaNode = null;
                }
                else
                {
                    _yogaNode.RemoveAt(index);
                }
            }

            MarkUpdated();

            var decrease = removed._isLayoutOnly ? removed._totalNativeChildren : 1;
            _totalNativeChildren -= decrease;
            UpdateNativeChildrenCountInParent(-decrease);

            return removed;
        }

        /// <summary>
        /// Replaces a child Yoga node with a new one 
        /// </summary>
        /// <param name="oldNode">Old Yoga node child.</param>
        /// <param name="newNode">New Yoga node child.</param>
        public void ReplaceYogaNode(YogaNode oldNode, YogaNode newNode)
        {
            if (_yogaNode == null)
            {
                throw new InvalidOperationException(
                    "Cannot replace a Yoga child node when there's no parent!");
            }

            int index = _yogaNode.IndexOf(oldNode);
            if (index < 0)
            {
                throw new InvalidOperationException(
                    "Yoga child node is not present in parent!");
            }

            _yogaNode.RemoveAt(index);
            _yogaNode.Insert(index, newNode);
        }

        /// <summary>
        /// This method will be called by <see cref="UIManagerModule"/> once
        /// per batch, before calculating layout. This will only be called for
        /// nodes that are marked as updated with <see cref="MarkUpdated"/> or
        /// require layout (i.e., marked with <see cref="dirty"/>).
        /// </summary>
        public virtual void OnBeforeLayout()
        {
        }

        /// <summary>
        /// Updates the props of the node.
        /// </summary>
        /// <param name="props">The props.</param>
        public void UpdateProps(JObject props)
        {
            var setters = ViewManagersPropCache.GetNativePropSettersForShadowNodeType(GetType());
            foreach (var key in props.Keys())
            {
                if (setters.TryGetValue(key, out var setter))
                {
                    setter.UpdateShadowNodeProp(this, props);
                }
            }

            OnAfterUpdateTransaction();
        }

        /// <summary>
        /// Called following prop updates for node.
        /// </summary>
        public virtual void OnAfterUpdateTransaction()
        {
        }

        /// <summary>
        /// Called after a layout step at the end of a UI batch from
        /// <see cref="UIManagerModule"/>. May be used to enqueue additional UI
        /// operations for the native view. Will only be called on nodes marked
        /// as updated.
        /// </summary>
        /// <param name="uiViewOperationQueue">
        /// Interface for enqueueing UI operations.
        /// </param>
        public virtual void OnCollectExtraUpdates(UIViewOperationQueue uiViewOperationQueue)
        {
        }

        /// <summary>
        /// Gets the child node at the given index.
        /// </summary>
        /// <param name="index">The index.</param>
        /// <returns>The child node.</returns>
        public ReactShadowNode GetChildAt(int index)
        {
            if (_children == null)
            {
                throw new IndexOutOfRangeException(
                  "Index " + index + " out of bounds: node has no children");
            }

            return _children[index];
        }

        /// <summary>
        /// Gets the index of the given child node.
        /// </summary>
        /// <param name="child">The child node.</param>
        /// <returns>The index.</returns>
        public int IndexOf(ReactShadowNode child)
        {
            return _children?.IndexOf(child) ?? -1;
        }

        /// <summary>
        /// Removes and disposes all children.
        /// </summary>
        public void RemoveAndDisposeAllChildren()
        {
            if (ChildCount == 0)
            {
                return;
            }

            var decrease = 0;
            for (int i = ChildCount - 1; i >= 0; i--)
            {
                var toRemove = GetChildAt(i);

                if (_yogaNode != null && (!_yogaNode.IsMeasureDefined || IsDelegatedLayout))
                {
                    if (IsDelegatedLayout)
                    {
                        if (ChildCount > 1)
                        {
                            throw new InvalidOperationException(
                              "Cannot have more than one child to a CSS node that delegates the layouting!");
                        }

                        // Notify parent, if needed
                        if (_parent != null)
                        {
                            _parent.ReplaceYogaNode(_yogaNode, _initialYogaNode);
                        }

                        _yogaNode = _initialYogaNode;
                        _initialYogaNode = null;
                    }
                    else
                    {
                        _yogaNode.RemoveAt(i);
                    }
                }

                toRemove._parent = null;
                toRemove.Dispose();

                decrease += toRemove._isLayoutOnly ? toRemove._totalNativeChildren : 1;
            }

            _children.Clear();
            MarkUpdated();

            _totalNativeChildren -= decrease;
            UpdateNativeChildrenCountInParent(-decrease);
        }

        /// <summary>
        /// Adds a child that the native view hierarchy will have at this index
        /// in the native view corresponding to this node.
        /// </summary>
        /// <param name="child"></param>
        /// <param name="nativeIndex"></param>
        public void AddNativeChildAt(ReactShadowNode child, int nativeIndex)
        {
            if (IsLayoutOnly || child.IsLayoutOnly)
            {
                throw new InvalidOperationException("Invalid operation for layout-only nodes.");
            }

            if (_nativeChildren == null)
            {
                _nativeChildren = new List<ReactShadowNode>(4);
            }

            _nativeChildren.Insert(nativeIndex, child);
            child._nativeParent = this;
        }

        /// <summary>
        /// Removes the native child at the given index.
        /// </summary>
        /// <param name="index">The index.</param>
        /// <returns>The child removed.</returns>
        public ReactShadowNode RemoveNativeChildAt(int index)
        {
            if (_nativeChildren == null)
            {
                throw new InvalidOperationException("No native children available.");
            }

            var removed = _nativeChildren[index];
            _nativeChildren.RemoveAt(index);
            removed._nativeParent = null;
            return removed;
        }

        /// <summary>
        /// Remove all native children.
        /// </summary>
        public void RemoveAllNativeChildren()
        {
            if (_nativeChildren != null)
            {
                foreach (var item in _nativeChildren)
                {
                    item._nativeParent = null;
                }

                _nativeChildren.Clear();
            }
        }

        /// <summary>
        /// Gets the index of a native child.
        /// </summary>
        /// <param name="nativeChild">The native child.</param>
        /// <returns>The index, or -1 if none is found.</returns>
        public int IndexOfNativeChild(ReactShadowNode nativeChild)
        {
            return _nativeChildren.IndexOf(nativeChild);
        }

        /// <summary>
        /// Returns the offset within the native children owned by all layout-
        /// only nodes in the subtree rooted at this node for the given child.
        /// Put another way, this returns the number of native nodes (nodes not
        /// optimized out of the native tree) that are a) to the left (visited
        /// before by a depth-first search) of the given child in the subtree
        /// rooted at this node and b) do not have a native parent in this
        /// subtree (which means that the given child will be a sibling of
        /// theirs in the final native hierarchy since they'll get attached to
        /// the same native parent).
        /// 
        /// Basically, a view might have children that have been optimized away
        /// by <see cref="NativeViewHierarchyOptimizer"/>. Since those children
        /// will then add their native children to this view, we now have
        /// ranges of native children that correspond to single unoptimized
        /// children. The purpose of this method is to return the index within
        /// the native children that corresponds to the start of the native 
        /// children that belong to the given child. Also, note that all of the
        /// children of a view might be optimized away, so this could return
        /// the same value for multiple different children.
        /// </summary>
        /// <param name="child">The child.</param>
        /// <returns>The native offset.</returns>
        public int GetNativeOffsetForChild(ReactShadowNode child)
        {
            var index = 0;
            var found = false;
            for (var i = 0; i < ChildCount; ++i)
            {
                var current = GetChildAt(i);
                if (child == current)
                {
                    found = true;
                    break;
                }

                index += current.IsLayoutOnly ? current.TotalNativeChildren : 1;
            }

            if (!found)
            {
                throw new InvalidOperationException(
                    Invariant($"Child '{child.ReactTag}' was not a child of '{ReactTag}'."));
            }

            return index;
        }

        /// <summary>
        /// Disposes the shadow node.
        /// </summary>
        public void Dispose()
        {
            YogaNode nodeToDispose = (IsDelegatedLayout && _initialYogaNode != null) ? _initialYogaNode : _yogaNode;

            if (nodeToDispose != null)
            {
                nodeToDispose.Reset();
                YogaNodePool.Instance.Free(nodeToDispose);
            }
        }

        /// <summary>
        /// Dispatches a batch of updates.
        /// </summary>
        /// <param name="absoluteX">
        /// The absolute X-position of the node.
        /// </param>
        /// <param name="absoluteY">
        /// The 
        /// </param>
        /// <param name="uiViewOperationQueue">
        /// Interface for enqueueing UI operations.
        /// </param>
        /// <param name="nativeViewHierarchyOptimizer">
        /// Interface for optimizing native hierarchy calls.
        /// </param>
        /// <returns>
        /// <code>true</code> if updates were dispatched, otherwise <code>false</code>.
        /// </returns>
        internal bool DispatchUpdates(
            float absoluteX,
            float absoluteY,
            UIViewOperationQueue uiViewOperationQueue,
            NativeViewHierarchyOptimizer nativeViewHierarchyOptimizer)
        {
            if (_nodeUpdated)
            {
                OnCollectExtraUpdates(uiViewOperationQueue);
            }

            if (HasNewLayout)
            {
                var layoutX = LayoutX;
                var layoutY = LayoutY;
                var newAbsoluteLeft = (int)Math.Round(absoluteX + layoutX);
                var newAbsoluteTop = (int)Math.Round(absoluteY + layoutY);
                var newAbsoluteRight = (int)Math.Round(absoluteX + layoutX + LayoutWidth);
                var newAbsoluteBottom = (int)Math.Round(absoluteY + layoutY + LayoutHeight);

                var newScreenX = (int)Math.Round(layoutX);
                var newScreenY = (int)Math.Round(layoutY);
                var newScreenWidth = newAbsoluteRight - newAbsoluteLeft;
                var newScreenHeight = newAbsoluteBottom - newAbsoluteTop;

                var layoutHasChanged =
                    MustForceLayout ||
                    newScreenX != ScreenX ||
                    newScreenY != ScreenY ||
                    newScreenWidth != ScreenWidth ||
                    newScreenHeight != ScreenHeight;

                ScreenX = newScreenX;
                ScreenY = newScreenY;
                ScreenWidth = newScreenWidth;
                ScreenHeight = newScreenHeight;
                
                if (layoutHasChanged)
                {
                    nativeViewHierarchyOptimizer.HandleUpdateLayout(this);
                }

                return layoutHasChanged;
            }
            else
            {
                return false;
            }
        }

        /// <summary>
        /// Marks that the node is dirty.
        /// </summary>
        protected void dirty()
        {
            if (!IsVirtual)
            {
                _yogaNode.MarkDirty();
            }
        }

        /// <summary>
        /// Marks a node as updated.
        /// </summary>
        protected virtual void MarkUpdated()
        {
            if (_nodeUpdated)
            {
                return;
            }

            _nodeUpdated = true;
            var parent = Parent;
            parent?.MarkUpdated();
        }

        internal void BeforeDispatchUpdatesToDescendants()
        {
            if (IsDelegatedLayout)
            {
                // We cache the "changed" value from the child Yoga node so we can mark
                // the current shadow node as seen separately from the child.
                _hasChildLayoutChanged = _yogaNode?.HasNewLayout ?? false;
            }
        }

        private void UpdateNativeChildrenCountInParent(int delta)
        {
            if (_isLayoutOnly)
            {
                ReactShadowNode parent = Parent;
                while (parent != null)
                {
                    parent._totalNativeChildren += delta;
                    if (!parent._isLayoutOnly)
                    {
                        break;
                    }
                    parent = parent.Parent;
                }
            }
        }

        private void UpdatePadding()
        {
            for (var spacingType = EdgeSpacing.Left; spacingType <= EdgeSpacing.All; spacingType++)
            {
                if (spacingType == EdgeSpacing.Left ||
                    spacingType == EdgeSpacing.Right ||
                    spacingType == EdgeSpacing.Start ||
                    spacingType == EdgeSpacing.End)
                {
                    if (YogaConstants.IsUndefined(_padding.GetRaw(spacingType)) &&
                        YogaConstants.IsUndefined(_padding.GetRaw(EdgeSpacing.Horizontal)) &&
                        YogaConstants.IsUndefined(_padding.GetRaw(EdgeSpacing.All)))
                    {
                        SetPadding(_yogaNode, spacingType, _defaultPadding.GetRaw(spacingType));
                        continue;
                    }
                }
                else if (spacingType == EdgeSpacing.Top || spacingType == EdgeSpacing.Bottom)
                {
                    if (YogaConstants.IsUndefined(_padding.GetRaw(spacingType)) &&
                        YogaConstants.IsUndefined(_padding.GetRaw(EdgeSpacing.Vertical)) &&
                        YogaConstants.IsUndefined(_padding.GetRaw(EdgeSpacing.All)))
                    {
                        SetPadding(_yogaNode, spacingType, _defaultPadding.GetRaw(spacingType));
                        continue;
                    }
                }
                else
                {
                    if (YogaConstants.IsUndefined(_padding.GetRaw(spacingType)))
                    {
                        SetPadding(_yogaNode, spacingType, _defaultPadding.GetRaw(spacingType));
                        continue;
                    }
                }

                SetPadding(_yogaNode, spacingType, _padding.GetRaw(spacingType));
            }
        }

        private void SetPadding(YogaNode node, int spacingType, YogaValue padding)
        {
            var yogaEdge = (YogaEdge)spacingType;
            switch (yogaEdge)
            {
                case YogaEdge.Left:
                    node.PaddingLeft = padding;
                    break;
                case YogaEdge.Top:
                    node.PaddingTop = padding;
                    break;
                case YogaEdge.Right:
                    node.PaddingRight = padding;
                    break;
                case YogaEdge.Bottom:
                    node.PaddingBottom = padding;
                    break;
                case YogaEdge.Start:
                    node.PaddingStart = padding;
                    break;
                case YogaEdge.End:
                    node.PaddingEnd = padding;
                    break;
                case YogaEdge.Horizontal:
                    node.PaddingHorizontal = padding;
                    break;
                case YogaEdge.Vertical:
                    node.PaddingVertical = padding;
                    break;
                case YogaEdge.All:
                    node.Padding = padding;
                    break;
                default:
                    throw new NotSupportedException(
                        Invariant($"Unsupported padding type '{yogaEdge}'."));
            }
        }
    }
}
