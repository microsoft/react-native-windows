// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Facebook.Yoga;
using ReactNative.Reflection;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using System;
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

        private bool _allowFontScaling;

        private FontStyle? _fontStyle;
        private FontWeight? _fontWeight;
        private TextAlignment _textAlignment = TextAlignment.DetectFromContent;

        private string _fontFamily;

        /// <summary>
        /// Instantiates a <see cref="ReactTextShadowNode"/>.
        /// </summary>
        public ReactTextShadowNode()
        {
            MeasureFunction = (node, width, widthMode, height, heightMode) =>
                MeasureText(this, node, width, widthMode, height, heightMode);
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
        /// <param name="fontWeightValue">The font weight string.</param>
        [ReactProp(ViewProps.FontWeight)]
        public void SetFontWeight(string fontWeightValue)
        {
            var fontWeight = FontStyleHelpers.ParseFontWeight(fontWeightValue);
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
        /// <param name="fontStyleValue">The font style string.</param>
        [ReactProp(ViewProps.FontStyle)]
        public void SetFontStyle(string fontStyleValue)
        {
            var fontStyle = EnumHelpers.ParseNullable<FontStyle>(fontStyleValue);
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
            var spacing = 50*letterSpacing; // TODO: Find exact multiplier (50) to match iOS

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
        /// Set fontScaling
        /// </summary>
        /// <param name="allowFontScaling">Max number of lines.</param>
        [ReactProp(ViewProps.AllowFontScaling)]
        public virtual void SetAllowFontScaling(bool allowFontScaling)
        {
            if (_allowFontScaling != allowFontScaling)
            {
                _allowFontScaling = allowFontScaling;
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

        private static YogaSize MeasureText(ReactTextShadowNode textNode, YogaNode node, float width, YogaMeasureMode widthMode, float height, YogaMeasureMode heightMode)
        {
            // TODO: Measure text with DirectWrite or other API that does not
            // require dispatcher access. Currently, we're instantiating a
            // second CoreApplicationView (that is never activated) and using
            // its Dispatcher thread to calculate layout.
            var textBlock = new RichTextBlock
            {
                TextWrapping = TextWrapping.Wrap,
                TextAlignment = TextAlignment.DetectFromContent,
                TextTrimming = TextTrimming.CharacterEllipsis,
            };

            textNode.UpdateTextBlockCore(textBlock, true);

            var block = new Paragraph();
            for (var i = 0; i < textNode.ChildCount; ++i)
            {
                var child = textNode.GetChildAt(i);
                block.Inlines.Add(ReactInlineShadowNodeVisitor.Apply(child));
            }
            textBlock.Blocks.Add(block);

            var normalizedWidth = YogaConstants.IsUndefined(width) ? double.PositiveInfinity : width;
            var normalizedHeight = YogaConstants.IsUndefined(height) ? double.PositiveInfinity : height;
            textBlock.Measure(new Size(normalizedWidth, normalizedHeight));
            return MeasureOutput.Make(
                (float)Math.Ceiling(textBlock.DesiredSize.Width),
                (float)Math.Ceiling(textBlock.DesiredSize.Height));
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
            textBlock.IsTextScaleFactorEnabled = _allowFontScaling;

            if (!measureOnly)
            {
                textBlock.Padding = new Thickness(
                    GetPadding(YogaEdge.Left),
                    GetPadding(YogaEdge.Top),
                    GetPadding(YogaEdge.Right),
                    GetPadding(YogaEdge.Bottom));
            }
        }

        /// <summary>
        /// This method will be called by <see cref="UIManagerModule"/> once
        /// per batch, before calculating layout. This will only be called for
        /// nodes that are marked as updated with <see cref="MarkUpdated"/> or
        /// require layout (i.e., marked with <see cref="ReactShadowNode.dirty"/>).
        /// </summary>
        public override void OnBeforeLayout()
        {
            // Run flexbox on the children which are inline views.
            for (var i = 0; i < ChildCount; ++i)
            {
                var child = GetChildAt(i);
                if (!(child is ReactInlineShadowNode))
                {
                    child.CalculateLayout();
                }
            }
        }
    }
}
