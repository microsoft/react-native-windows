using System.Collections.Generic;

namespace ReactNative.UIManager
{
    /// <summary>
    /// Property keys for React views.
    /// </summary>
    public static class ViewProps
    {
#pragma warning disable CS1591
        public const string ViewClassName = "RCTView";

        // Layout only (only affect positions of children, causes no drawing)
        // !!! Keep in sync with s_layoutOnlyProperties below !!!
        public const string AlignItems = "alignItems";
        public const string AlignSelf = "alignSelf";
        public const string Overflow = "overflow";
        public const string Bottom = "bottom";
        public const string Collapsible = "collapsable";
        public const string Flex = "flex";
        public const string FlexGrow = "flexGrow";
        public const string FlexShrink = "flexShrink";
        public const string FlexBasis = "flexBasis";
        public const string FlexDirection = "flexDirection";
        public const string FlexWrap = "flexWrap";
        public const string Height = "height";
        public const string JustifyContent = "justifyContent";
        public const string Left = "left";

        public const string Margin = "margin";
        public const string MarginVertical = "marginVertical";
        public const string MarginHorizontal = "marginHorizontal";
        public const string MarginLeft = "marginLeft";
        public const string MarginRight = "marginRight";
        public const string MarginTop = "marginTop";
        public const string MarginBottom = "marginBottom";

        public const string Padding = "padding";
        public const string PaddingVertical = "paddingVertical";
        public const string PaddingHorizontal = "paddingHorizontal";
        public const string PaddingLeft = "paddingLeft";
        public const string PaddingRight = "paddingRight";
        public const string PaddingTop = "paddingTop";
        public const string PaddingBottom = "paddingBottom";

        public const string Position = "position";
        public const string Right = "right";
        public const string Top = "top";
        public const string Width = "width";

        public const string MinWidth = "minWidth";
        public const string MaxWidth = "maxWidth";
        public const string MinHeight = "minHeight";
        public const string MaxHeight = "maxHeight";

        public const string AspectRatio = "aspectRatio";
      
        // Properties that affect more than just layout
        public const string Disabled = "disabled";
        public const string BackgroundColor = "backgroundColor";
        public const string Color = "color";
        public const string FontSize = "fontSize";
        public const string FontWeight = "fontWeight";
        public const string FontStyle = "fontStyle";
        public const string FontFamily = "fontFamily";
        public const string LetterSpacing = "letterSpacing";
        public const string LineHeight = "lineHeight";
        public const string NeedsOffScreenAlphaCompositing = "needsOffscreenAlphaCompositing";
        public const string NumberOfLines = "numberOfLines";
        public const string Value = "value";
        public const string ResizeMode = "resizeMode";
        public const string TextAlign = "textAlign";
        public const string TextAlignVertical = "textAlignVertical";
        public const string TextDecorationLine = "textDecorationLine";
        public const string AllowFontScaling = "allowFontScaling";

        public const string BorderWidth = "borderWidth";
        public const string BorderLeftWidth = "borderLeftWidth";
        public const string BorderTopWidth = "borderTopWidth";
        public const string BorderRightWidth = "borderRightWidth";
        public const string BorderBottomWidth = "borderBottomWidth";
        public const string BorderRadius = "borderRadius";
        public const string BorderTopLeftRadius = "borderTopLeftRadius";
        public const string BorderTopRightRadius = "borderTopRightRadius";
        public const string BorderBottomLeftRadius = "borderBottomLeftRadius";
        public const string BorderBottomRightRadius = "borderBottomRightRadius";

#pragma warning restore CS1591

        /// <summary>
        /// Ordered list of margin spacing types.
        /// </summary>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Security", "CA2104:DoNotDeclareReadOnlyMutableReferenceTypes", Justification = "IReadOnlyList is immutable.")]
        public static readonly IReadOnlyList<int> PaddingMarginSpacingTypes = 
            new List<int>
            {
                EdgeSpacing.All,
                EdgeSpacing.Vertical,
                EdgeSpacing.Horizontal,
                EdgeSpacing.Start,
                EdgeSpacing.End,
                EdgeSpacing.Top,
                EdgeSpacing.Bottom,
            };

        /// <summary>
        /// Ordered list of border spacing types.
        /// </summary>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Security", "CA2104:DoNotDeclareReadOnlyMutableReferenceTypes", Justification = "IReadOnlyList is immutable.")]
        public static readonly IReadOnlyList<int> BorderSpacingTypes =
            new List<int>
            {
                EdgeSpacing.All,
                EdgeSpacing.Start,
                EdgeSpacing.End,
                EdgeSpacing.Top,
                EdgeSpacing.Bottom,
            };

        /// <summary>
        /// Ordered list of position spacing types.
        /// </summary>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Security", "CA2104:DoNotDeclareReadOnlyMutableReferenceTypes", Justification = "IReadOnlyList is immutable.")]
        public static readonly IReadOnlyList<int> PositionSpacingTypes =
            new List<int>
            {
                EdgeSpacing.Start,
                EdgeSpacing.End,
                EdgeSpacing.Top,
                EdgeSpacing.Bottom,
            };

        private static readonly HashSet<string> s_layoutOnlyProperties =
            new HashSet<string>
            {
                AlignItems,
                AlignSelf,
                Collapsible,
                Flex,
                FlexDirection,
                FlexWrap,
                JustifyContent,
                Overflow,

                /* position */
                Position,
                Right,
                Top,
                Bottom,
                Left,

                /* dimensions */
                Width,
                Height,
                MinWidth,
                MaxWidth,
                MinHeight,
                MaxHeight,

                /* margins */
                Margin,
                MarginVertical,
                MarginHorizontal,
                MarginLeft,
                MarginRight,
                MarginTop,
                MarginBottom,
                
                /* paddings */
                Padding,
                PaddingVertical,
                PaddingHorizontal,
                PaddingLeft,
                PaddingRight,
                PaddingTop,
                PaddingBottom,
            };

        /// <summary>
        /// Checks if the property key is layout-only.
        /// </summary>
        /// <param name="key">The key.</param>
        /// <returns>
        /// <b>true</b> if the property is layout-only, <b>false</b> otherwise.
        /// </returns>
        public static bool IsLayoutOnly(string key)
        {
            return s_layoutOnlyProperties.Contains(key);
        }
    }
}