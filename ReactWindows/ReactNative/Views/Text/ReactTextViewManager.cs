using ReactNative.Reflection;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using System;
using System.Linq;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Documents;
using Windows.UI.Xaml.Media;

namespace ReactNative.Views.Text
{
    /// <summary>
    /// The view manager for text views.
    /// </summary>
    public class ReactTextViewManager : ViewParentManager<RichTextBlock, ReactTextShadowNode>
    {
        private static readonly IReactCompoundView s_compoundView = new ReactTextCompoundView();

        /// <summary>
        /// The name of the view manager.
        /// </summary>
        public override string Name
        {
            get
            {
                return "RCTText";
            }
        }

        /// <summary>
        /// Sets the font color for the node.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="color">The masked color value.</param>
        [ReactProp(ViewProps.Color, CustomType = "Color")]
        public void SetColor(RichTextBlock view, uint? color)
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
        public void SetSelectable(RichTextBlock view, bool selectable)
        {
            view.IsTextSelectionEnabled = selectable;
        }

        /// <summary>
        /// Sets the TextDecorationLine for the node.
        /// </summary>
        /// /// <param name="view">The view.</param>
        /// <param name="textDecorationLineValue">The TextDecorationLine value.</param>
        [ReactProp(ViewProps.TextDecorationLine)]
        public void SetTextDecorationLine(RichTextBlock view, string textDecorationLineValue)
        {
            var textDecorationLine = EnumHelpers.ParseNullable<TextDecorationLine>(textDecorationLineValue) ?? TextDecorationLine.None;

            if (textDecorationLine != TextDecorationLine.None && textDecorationLine != TextDecorationLine.Underline)
            {
                throw new NotSupportedException("textDecorationLine = \"Strike-through\" is not implemeted in UWP");
            }

            var firstItemAsUnderline = view.Blocks.OfType<Paragraph>().First().Inlines.FirstOrDefault() as Underline;

            switch (textDecorationLine)
            {
                case TextDecorationLine.None:
                    {
                        if (firstItemAsUnderline == null)
                            break;

                        var inlines = GetInlines(view).ToArray();
                        var paragraph = view.Blocks.OfType<Paragraph>().First();
                        paragraph.Inlines.Clear();
                        firstItemAsUnderline.Inlines.Clear();
                        foreach (var i in inlines)
                        {
                            paragraph.Inlines.Add(i);
                        }
                        break;
                    }

                case TextDecorationLine.Underline:
                    {
                        if (firstItemAsUnderline != null)
                            break;

                        var inlines = GetInlines(view).ToArray();
                        var paragraph = view.Blocks.OfType<Paragraph>().First();
                        paragraph.Inlines.Clear();

                        var underline = new Underline();
                        foreach (var i in inlines)
                        {
                            underline.Inlines.Add(i);
                        }

                        paragraph.Inlines.Add(underline);

                        break;
                    }
            }
        }

        private InlineCollection GetInlines(RichTextBlock textBlock)
        {
            var inlines = textBlock.Blocks.OfType<Paragraph>().First().Inlines;
            var firstInnerUnderline = inlines.FirstOrDefault() as Underline;

            if (firstInnerUnderline != null)
            {
                return firstInnerUnderline.Inlines;
            }

            return inlines;
        }

        /// <summary>
        /// Adds a child at the given index.
        /// </summary>
        /// <param name="parent">The parent view.</param>
        /// <param name="child">The child view.</param>
        /// <param name="index">The index.</param>
        public override void AddView(RichTextBlock parent, DependencyObject child, int index)
        {
            var inlineChild = child as Inline;
            if (inlineChild == null)
            {
                inlineChild = new InlineUIContainer
                {
                    Child = (UIElement)child,
                };
            }

            GetInlines(parent).Insert(index, inlineChild);
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
        public override DependencyObject GetChildAt(RichTextBlock parent, int index)
        {
            var child = GetInlines(parent)[index];
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
        public override int GetChildCount(RichTextBlock parent)
        {
            return GetInlines(parent).Count;
        }

        /// <summary>
        /// Removes all children from the view parent.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        public override void RemoveAllChildren(RichTextBlock parent)
        {
            var inlines = GetInlines(parent);
            inlines.Clear();
        }

        /// <summary>
        /// Removes the child at the given index.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        /// <param name="index">The index.</param>
        public override void RemoveChildAt(RichTextBlock parent, int index)
        {
            var inlines = GetInlines(parent);
            inlines.RemoveAt(index);
        }

        /// <summary>
        /// Receive extra updates from the shadow node.
        /// </summary>
        /// <param name="root">The root view.</param>
        /// <param name="extraData">The extra data.</param>
        public override void UpdateExtraData(RichTextBlock root, object extraData)
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
        /// <param name="reactContext">The React context.</param>
        /// <returns>The view instance.</returns>
        protected override RichTextBlock CreateViewInstance(ThemedReactContext reactContext)
        {
            var richTextBlock = new RichTextBlock
            {
                IsTextSelectionEnabled = false,
                TextAlignment = TextAlignment.DetectFromContent,
                TextTrimming = TextTrimming.CharacterEllipsis,
                TextWrapping = TextWrapping.Wrap,
            };

            richTextBlock.Blocks.Add(new Paragraph());
            richTextBlock.SetReactCompoundView(s_compoundView);

            return richTextBlock;
        }
    }
}
