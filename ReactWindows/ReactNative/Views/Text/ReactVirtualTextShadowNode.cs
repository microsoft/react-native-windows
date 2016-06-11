using ReactNative.Reflection;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using System.Collections.Generic;
using Windows.UI.Text;
using Windows.UI.Xaml.Documents;
using Windows.UI.Xaml.Media;

namespace ReactNative.Views.Text
{
    public class ReactVirtualTextShadowNode : ReactTextInlineShadowNode
    {
        private double? _fontSize;
        private int _letterSpacing;

        private FontStyle? _fontStyle;
        private FontWeight? _fontWeight;

        private string _fontFamily;

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

        public override Inline MakeInline(IList<Inline> children)
        {
            var span = new Span();
            UpdateInline(span);

            foreach (var child in children)
            {
                span.Inlines.Add(child);
            }

            return span;
        }

        public override void UpdateInline(Inline inline)
        {
            if (_letterSpacing != inline.CharacterSpacing)
            {
                inline.CharacterSpacing = _letterSpacing;
            }

            if (_fontSize.HasValue && inline.FontSize != _fontSize.Value)
            {
                inline.FontSize = _fontSize.Value;
            }

            if (_fontStyle.HasValue && inline.FontStyle != _fontStyle.Value)
            {
                inline.FontStyle = _fontStyle.Value;
            }

            if (_fontWeight.HasValue && inline.FontWeight.Weight != _fontWeight.Value.Weight)
            {
                inline.FontWeight = _fontWeight.Value;
            }

            if (_fontFamily != null && inline.FontFamily.Source != _fontFamily)
            {
                inline.FontFamily = new FontFamily(_fontFamily);
            }
        }
    }
}
