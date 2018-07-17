// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using ReactNative.Reflection;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using System.Collections.Generic;
#if WINDOWS_UWP
using Windows.UI.Text;
using Windows.UI.Xaml.Documents;
using Windows.UI.Xaml.Media;
#else
using System.Windows;
using System.Windows.Documents;
using System.Windows.Media;
#endif

namespace ReactNative.Views.Text
{
    /// <summary>
    /// Shadow node for virtual text nodes.
    /// </summary>
    public class ReactSpanShadowNode : ReactInlineShadowNode
    {
        private double? _fontSize;
        private int? _letterSpacing;

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
#if WINDOWS_UWP
                _fontWeight.Value.Weight != fontWeight.Value.Weight))
#else
                _fontWeight.Value != fontWeight.Value))
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
        public void SetLetterSpacing(int? letterSpacing)
        {
            if (_letterSpacing != letterSpacing)
            {
                _letterSpacing = letterSpacing;
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
#if WINDOWS_UWP
            if (_letterSpacing.HasValue)
            {
                var spacing = 50 * _letterSpacing.Value; // TODO: Find exact multiplier (50) to match iOS
                inline.CharacterSpacing = spacing;
            }
            else
            {
                inline.ClearValue(Inline.CharacterSpacingProperty);
            }
#endif
            if (_fontStyle.HasValue)
            {
                inline.FontStyle = _fontStyle.Value;
            }
            else
            {
                inline.ClearValue(Inline.FontStyleProperty);
            }
            
            if (!string.IsNullOrEmpty(_fontFamily))
            {
                inline.FontFamily = new FontFamily(_fontFamily);
            }
            else
            {
                inline.ClearValue(Inline.FontFamilyProperty);
            }

            if (_fontSize.HasValue)
            {
                inline.FontSize = _fontSize.Value;
            }
            else
            {
                inline.ClearValue(Inline.FontSizeProperty);
            }

            if (_fontWeight.HasValue)
            {
                inline.FontWeight = _fontWeight.Value;
            }
            else
            {
                inline.ClearValue(Inline.FontWeightProperty);
            }
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
