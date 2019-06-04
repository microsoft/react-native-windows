// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Json;
using System.Collections.Generic;
#if WINDOWS_UWP
using Windows.UI.Xaml.Automation.Peers;
#endif

namespace ReactNative.UIManager
{
    /// <summary>
    /// Prop keys for React views.
    /// </summary>
    public static class ViewProps
    {
#pragma warning disable CS1591
        public const string ViewClassName = "RCTView";

        // Layout only (only affect positions of children, causes no drawing)
        // !!! Keep in sync with s_layoutOnlyProps below !!!
        public const string AlignItems = "alignItems";
        public const string AlignSelf = "alignSelf";
        public const string AlignContent = "alignContent";
        public const string Display = "display";
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
        public const string MarginStart = "marginStart";
        public const string MarginEnd = "marginEnd";

        public const string Padding = "padding";
        public const string PaddingVertical = "paddingVertical";
        public const string PaddingHorizontal = "paddingHorizontal";
        public const string PaddingLeft = "paddingLeft";
        public const string PaddingRight = "paddingRight";
        public const string PaddingTop = "paddingTop";
        public const string PaddingBottom = "paddingBottom";
        public const string PaddingStart = "paddingStart";
        public const string PaddingEnd = "paddingEnd";

        public const string Position = "position";
        public const string Right = "right";
        public const string Top = "top";
        public const string Width = "width";
        public const string Start = "start";
        public const string End = "end";

        public const string MinWidth = "minWidth";
        public const string MaxWidth = "maxWidth";
        public const string MinHeight = "minHeight";
        public const string MaxHeight = "maxHeight";

        public const string AspectRatio = "aspectRatio";

        // Props that sometimes may prevent us from collapsing views
        public const string PointerEvents = "pointerEvents";
        public const string Auto = "auto";
        public const string BoxNone = "box-none";

        // Props that affect more than just layout
        public const string Disabled = "disabled";
        public const string BackgroundColor = "backgroundColor";
        public const string Color = "color";
        public const string FontSize = "fontSize";
        public const string FontWeight = "fontWeight";
        public const string FontStyle = "fontStyle";
        public const string FontFamily = "fontFamily";
        public const string LetterSpacing = "letterSpacing";
        public const string LineHeight = "lineHeight";
        public const string NumberOfLines = "numberOfLines";
        public const string Value = "value";
        public const string ResizeMode = "resizeMode";
        public const string TextAlign = "textAlign";
        public const string TextAlignVertical = "textAlignVertical";
        public const string TextDecorationLine = "textDecorationLine";
        public const string Opacity = "opacity";
        public const string Overflow = "overflow";

        public const string Hidden = "hidden";
        public const string Visible = "visible";

        public const string AccessibilityTraits = "accessibilityTraits";
        public const string AccessibilityLabel = "accessibilityLabel";
        public const string ImportantForAccessibility = "importantForAccessibility";
        public const string AccessibilityLiveRegion = "accessibilityLiveRegion";

        public const string AllowFontScaling = "allowFontScaling";

        public const string BorderWidth = "borderWidth";
        public const string BorderLeftWidth = "borderLeftWidth";
        public const string BorderStartWidth = "borderStartWidth";
        public const string BorderEndWidth = "borderEndWidth";
        public const string BorderTopWidth = "borderTopWidth";
        public const string BorderRightWidth = "borderRightWidth";
        public const string BorderBottomWidth = "borderBottomWidth";
        public const string BorderRadius = "borderRadius";
        public const string BorderTopLeftRadius = "borderTopLeftRadius";
        public const string BorderTopRightRadius = "borderTopRightRadius";
        public const string BorderBottomLeftRadius = "borderBottomLeftRadius";
        public const string BorderBottomRightRadius = "borderBottomRightRadius";
        public const string BorderColor = "borderColor";
        public const string BorderTopStartRadius = "borderTopStartRadius";
        public const string BorderTopEndRadius = "borderTopEndRadius";
        public const string BorderBottomStartRadius = "borderBottomStartRadius";
        public const string BorderBottomEndRadius = "borderBottomEndRadius";
        public const string OnLayout = "onLayout";

#pragma warning restore CS1591

