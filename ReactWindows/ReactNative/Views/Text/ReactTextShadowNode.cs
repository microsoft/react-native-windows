using Facebook.CSSLayout;
using ReactNative.Bridge;
using ReactNative.Reflection;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using System;
using System.Collections.Generic;
using Windows.Foundation;
using Windows.UI.Text;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Documents;
using Windows.UI.Xaml.Media;

namespace ReactNative.Views.Text
{
    /// <summary>
    /// The shadow node implementation for text views.
    /// </summary>
    public class ReactTextShadowNode : LayoutShadowNode
    {
        private int _letterSpacing;
        private int _numberOfLines;

        private double? _fontSize;
        private double _lineHeight;

        private FontStyle? _fontStyle;
        private FontWeight? _fontWeight;
        private TextAlignment _textAlignment = TextAlignment.DetectFromContent;

        private string _fontFamily;

        /// <summary>
        /// Instantiates a <see cref="ReactTextShadowNode"/>.
        /// </summary>
        public ReactTextShadowNode()
        {
            MeasureFunction = MeasureText;
        }

        /// <summary>
        /// Instantiates the <see cref="ReactTextShadowNode"/>.
        /// </summary>
        /// <param name="isRoot">
        /// A flag signaling whether or not the node is the root node.
        /// </param>
        public ReactTextShadowNode(bool isRoot)
        {
            if (isRoot)
            {
                MeasureFunction = MeasureText;
            }
        }

        /// <summary>
        /// Sets the font size for the node.
        /// </summary>
        /// <param name="fontSize">The font size.</param>
        [ReactProp(ViewProps.FontSize)]
        public void SetFontSize(double? fontSize)
        {
            if (_fontSize != fontSize)
            {
                _fontSize = fontSize;
                MarkUpdated();
            }
        }

        /// <summary>
        /// Sets the font family for the node.
        /// </summary>
        /// <param name="fontFamily">The font family.</param>
        [ReactProp(ViewProps.FontFamily)]
        public void SetFontFamily(string fontFamily)
        {
            if (_fontFamily != fontFamily)
            {
                _fontFamily = fontFamily;
                MarkUpdated();
            }
        }

        /// <summary>
        /// Sets the font weight for the node.
        /// </summary>
        /// <param name="fontWeightString">The font weight string.</param>
        [ReactProp(ViewProps.FontWeight)]
        public void SetFontWeight(string fontWeightString)
        {
            var fontWeight = FontStyleHelpers.ParseFontWeight(fontWeightString);
            if (_fontWeight.HasValue != fontWeight.HasValue ||
                (_fontWeight.HasValue && fontWeight.HasValue &&
                _fontWeight.Value.Weight != fontWeight.Value.Weight))
            {
                _fontWeight = fontWeight;
                MarkUpdated();
            }
        }

        /// <summary>
        /// Sets the font style for the node.
        /// </summary>
        /// <param name="fontStyleString">The font style string.</param>
        [ReactProp(ViewProps.FontStyle)]
        public void SetFontStyle(string fontStyleString)
        {
            var fontStyle = EnumHelpers.ParseNullable<FontStyle>(fontStyleString);
            if (_fontStyle != fontStyle)
            {
                _fontStyle = fontStyle;
                MarkUpdated();
            }
        }

        /// <summary>
        /// Sets the letter spacing for the node.
        /// </summary>
        /// <param name="letterSpacing">The letter spacing.</param>
        [ReactProp(ViewProps.LetterSpacing)]
        public void SetLetterSpacing(int letterSpacing)
        {
            var spacing = 50 * letterSpacing; // TODO: Find exact multiplier (50) to match iOS

            if (_letterSpacing != spacing)
            {
                _letterSpacing = spacing;
                MarkUpdated();
            }
        }

        /// <summary>
        /// Sets the line height.
        /// </summary>
        /// <param name="lineHeight">The line height.</param>
        [ReactProp(ViewProps.LineHeight)]
        public virtual void SetLineHeight(double lineHeight)
        {
            if (_lineHeight != lineHeight)
            {
                _lineHeight = lineHeight;
                MarkUpdated();
            }
        }

        /// <summary>
        /// Sets the maximum number of lines.
        /// </summary>
        /// <param name="numberOfLines">Max number of lines.</param>
        [ReactProp(ViewProps.NumberOfLines)]
        public virtual void SetNumberOfLines(int numberOfLines)
        {
            if (_numberOfLines != numberOfLines)
            {
                _numberOfLines = numberOfLines;
                MarkUpdated();
            }
        }

