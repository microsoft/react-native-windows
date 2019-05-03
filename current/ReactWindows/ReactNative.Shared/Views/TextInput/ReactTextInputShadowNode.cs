// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Facebook.Yoga;
using Newtonsoft.Json.Linq;
using ReactNative.Reflection;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using ReactNative.Views.Text;
using System;
#if WINDOWS_UWP
using Windows.Foundation;
using Windows.UI.Text;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
#else
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Media;
#endif

namespace ReactNative.Views.TextInput
{
    /// <summary>
    /// This extension of <see cref="LayoutShadowNode"/> is responsible for
    /// measuring the layout for Native <see cref="TextBox"/>.
    /// </summary>
    public class ReactTextInputShadowNode : LayoutShadowNode
    {
        private const int Unset = -1;
        private const int DefaultBorderWidth = 2;

        private static readonly float[] s_defaultPaddings =
        {
            10f,
            3f,
            6f,
            5f,
        };

        private bool _multiline;
        private bool _autoGrow;
        private int _letterSpacing;
        private int _numberOfLines;

        private double _fontSize = Unset;
        private double _lineHeight;
        private double? _maxHeight;

        private FontStyle? _fontStyle;
        private FontWeight? _fontWeight;
#if WINDOWS_UWP
        private TextAlignment _textAlignment = TextAlignment.DetectFromContent;
#else
        private TextAlignment _textAlignment = TextAlignment.Left;
#endif

        private string _fontFamily;
        private string _text;

        private int _jsEventCount = Unset;

        /// <summary>
        /// Instantiates the <see cref="ReactTextInputShadowNode"/>.
        /// </summary>
        public ReactTextInputShadowNode()
        {
            SetDefaultPadding(EdgeSpacing.Left, s_defaultPaddings[0]);
            SetDefaultPadding(EdgeSpacing.Top, s_defaultPaddings[1]);
            SetDefaultPadding(EdgeSpacing.Right, s_defaultPaddings[2]);
            SetDefaultPadding(EdgeSpacing.Bottom, s_defaultPaddings[3]);
            SetBorder(EdgeSpacing.All, DefaultBorderWidth);
            MeasureFunction = (node, width, widthMode, height, heightMode) => 
                MeasureTextInput(this, node, width, widthMode, height, heightMode);
        }

        /// <summary>
        /// Sets the text for the node.
        /// </summary>
        /// <param name="text">The text.</param>
        [ReactProp("text")]
        public void SetText(string text)
        {
            _text = text ?? "";
            MarkUpdated();
        }

        /// <summary>
        /// Sets the font size for the node.
        /// </summary>
        /// <param name="fontSize">The font size.</param>
        [ReactProp(ViewProps.FontSize, DefaultDouble = Unset)]
        public void SetFontSize(double fontSize)
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
#if WINDOWS_UWP
                _fontWeight.Value.Weight != fontWeight.Value.Weight))
#else
                _fontWeight.Value.ToOpenTypeWeight() != fontWeight.Value.ToOpenTypeWeight()))
