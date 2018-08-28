// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using System;
using System.Linq;
#if WINDOWS_UWP
using Windows.UI.Xaml;
using Windows.UI.Xaml.Documents;
using Windows.UI.Xaml.Media;
#else
using ReactNative.Reflection;
using System.Collections;
using System.Windows;
using System.Windows.Documents;
using System.Windows.Media;
#endif
using static System.FormattableString;

namespace ReactNative.Views.Text
{
    /// <summary>
    /// A virtual view manager for raw text nodes.
    /// </summary>
    public class ReactSpanViewManager : ReactTextInlineViewManager<Span, ReactSpanShadowNode>, IViewParentManager
    {
        /// <summary>
        /// The view manager name.
        /// </summary>
        public override string Name
        {
            get
            {
                return "RCTVirtualText";
            }
        }

        /// <summary>
        /// This view does not layout its children.
        /// </summary>
        public bool NeedsCustomLayoutForChildren
        {
            get
            {
                return false;
            }
        }

        /// <summary>
        /// Sets the font color for the node.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="color">The masked color value.</param>
        [ReactProp(ViewProps.Color, CustomType = "Color")]
        public void SetColor(Span view, uint? color)
        {
            if (color.HasValue)
            {
                view.Foreground = new SolidColorBrush(ColorHelpers.Parse(color.Value));
            }
            else
            {
                view.ClearValue(Span.ForegroundProperty);
            }
        }

#if !WINDOWS_UWP
        /// <summary>
        /// Sets the TextDecorationLine for the node.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="textDecorationLineValue">The TextDecorationLine value.</param>
        [ReactProp(ViewProps.TextDecorationLine)]
        public void SetTextDecorationLine(Span view, string textDecorationLineValue)
        {
            var textDecorationLine = EnumHelpers.ParseNullable<TextDecorationLine>(textDecorationLineValue);

            if (textDecorationLine.HasValue)
            {
                switch (textDecorationLine.Value)
                {
                    case TextDecorationLine.Underline:
                        view.TextDecorations = TextDecorations.Underline;
                        break;
                    case TextDecorationLine.LineThrough:
                        view.TextDecorations = TextDecorations.Strikethrough;
                        break;
                    case TextDecorationLine.UnderlineLineThrough:
                        view.TextDecorations = new TextDecorationCollection(TextDecorations.Underline.Concat(TextDecorations.Strikethrough));
                        break;
                    case TextDecorationLine.None:
                    default:
                        view.TextDecorations = null;
                        break;
                }
            }
            else
            {
                view.ClearValue(Span.TextDecorationsProperty);
            }
        }
#endif

        /// <summary>
        /// Adds a child at the given index.
        /// </summary>
        /// <param name="parent">The parent view.</param>
        /// <param name="child">The child view.</param>
        /// <param name="index">The index.</param>
        public void AddView(object parent, object child, int index)
        {
            var span = (Span)parent;

            var dependencyObject = ViewConversion.GetDependencyObject(child);
            var inlineChild = dependencyObject as Inline;
            if (inlineChild == null)
            {
                inlineChild = new InlineUIContainer
                {
                    Child = dependencyObject.As<UIElement>(),
                };
            }

#if WINDOWS_UWP
            span.Inlines.Insert(index, inlineChild);
#else
            ((IList)span.Inlines).Insert(index, inlineChild);
#endif
        }

        /// <summary>
        /// This method should return the subclass of <see cref="ReactShadowNode"/>
        /// which will be then used for measuring the position and size of the
        /// view. 
        /// </summary>
        /// <remarks>
        /// In most cases, this will just return an instance of
        /// <see cref="ReactShadowNode"/>.
        /// </remarks>
        /// <returns>The shadow node instance.</returns>
        public override ReactSpanShadowNode CreateShadowNodeInstance()
        {
            return new ReactSpanShadowNode();
        }

        /// <summary>
        /// Gets the child at the given index.
        /// </summary>
        /// <param name="parent">The parent view.</param>
        /// <param name="index">The index.</param>
        /// <returns>The child view.</returns>
        public object GetChildAt(object parent, int index)
        {
            var span = (Span)parent;
#if WINDOWS_UWP
            var child = span.Inlines[index];
#else
            var child = ((IList)span.Inlines)[index];
#endif
            var childInlineContainer = child as InlineUIContainer;
            if (childInlineContainer != null)
            {
                return childInlineContainer.Child;
            }
            else
            {
                return child;
            }
        }

        /// <summary>
        /// Gets the number of children in the view parent.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        /// <returns>The number of children.</returns>
        public int GetChildCount(object parent)
        {
            var span = (Span)parent;
            return span.Inlines.Count;
        }

        /// <summary>
        /// Gets the dimensions of the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <returns>Does not return.</returns>
        /// <remarks>
        /// Dimensions are not available for virtual text nodes.
        /// </remarks>
        public override Dimensions GetDimensions(Span view)
        {
            throw new InvalidOperationException(
                Invariant($"Dimensions are not available for a virtual text node."));
        }

        /// <summary>
        /// Removes all children from the view parent.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        public void RemoveAllChildren(object parent)
        {
            var span = (Span)parent;
            span.Inlines.Clear();
        }

        /// <summary>
        /// Removes the child at the given index.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        /// <param name="index">The index.</param>
        public void RemoveChildAt(object parent, int index)
        {
            var span = (Span)parent;
#if WINDOWS_UWP
            span.Inlines.RemoveAt(index);
#else
            ((IList)span.Inlines).RemoveAt(index);
#endif
        }

        /// <summary>
        /// Sets the dimensions of the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="dimensions">The output buffer.</param>
        /// <remarks>
        /// This is a no-op as all layout is performed by the parent text block.
        /// </remarks>
        public override void SetDimensions(Span view, Dimensions dimensions)
        {
        }

        /// <summary>
        /// Creates a new view instance of type <see cref="Span"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <returns>The view instance.</returns>
        protected override Span CreateViewInstance(ThemedReactContext reactContext)
        {
            return new Span();
        }
    }
}
