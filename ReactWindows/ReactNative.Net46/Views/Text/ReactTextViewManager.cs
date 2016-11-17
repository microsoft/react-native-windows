﻿using System;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using System.Collections;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Media;
using ReactNative.UIManager.Events;

namespace ReactNative.Views.Text
{
    /// <summary>
    /// The view manager for text views.
    /// </summary>
    public class ReactTextViewManager : ViewParentManager<TextBlock, ReactTextShadowNode>
    {
        private static readonly IReactCompoundView s_compoundView = new ReactTextCompoundView();

        /// <summary>
        /// Instantiates the class <see cref="ReactTextViewManager"/>.
        /// </summary>
        public ReactTextViewManager()
        {
        }

        /// <summary>
        /// Instantiates the class <see cref="ReactTextViewManager"/>.
        /// </summary>
        /// <param name="eventDispatcher">The event dispatcher to associate with this instance</param>
        public ReactTextViewManager(IEventDispatcher eventDispatcher)
            : base(eventDispatcher)
        {
        }

        /// <summary>
        /// The name of the view manager.
        /// </summary>
        public override string Name
        {
            get { return "RCTText"; }
        }

        /// <summary>
        /// Sets the font color for the node.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="color">The masked color value.</param>
        [ReactProp(ViewProps.Color, CustomType = "Color")]
        public void SetColor(TextBlock view, uint? color)
        {
            view.Foreground = color.HasValue
                ? new SolidColorBrush(ColorHelpers.Parse(color.Value))
                : null;
        }

        /// <summary>
        /// Sets whether or not the text is selectable.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="selectable">A flag indicating whether or not the text is selectable.</param>
        [ReactProp("selectable")]
        public void SetSelectable(TextBlock view, bool selectable)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Adds a child at the given index.
        /// </summary>
        /// <param name="parent">The parent view.</param>
        /// <param name="child">The child view.</param>
        /// <param name="index">The index.</param>
        public override void AddView(TextBlock parent, DependencyObject child, int index)
        {
            var inlineChild = child as Inline;
            if (inlineChild == null)
            {
                inlineChild = new InlineUIContainer
                {
                    Child = (UIElement) child,
                };
            }

            ((IList) parent.Inlines).Insert(index, inlineChild);
        }

        /// <summary>
        /// Creates the shadow node instance.
        /// </summary>
        /// <returns>The shadow node instance.</returns>
        public override ReactTextShadowNode CreateShadowNodeInstance()
        {
            return new ReactTextShadowNode();
        }

        /// <summary>
        /// Gets the child at the given index.
        /// </summary>
        /// <param name="parent">The parent view.</param>
        /// <param name="index">The index.</param>
        /// <returns>The child view.</returns>
        public override DependencyObject GetChildAt(TextBlock parent, int index)
        {
            var child = ((IList) parent.Inlines)[index];
            var childInlineContainer = child as InlineUIContainer;
            if (childInlineContainer != null)
            {
                return childInlineContainer.Child;
            }
            else
            {
                return (DependencyObject) child;
            }
        }

        /// <summary>
        /// Gets the number of children in the view parent.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        /// <returns>The number of children.</returns>
        public override int GetChildCount(TextBlock parent)
        {
            return ((IList) parent.Inlines).Count;
        }

        /// <summary>
        /// Removes all children from the view parent.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        public override void RemoveAllChildren(TextBlock parent)
        {
            var inlines = parent.Inlines;
            inlines.Clear();
        }

        /// <summary>
        /// Removes the child at the given index.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        /// <param name="index">The index.</param>
        public override void RemoveChildAt(TextBlock parent, int index)
        {
            var inlines = (IList) parent.Inlines;
            inlines.RemoveAt(index);
        }

        /// <summary>
        /// Receive extra updates from the shadow node.
        /// </summary>
        /// <param name="root">The root view.</param>
        /// <param name="extraData">The extra data.</param>
        public override void UpdateExtraData(TextBlock root, object extraData)
        {
            base.UpdateExtraData(root, extraData);

            var textNode = extraData as ReactTextShadowNode;
            if (textNode != null)
            {
                textNode.UpdateTextBlock(root);
            }
        }

        /// <summary>
        /// Creates the view instance.
        /// </summary>
        /// <returns>The view instance.</returns>
        protected override TextBlock CreateViewInstance()
        {
            var textBlock = new TextBlock
            {
                TextAlignment = TextAlignment.Left,
                TextTrimming = TextTrimming.CharacterEllipsis,
            };

            textBlock.SetReactCompoundView(s_compoundView);

            return textBlock;
        }

        /// <summary>
        /// Installing the sizechanged event emitter on the <see cref="TextBlock"/> Control.
        /// </summary>
        /// <param name="view">The <see cref="TextBlock"/> view instance.</param>
        protected override void AddEventEmitters(TextBlock view)
        {
            base.AddEventEmitters(view);
            view.SizeChanged += OnSizeChanged;
        }

        /// <summary>
        /// Called when view is detached from view hierarchy and allows for
        /// additional cleanup by the <see cref="ReactTextViewManager"/>.
        /// subclass. Unregister all event handlers for the <see cref="TextBlock"/>.
        /// </summary>
        /// <param name="view">The <see cref="TextBlock"/>.</param>
        public override void OnDropViewInstance(TextBlock view)
        {
            base.OnDropViewInstance(view);

            view.SizeChanged -= OnSizeChanged;
        }

        private void OnSizeChanged(object sender, SizeChangedEventArgs e)
        {
            var textBlock = (TextBlock) sender;

            int viewTag = textBlock.HasTag() ? textBlock.GetTag() : -1;

            EventDispatcher
                .DispatchEvent(
                    new ReactTextViewSizeChangedEvent(viewTag, e.NewSize, e.PreviousSize, e.HeightChanged, e.WidthChanged));
        }
    }
}
