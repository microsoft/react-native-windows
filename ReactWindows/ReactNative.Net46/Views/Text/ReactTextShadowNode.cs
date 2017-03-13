using Facebook.Yoga;
using ReactNative.Bridge;
using ReactNative.Reflection;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using System;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

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
        private TextAlignment _textAlignment = TextAlignment.Left;

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
                _fontWeight.Value != fontWeight.Value))
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
                TextAlignment.Left :
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

        private static YogaSize MeasureText(ReactTextShadowNode textNode, YogaNode node, float width, YogaMeasureMode widthMode, float height, YogaMeasureMode heightMode)
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
                var textBlock = new TextBlock
                {
                    TextAlignment = TextAlignment.Left,
                    TextWrapping = TextWrapping.Wrap,
                    TextTrimming = TextTrimming.CharacterEllipsis,
                };

                textNode.UpdateTextBlockCore(textBlock, true);

                for (var i = 0; i < textNode.ChildCount; ++i)
                {
                    var child = textNode.GetChildAt(i);
                    textBlock.Inlines.Add(ReactInlineShadowNodeVisitor.Apply(child));
                }

                var normalizedWidth = YogaConstants.IsUndefined(width) ? double.PositiveInfinity : width;
                var normalizedHeight = YogaConstants.IsUndefined(height) ? double.PositiveInfinity : height;
                textBlock.Measure(new Size(normalizedWidth, normalizedHeight));
                return MeasureOutput.Make(
                    (float)Math.Ceiling(textBlock.DesiredSize.Width),
                    (float)Math.Ceiling(textBlock.DesiredSize.Height));
            });

            return task.Result;
        }

        /// <summary>
        /// Updates the properties of a <see cref="TextBlock"/> view.
        /// </summary>
        /// <param name="textBlock">The view.</param>
        public void UpdateTextBlock(TextBlock textBlock)
        {
            UpdateTextBlockCore(textBlock, false);
        }

        private void UpdateTextBlockCore(TextBlock textBlock, bool measureOnly)
        {
            //textBlock.CharacterSpacing = _letterSpacing;
            //textBlock.MaxLines = _numberOfLines;
            textBlock.LineHeight = _lineHeight != 0 ? _lineHeight : double.NaN;
            textBlock.TextAlignment = _textAlignment;
            textBlock.FontSize = _fontSize ?? 15;
            textBlock.FontStyle = _fontStyle ?? new FontStyle();
            textBlock.FontWeight = _fontWeight ?? FontWeights.Normal;

            if (_fontFamily != null)
            {
                // convert font string into something WPF can use
                // https://msdn.microsoft.com/en-us/library/ms753303(v=vs.110).aspx
                // e.g. FontFamily(new System.Uri("pack://application:,,,/"), "./Assets/#fontname")
                string[] path = _fontFamily.Split('/');
                path = path.Take(path.Count() - 1).ToArray();
                string cleanPath = "./" + string.Join("/", path) + "/";
                string[] fontParts = _fontFamily.Split('#');
                textBlock.FontFamily = new FontFamily(new System.Uri("pack://application:,,,/"), cleanPath + "#" + fontParts.Last());
            }
            else
            {
                textBlock.FontFamily = new FontFamily();
            }

            if (!measureOnly)
            {
                textBlock.Padding = new Thickness(
                    GetPadding(YogaEdge.Left),
                    GetPadding(YogaEdge.Top),
                    0,
                    0);
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
