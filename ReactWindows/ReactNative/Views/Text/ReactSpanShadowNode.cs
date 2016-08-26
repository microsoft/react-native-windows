using ReactNative.Reflection;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using System.Collections.Generic;
using Windows.UI.Text;
using Windows.UI.Xaml.Documents;
using Windows.UI.Xaml.Media;

namespace ReactNative.Views.Text
{
    /// <summary>
    /// Shadow node for virtual text nodes.
    /// </summary>
    public class ReactSpanShadowNode : ReactInlineShadowNode
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
        /// Create the <see cref="Span"/> instance for the measurement calculation.
        /// </summary>
        /// <param name="children">The children.</param>
        /// <returns>The instance.</returns>
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

        /// <summary>
        /// Update the properties on the inline instance.
        /// </summary>
        /// <param name="inline">The instance.</param>
        public override void UpdateInline(Inline inline)
        {
            inline.CharacterSpacing = _letterSpacing;
            inline.FontSize = _fontSize ?? 15;
            inline.FontStyle = _fontStyle ?? FontStyle.Normal;
            inline.FontWeight = _fontWeight ?? FontWeights.Normal;
            inline.FontFamily = _fontFamily != null ? new FontFamily(_fontFamily) : FontFamily.XamlAutoFontFamily;
        }

        /// <summary>
        /// This method will be called by <see cref="UIManagerModule"/> once
        /// per batch, before calculating layout. This will only be called for
        /// nodes that are marked as updated with <see cref="ReactShadowNode.MarkUpdated"/> or
        /// require layout (i.e., marked with <see cref="ReactShadowNode.dirty"/> ).
        /// </summary>
        public override void OnBeforeLayout()
        {
            // Run flexbox on the children which are inline views.
            foreach (var child in this.Children)
            {
                if (!(child is ReactInlineShadowNode))
                {
                    child.CalculateLayout();
                }
            }
        }
    }
}