#endif
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
        public void SetLineHeight(double lineHeight)
        {
            if (_lineHeight != lineHeight)
            {
                _lineHeight = lineHeight;
                MarkUpdated();
            }
        }

        /// <summary>
        /// Sets the max height.
        /// </summary>
        /// <param name="maxHeight">The max height.</param>
        [ReactProp(ViewProps.MaxHeight)]
        public override void SetMaxHeight(JValue maxHeight)
        {
            var maxHeightValue = maxHeight.Value<double?>();
            if (_maxHeight != maxHeightValue)
            {
                _maxHeight = maxHeightValue;
                MarkUpdated();
            }
        }

        /// <summary>
        /// Sets the maximum number of lines.
        /// </summary>
        /// <param name="numberOfLines">Max number of lines.</param>
        [ReactProp(ViewProps.NumberOfLines)]
        public void SetNumberOfLines(int numberOfLines)
        {
            if (_numberOfLines != numberOfLines)
            {
                _numberOfLines = numberOfLines;
                MarkUpdated();
            }
        }

        /// <summary>
        /// Sets whether to enable multiline input on the text input.
        /// </summary>
        /// <param name="multiline">The multiline flag.</param>
        [ReactProp("multiline")]
        public void SetMultiline(bool multiline)
        {
            if (_multiline != multiline)
            {
                _multiline = multiline;
                MarkUpdated();
            }
        }

        /// <summary>
        /// Sets whether to enable auto-grow on the text input.
        /// </summary>
        /// <param name="autoGrow">The auto-grow flag.</param>
        [ReactProp("autoGrow")]
        public void SetAutoGrow(bool autoGrow)
        {
            if (_autoGrow != autoGrow)
            {
                _autoGrow = autoGrow;
                if (!_autoGrow)
                {
                    MarkUpdated();
                }
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
#if WINDOWS_UWP
                TextAlignment.DetectFromContent :
#else
                TextAlignment.Left :
#endif
                EnumHelpers.Parse<TextAlignment>(textAlign);

            if (_textAlignment != textAlignment)
            {
                _textAlignment = textAlignment;
                MarkUpdated();
            }
        }

        /// <summary>
        /// Set the most recent event count in JavaScript.
        /// </summary>
        /// <param name="mostRecentEventCount">The event count.</param>
        [ReactProp("mostRecentEventCount")]
        public void SetMostRecentEventCount(int mostRecentEventCount)
        {
            _jsEventCount = mostRecentEventCount;
        }

        /// <summary>
        /// Called to aggregate the current text and event counter.
        /// </summary>
        /// <param name="uiViewOperationQueue">The UI operation queue.</param>
        public override void OnCollectExtraUpdates(UIViewOperationQueue uiViewOperationQueue)
        {
            base.OnCollectExtraUpdates(uiViewOperationQueue);

            var computedPadding = GetComputedPadding();

            if (computedPadding != null)
            {
                uiViewOperationQueue.EnqueueUpdateExtraData(ReactTag, computedPadding);
            }

            if (_jsEventCount != Unset)
            {
                uiViewOperationQueue.EnqueueUpdateExtraData(ReactTag, Tuple.Create(_jsEventCount, _text));
            }
        }

        /// <summary>
        /// Sets the paddings of the shadow node.
        /// </summary>
        /// <param name="index">The spacing type index.</param>
        /// <param name="padding">The padding value.</param>
        public override void SetPaddings(int index, JValue padding)
        {
            MarkUpdated();
            base.SetPaddings(index, padding);
        }

        /// <summary>
        /// Marks a node as updated.
        /// </summary>
        protected override void MarkUpdated()
        {
            base.MarkUpdated();
            dirty();
        }

        private float[] GetComputedPadding()
        {
            return new[]
            {
                GetPadding(YogaEdge.Left),
                GetPadding(YogaEdge.Top),
                GetPadding(YogaEdge.Right),
                GetPadding(YogaEdge.Bottom),
            };
        }

        private static YogaSize MeasureTextInput(ReactTextInputShadowNode textInputNode, YogaNode node, float width, YogaMeasureMode widthMode, float height, YogaMeasureMode heightMode)
        {
            var normalizedWidth = Math.Max(0,
                (YogaConstants.IsUndefined(width) ? double.PositiveInfinity : width));

            var normalizedHeight = Math.Max(0,
                (YogaConstants.IsUndefined(height) ? double.PositiveInfinity : height));

            var normalizedText = string.IsNullOrEmpty(textInputNode._text) ? " " : textInputNode._text;

            var textBlock = new TextBlock
            {
                Text = normalizedText,
                TextWrapping = textInputNode._multiline ? TextWrapping.Wrap : TextWrapping.NoWrap,
            };

            ApplyStyles(textInputNode, textBlock);

            textBlock.Measure(new Size(normalizedWidth, normalizedHeight));

            return MeasureOutput.Make(
                (float)Math.Ceiling(width),
                (float)Math.Ceiling(textBlock.ActualHeight));
        }

        private static void ApplyStyles(ReactTextInputShadowNode textNode, TextBlock textBlock)
        {
            if (textNode._fontSize != Unset)
            {
                var fontSize = textNode._fontSize;
                textBlock.FontSize = fontSize;
            }

            if (textNode._fontStyle.HasValue)
            {
                var fontStyle = textNode._fontStyle.Value;
                textBlock.FontStyle = fontStyle;
            }

            if (textNode._fontWeight.HasValue)
            {
                var fontWeight = textNode._fontWeight.Value;
                textBlock.FontWeight = fontWeight;
            }

            if (textNode._fontFamily != null)
            {
                var fontFamily = new FontFamily(textNode._fontFamily);
                textBlock.FontFamily = fontFamily;
            }

            if (textNode._maxHeight.HasValue)
            {
                textBlock.MaxHeight = textNode._maxHeight.Value;
            }
        }
    }
}