        /// <summary>
        /// Ordered list of margin spacing types.
        /// </summary>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Security", "CA2104:DoNotDeclareReadOnlyMutableReferenceTypes", Justification = "IReadOnlyList is immutable.")]
        public static readonly IReadOnlyList<int> BorderSpacingTypes =
            new List<int>
            {
                EdgeSpacing.All,
                EdgeSpacing.Top,
                EdgeSpacing.Bottom,
                EdgeSpacing.Left,
                EdgeSpacing.Right,
            };

        /// <summary>
        /// Ordered list of border spacing types.
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
                EdgeSpacing.Left,
                EdgeSpacing.Right,
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
                EdgeSpacing.Left,
                EdgeSpacing.Right,
                EdgeSpacing.Top,
                EdgeSpacing.Bottom,
            };

        private static readonly HashSet<string> s_layoutOnlyProps =
            new HashSet<string>
            {
                AlignSelf,
                AlignItems,
                Collapsible,
                Flex,
                FlexBasis,
                FlexDirection,
                FlexGrow,
                FlexShrink,
                FlexWrap,
                JustifyContent,
                AlignContent,
                Display,

                /* position */
                Position,
                Right,
                Top,
                Bottom,
                Left,
                Start,
                End,

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
                MarginStart,
                MarginEnd,

                /* paddings */
                Padding,
                PaddingVertical,
                PaddingHorizontal,
                PaddingLeft,
                PaddingRight,
                PaddingTop,
                PaddingBottom,
                PaddingStart,
                PaddingEnd,
            };

        /// <summary>
        /// Checks if the prop key is layout-only.
        /// </summary>
        /// <param name="props">The prop collection.</param>
        /// <param name="prop">The prop name.</param>
        /// <returns>
        /// <b>true</b> if the prop is layout-only, <b>false</b> otherwise.
        /// </returns>
        public static bool IsLayoutOnly(JObject props, string prop)
        {
            if (s_layoutOnlyProps.Contains(prop))
            {
                return true;
            }
            else if (prop == PointerEvents)
            {
                var value = props.Value<string>(prop);
                return value == Auto || value == BoxNone;
            }

            // These are more aggressive optimizations based on property values.
            // In RN Android there is a runtime check here. We omitted it because
            // the check didn't inspire confidence in the optimizations that must be
            // either correct or not.
            {
                var value = props[prop];

                switch (prop)
                {
                    case Opacity:
                        // null opacity behaves like opacity = 1
                        // Ignore if explicitly set to default opacity.
                        return value == null || value.Value<double>() == 1.0;
                    case BorderWidth:
                        return value == null || value.Value<double>() == 0.0;
                    case BorderLeftWidth:
                        return value == null || value.Value<double>() == 0.0;
                    case BorderTopWidth:
                        return value == null || value.Value<double>() == 0.0;
                    case BorderRightWidth:
                        return value == null || value.Value<double>() == 0.0;
                    case BorderBottomWidth:
                        return value == null || value.Value<double>() == 0.0;
                    case Overflow:
                        return value == null || value.Value<string>() == Visible;
                    case AccessibilityTraits:
                        return value == null || value is JArray array && array.Count == 0;
                    case AccessibilityLabel:
                        return value == null || value.Type == JTokenType.String && value.Value<string>().Length == 0;
                    case ImportantForAccessibility:
                        return value == null || value.Type == JTokenType.String &&
                            (value.Value<string>().Length == 0 || value.Value<string>() == "auto");
#if WINDOWS_UWP
                    case AccessibilityLiveRegion:
                        return value == null || value.Type == JTokenType.String &&
                            (value.Value<string>().Length == 0 || value.Value<string>() == "off");
#endif
                }
            }

            return false;
        }
    }
}