        /// <summary>
        /// Sets the text alignment.
        /// </summary>
        /// <param name="textAlign">The text alignment string.</param>
        [ReactProp(ViewProps.TextAlign)]
        public void SetTextAlign(string textAlign)
        {
            var textAlignment = textAlign == "auto" || textAlign == null ? 
                TextAlignment.DetectFromContent :
                EnumHelpers.Parse<TextAlignment>(textAlign);

            if (_textAlignment != textAlignment)
            {
                _textAlignment = textAlignment;
                MarkUpdated();
            }
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
        public override void OnCollectExtraUpdates(UIViewOperationQueue uiViewOperationQueue)
        {
            base.OnCollectExtraUpdates(uiViewOperationQueue);
            uiViewOperationQueue.EnqueueUpdateExtraData(ReactTag, this);
        }

        /// <summary>
        /// Marks a node as updated.
        /// </summary>
        protected override void MarkUpdated()
        {
            base.MarkUpdated();
            dirty();
        }

        private static MeasureOutput MeasureText(CSSNode node, float width, CSSMeasureMode widthMode, float height, CSSMeasureMode heightMode)
        {
            // This is not a terribly efficient way of projecting the height of
            // the text elements. It requires that we have access to the
            // dispatcher in order to do measurement, which, for obvious
            // reasons, can cause perceived performance issues as it will block
            // the UI thread from handling other work.
            //
            // TODO: determine another way to measure text elements.
            var task = DispatcherHelpers.CallOnDispatcher(() =>
            {
                var textBlock = new RichTextBlock
                {
                    TextWrapping = TextWrapping.Wrap,
                    TextAlignment = TextAlignment.DetectFromContent,
                    TextTrimming = TextTrimming.CharacterEllipsis,
                };

                var textNode = (ReactTextShadowNode)node;
                textNode.UpdateTextBlockCore(textBlock, true);

                var block = new Paragraph();
                foreach (var child in textNode.Children)
                {
                    block.Inlines.Add(ReactInlineShadowNodeVisitor.Apply(child));
                }
                textBlock.Blocks.Add(block);

                var normalizedWidth = CSSConstants.IsUndefined(width) ? double.PositiveInfinity : width;
                var normalizedHeight = CSSConstants.IsUndefined(height) ? double.PositiveInfinity : height;
                textBlock.Measure(new Size(normalizedWidth, normalizedHeight));
                return new MeasureOutput(
                    (float)textBlock.DesiredSize.Width,
                    (float)textBlock.DesiredSize.Height);
            });

            return task.Result;
        }

        /// <summary>
        /// Updates the properties of a <see cref="RichTextBlock"/> view.
        /// </summary>
        /// <param name="textBlock">The view.</param>
        public void UpdateTextBlock(RichTextBlock textBlock)
        {
            UpdateTextBlockCore(textBlock, false);
        }

        private void UpdateTextBlockCore(RichTextBlock textBlock, bool measureOnly)
        {
            textBlock.CharacterSpacing = _letterSpacing;
            textBlock.LineHeight = _lineHeight;
            textBlock.MaxLines = _numberOfLines;
            textBlock.TextAlignment = _textAlignment;
            textBlock.FontFamily = _fontFamily != null ? new FontFamily(_fontFamily) : FontFamily.XamlAutoFontFamily;
            textBlock.FontSize = _fontSize ?? 15;
            textBlock.FontStyle = _fontStyle ?? FontStyle.Normal;
            textBlock.FontWeight = _fontWeight ?? FontWeights.Normal;

            if (!measureOnly)
            {
                textBlock.Padding = new Thickness(
                    this.GetPaddingSpace(CSSSpacingType.Left),
                    this.GetPaddingSpace(CSSSpacingType.Top),
                    0,
                    0);
            }
        }

        /// <summary>
        /// This method will be called by <see cref="UIManagerModule"/> once
        /// per batch, before calculating layout. This will only be called for
        /// nodes that are marked as updated with <see cref="MarkUpdated"/> or
        /// require layout (i.e., marked with <see cref="dirty"/>).
        /// </summary>
        public override void OnBeforeLayout()
        {
            // Run flexbox on the children which are inline views.
            foreach (var child in this.Children)
            {
                if (!(child is ReactRunShadowNode) && !(child is ReactSpanShadowNode))
                {
                    child.CalculateLayout();
                }
            }
        }
    }
}
