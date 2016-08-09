using Facebook.CSSLayout;
using ReactNative.Bridge;
using ReactNative.Reflection;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using ReactNative.Views.Text;
using System;
using Windows.Foundation;
using Windows.UI.Text;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Documents;
using Windows.UI.Xaml.Media;

namespace ReactNative.Views.TextInput
{
    /// <summary>
    /// This extension of <see cref="LayoutShadowNode"/> is responsible for 
    /// measuring the layout for Native <see cref="TextBox"/>.
    /// </summary>
    public class ReactTextInputShadowNode : LayoutShadowNode
    {
        private const int Unset = -1;

        private float[] _computedPadding;

        private int _letterSpacing;
        private int _numberOfLines;

        private double _fontSize = Unset;
        private double _lineHeight;

        private FontStyle? _fontStyle;
        private FontWeight? _fontWeight;
        private TextAlignment _textAlignment = TextAlignment.DetectFromContent;

        private string _fontFamily;
        private string _text;

        private int _jsEventCount = Unset;

        /// <summary>
        /// Instantiates the <see cref="ReactTextInputShadowNode"/>.
        /// </summary>
        public ReactTextInputShadowNode()
        {
            var computedPadding = GetDefaultPaddings();
            SetPadding(CSSSpacingType.Left, computedPadding[0]);
            SetPadding(CSSSpacingType.Top, computedPadding[1]);
            SetPadding(CSSSpacingType.Right, computedPadding[2]);
            SetPadding(CSSSpacingType.Bottom, computedPadding[3]);
            MeasureFunction = MeasureTextInput;
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
        /// Marks a node as updated.
        /// </summary>
        protected override void MarkUpdated()
        {
            base.MarkUpdated();
            dirty();
        }

        private float[] GetDefaultPaddings()
        {
            // TODO: calculate dynamically
            return new[]
            {
                10f,
                3f,
                6f,
                5f,
            };
        }

        private float[] GetComputedPadding()
        {
            return new float[]
            {
                GetPadding(CSSSpacingType.Left),
                GetPadding(CSSSpacingType.Top),
                GetPadding(CSSSpacingType.Right),
                GetPadding(CSSSpacingType.Bottom),
            };
        }

        private static MeasureOutput MeasureTextInput(CSSNode node, float width, CSSMeasureMode widthMode, float height, CSSMeasureMode heightMode)
        {
            var textInputNode = (ReactTextInputShadowNode)node;
            textInputNode._computedPadding = textInputNode.GetComputedPadding();

            var borderLeftWidth = textInputNode.GetBorder(CSSSpacingType.Left);
            var borderRightWidth = textInputNode.GetBorder(CSSSpacingType.Right);

            var normalizedWidth = Math.Max(0,
                (CSSConstants.IsUndefined(width) ? double.PositiveInfinity : width)
                - textInputNode._computedPadding[0]
                - textInputNode._computedPadding[2]
                - (CSSConstants.IsUndefined(borderLeftWidth) ? 0 : borderLeftWidth)
                - (CSSConstants.IsUndefined(borderRightWidth) ? 0 : borderRightWidth));
            var normalizedHeight = Math.Max(0, CSSConstants.IsUndefined(height) ? double.PositiveInfinity : height);

            // This is not a terribly efficient way of projecting the height of
            // the text elements. It requires that we have access to the
            // dispatcher in order to do measurement, which, for obvious
            // reasons, can cause perceived performance issues as it will block
            // the UI thread from handling other work.
            //
            // TODO: determine another way to measure text elements.
            var task = DispatcherHelpers.CallOnDispatcher(() =>
            {
                var textNode = (ReactTextInputShadowNode)node;

                var textBlock = new TextBlock
                {
                    TextWrapping = TextWrapping.Wrap,
                };

                var normalizedText = string.IsNullOrEmpty(textNode._text) ? " " : textNode._text;
                var inline = new Run { Text = normalizedText };
                FormatInline(textNode, inline, true);

                textBlock.Inlines.Add(inline);

                textBlock.Measure(new Size(normalizedWidth, normalizedHeight));

                var borderTopWidth = textInputNode.GetBorder(CSSSpacingType.Top);
                var borderBottomWidth = textInputNode.GetBorder(CSSSpacingType.Bottom);

                var finalizedHeight = (float)textBlock.DesiredSize.Height;
                finalizedHeight += textInputNode._computedPadding[1];
                finalizedHeight += textInputNode._computedPadding[3];
                finalizedHeight += CSSConstants.IsUndefined(borderTopWidth) ? 0 : borderTopWidth;
                finalizedHeight += CSSConstants.IsUndefined(borderBottomWidth) ? 0 : borderBottomWidth;

                return new MeasureOutput(width, finalizedHeight);
            });

            return task.Result;
        }

        /// <summary>
        /// Formats an inline instance with shadow properties.
        /// </summary>
        /// <param name="textNode">The text shadow node.</param>
        /// <param name="inline">The inline.</param>
        /// <param name="measureOnly">Signals if the operation is used only for measurement.</param>
        protected static void FormatInline(ReactTextInputShadowNode textNode, Inline inline, bool measureOnly)
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
