using Facebook.Yoga;
using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
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
using Windows.UI.Xaml.Documents;
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

        private static readonly float[] s_defaultPaddings =
        {
            10f,
            3f,
            6f,
            5f,
        };

        private float[] _computedPadding;

        private int _letterSpacing;
        private int _numberOfLines;

        private double _fontSize = Unset;
        private double _lineHeight;

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
            SetDefaultPadding(EdgeSpacing.Start, s_defaultPaddings[0]);
            SetDefaultPadding(EdgeSpacing.Top, s_defaultPaddings[1]);
            SetDefaultPadding(EdgeSpacing.End, s_defaultPaddings[2]);
            SetDefaultPadding(EdgeSpacing.Bottom, s_defaultPaddings[3]);
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

            if (_computedPadding != null)
            {
                uiViewOperationQueue.EnqueueUpdateExtraData(ReactTag, _computedPadding);
                _computedPadding = null;
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
                GetPadding(YogaEdge.Start),
                GetPadding(YogaEdge.Top),
                GetPadding(YogaEdge.End),
                GetPadding(YogaEdge.Bottom),
            };
        }

        private static YogaSize MeasureTextInput(ReactTextInputShadowNode textInputNode, YogaNode node, float width, YogaMeasureMode widthMode, float height, YogaMeasureMode heightMode)
        {
            textInputNode._computedPadding = textInputNode.GetComputedPadding();

            var borderLeftWidth = textInputNode.GetBorder(YogaEdge.Left);
            var borderRightWidth = textInputNode.GetBorder(YogaEdge.Right);

            var normalizedWidth = Math.Max(0,
                (YogaConstants.IsUndefined(width) ? double.PositiveInfinity : width)
                - textInputNode._computedPadding[0]
                - textInputNode._computedPadding[2]
                - (YogaConstants.IsUndefined(borderLeftWidth) ? 0 : borderLeftWidth)
                - (YogaConstants.IsUndefined(borderRightWidth) ? 0 : borderRightWidth));
            var normalizedHeight = Math.Max(0, YogaConstants.IsUndefined(height) ? double.PositiveInfinity : height);

            // This is not a terribly efficient way of projecting the height of
            // the text elements. It requires that we have access to the
            // dispatcher in order to do measurement, which, for obvious
            // reasons, can cause perceived performance issues as it will block
            // the UI thread from handling other work.
            //
            // TODO: determine another way to measure text elements.
            var task = DispatcherHelpers.CallOnDispatcher(() =>
            {
                var textBlock = new TextBlock
                {
                    TextWrapping = TextWrapping.Wrap,
                };

                var normalizedText = string.IsNullOrEmpty(textInputNode._text) ? " " : textInputNode._text;
                var inline = new Run { Text = normalizedText };
                FormatInline(textInputNode, inline);

                textBlock.Inlines.Add(inline);

                textBlock.Measure(new Size(normalizedWidth, normalizedHeight));

                var borderTopWidth = textInputNode.GetBorder(YogaEdge.Top);
                var borderBottomWidth = textInputNode.GetBorder(YogaEdge.Bottom);

                var finalizedHeight = textBlock.DesiredSize.Height;
                finalizedHeight += textInputNode._computedPadding[1];
                finalizedHeight += textInputNode._computedPadding[3];
                finalizedHeight += YogaConstants.IsUndefined(borderTopWidth) ? 0 : borderTopWidth;
                finalizedHeight += YogaConstants.IsUndefined(borderBottomWidth) ? 0 : borderBottomWidth;

                return MeasureOutput.Make(
                    (float)Math.Ceiling(width), 
                    (float)Math.Ceiling(finalizedHeight));
            });

            return task.Result;
        }

        /// <summary>
        /// Formats an inline instance with shadow properties.
        /// </summary>
        /// <param name="textNode">The text shadow node.</param>
        /// <param name="inline">The inline.</param>
        protected static void FormatInline(ReactTextInputShadowNode textNode, TextElement inline)
        {
            if (textNode._fontSize != Unset)
            {
                var fontSize = textNode._fontSize;
                inline.FontSize = fontSize;
            }

            if (textNode._fontStyle.HasValue)
            {
                var fontStyle = textNode._fontStyle.Value;
                inline.FontStyle = fontStyle;
            }

            if (textNode._fontWeight.HasValue)
            {
                var fontWeight = textNode._fontWeight.Value;
                inline.FontWeight = fontWeight;
            }

            if (textNode._fontFamily != null)
            {
                var fontFamily = new FontFamily(textNode._fontFamily);
                inline.FontFamily = fontFamily;
            }
        }
    }
}
