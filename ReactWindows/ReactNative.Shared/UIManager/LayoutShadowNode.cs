// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Facebook.Yoga;
using Newtonsoft.Json.Linq;
using ReactNative.Reflection;
using ReactNative.UIManager.Annotations;
using System;
using System.Globalization;
using static System.FormattableString;

namespace ReactNative.UIManager
{
    /// <summary>
    /// Shadow node subclass that supplies setters for base view layout
    /// props such as width, height, flex props, borders, etc.
    /// </summary>
    public class LayoutShadowNode : ReactShadowNode
    {
        /// <summary>
        /// Instantiates a <see cref="LayoutShadowNode"/>.
        /// </summary>
        public LayoutShadowNode()
        {
        }

        /// <summary>
        /// Instantiates a <see cref="LayoutShadowNode"/>. 
        /// </summary>
        /// <param name="isVirtual">
        /// <code>true</code> if the node is virtual, otherwise <code>false</code>.
        /// </param>
        /// <param name="isDelegatedLayout">
        /// <code>true</code> if the node delegates the layout to the child.
        /// </param>
        public LayoutShadowNode(bool isVirtual, bool isDelegatedLayout)
            : base(isVirtual, isDelegatedLayout)
        {
        }

        /// <summary>
        /// Set the width of the shadow node.
        /// </summary>
        /// <param name="width">The width.</param>
        [ReactProp(ViewProps.Width, DefaultSingle = YogaConstants.Undefined)]
        public void SetWidth(JValue width)
        {
            if (IsVirtual || IsDelegatedLayout)
            {
                return;
            }

            StyleWidth = ToYogaValue(width);
        }

        /// <summary>
        /// Sets the minimum width of the shadow node.
        /// </summary>
        /// <param name="minWidth">The minimum width.</param>
        [ReactProp(ViewProps.MinWidth, DefaultSingle = YogaConstants.Undefined)]
        public void SetMinWidth(JValue minWidth)
        {
            if (IsVirtual || IsDelegatedLayout)
            {
                return;
            }

            StyleMinWidth = ToYogaValue(minWidth);
        }

        /// <summary>
        /// Sets the maximum width of the shadow node.
        /// </summary>
        /// <param name="maxWidth">The maximum width.</param>
        [ReactProp(ViewProps.MaxWidth, DefaultSingle = YogaConstants.Undefined)]
        public void SetMaxWidth(JValue maxWidth)
        {
            if (IsVirtual || IsDelegatedLayout)
            {
                return;
            }

            StyleMaxWidth = ToYogaValue(maxWidth);
        }

        /// <summary>
        /// Set the heigth of the shadow node.
        /// </summary>
        /// <param name="height">The height.</param>
        [ReactProp(ViewProps.Height, DefaultSingle = YogaConstants.Undefined)]
        public void SetHeight(JValue height)
        {
            if (IsVirtual || IsDelegatedLayout)
            {
                return;
            }

            StyleHeight = ToYogaValue(height);
        }

        /// <summary>
        /// Sets the minimum height of the shadow node.
        /// </summary>
        /// <param name="minHeight">The minimum height.</param>
        [ReactProp(ViewProps.MinHeight, DefaultSingle = YogaConstants.Undefined)]
        public void SetMinHeight(JValue minHeight)
        {
            if (IsVirtual || IsDelegatedLayout)
            {
                return;
            }

            StyleMinHeight = ToYogaValue(minHeight);
        }

        /// <summary>
        /// Sets the maximum height of the shadow node.
        /// </summary>
        /// <param name="maxHeight">The maximum height.</param>
        [ReactProp(ViewProps.MaxHeight, DefaultSingle = YogaConstants.Undefined)]
        public virtual void SetMaxHeight(JValue maxHeight)
        {
            if (IsVirtual || IsDelegatedLayout)
            {
                return;
            }

            StyleMaxHeight = ToYogaValue(maxHeight);
        }

        /// <summary>
        /// Sets the flex of the shadow node.
        /// </summary>
        /// <param name="flex">The flex value.</param>
        [ReactProp(ViewProps.Flex, DefaultSingle = 0f)]
        public void SetFlex(float flex)
        {
            if (IsVirtual || IsDelegatedLayout)
            {
                return;
            }

            Flex = flex;
        }

        /// <summary>
        /// Sets the flex grow of the shadow node.
        /// </summary>
        /// <param name="flexGrow">The flex grow value.</param>
        [ReactProp(ViewProps.FlexGrow, DefaultSingle = 0f)]
        public void SetFlexGrow(float flexGrow)
        {
            if (IsVirtual || IsDelegatedLayout)
            {
                return;
            }

            FlexGrow = flexGrow;
        }

        /// <summary>
        /// Sets the flex shrink of the shadow node.
        /// </summary>
        /// <param name="flexShrink">The flex shrink value.</param>
        [ReactProp(ViewProps.FlexShrink, DefaultSingle = 0f)]
        public void SetFlexShrink(float flexShrink)
        {
            if (IsVirtual || IsDelegatedLayout)
            {
                return;
            }

            FlexShrink = flexShrink;
        }

        /// <summary>
        /// Sets the flex basis of the shadow node.
        /// </summary>
        /// <param name="flexBasis">The flex basis value.</param>
        [ReactProp(ViewProps.FlexBasis, DefaultSingle = 0f)]
        public void SetFlexBasis(float flexBasis)
        {
            if (IsVirtual || IsDelegatedLayout)
            {
                return;
            }

            FlexBasis = flexBasis;
        }

        /// <summary>
        /// Sets the aspect ratio of the shadow node.
        /// </summary>
        /// <param name="aspectRatio">The aspect ratio.</param>
        [ReactProp(ViewProps.AspectRatio, DefaultSingle = YogaConstants.Undefined)]
        public void SetAspectRatio(float aspectRatio)
        {
            if (IsVirtual || IsDelegatedLayout)
            {
                return;
            }

            StyleAspectRatio = aspectRatio;
        }

        /// <summary>
        /// Sets the flex direction of the shadow node.
        /// </summary>
        /// <param name="flexDirection">The flex direction.</param>
        [ReactProp(ViewProps.FlexDirection)]
        public void SetFlexDirection(string flexDirection)
        {
            if (IsVirtual || IsDelegatedLayout)
            {
                return;
            }

            FlexDirection = EnumHelpers.ParseNullable<YogaFlexDirection>(flexDirection) ?? YogaFlexDirection.Column;
        }

        /// <summary>
        /// Sets the wrap prop on the shadow node.
        /// </summary>
        /// <param name="flexWrap">The wrap.</param>
        [ReactProp(ViewProps.FlexWrap)]
        public void SetFlexWrap(string flexWrap)
        {
            if (IsVirtual || IsDelegatedLayout)
            {
                return;
            }

            FlexWrap = EnumHelpers.ParseNullable<YogaWrap>(flexWrap) ?? YogaWrap.NoWrap;
        }

        /// <summary>
        /// Sets the self alignment of the shadow node.
        /// </summary>
        /// <param name="alignSelf">The align self prop.</param>
        [ReactProp(ViewProps.AlignSelf)]
        public void SetAlignSelf(string alignSelf)
        {
            if (IsVirtual || IsDelegatedLayout)
            {
                return;
            }

            AlignSelf = EnumHelpers.ParseNullable<YogaAlign>(alignSelf) ?? YogaAlign.Auto;
        }

        /// <summary>
        /// Sets the item alignment for the shadow node.
        /// </summary>
        /// <param name="alignItems">The item alignment.</param>
        [ReactProp(ViewProps.AlignItems)]
        public void SetAlignItems(string alignItems)
        {
            if (IsVirtual || IsDelegatedLayout)
            {
                return;
            }

            AlignItems = EnumHelpers.ParseNullable<YogaAlign>(alignItems) ?? YogaAlign.Stretch;
        }

        /// <summary>
        /// Sets the content alignment.
        /// </summary>
        /// <param name="alignContent">The content alignment.</param>
        [ReactProp(ViewProps.AlignContent)]
        public void SetAlignContent(string alignContent)
        {
            if (IsVirtual || IsDelegatedLayout)
            {
                return;
            }

            AlignContent = EnumHelpers.ParseNullable<YogaAlign>(alignContent) ?? YogaAlign.FlexStart;
        }

        /// <summary>
        /// Sets the content justification.
        /// </summary>
        /// <param name="justifyContent">The content justification.</param>
        [ReactProp(ViewProps.JustifyContent)]
        public void SetJustifyContent(string justifyContent)
        {
            if (IsVirtual || IsDelegatedLayout)
            {
                return;
            }

            JustifyContent = EnumHelpers.ParseNullable<YogaJustify>(justifyContent) ?? YogaJustify.FlexStart;
        }

        /// <summary>
        /// Sets the overflow of the shadow node.
        /// </summary>
        /// <param name="overflow">The overflow</param>
        [ReactProp(ViewProps.Overflow)]
        public void SetOverflow(string overflow)
        {
            if (IsVirtual || IsDelegatedLayout)
            {
                return;
            }

            Overflow = EnumHelpers.ParseNullable<YogaOverflow>(overflow) ?? YogaOverflow.Visible;
        }

        /// <summary>
        /// Sets the display mode.
        /// </summary>
        /// <param name="display">The display mode.</param>
        [ReactProp(ViewProps.Display)]
        public void SetDisplay(string display)
        {
            if (IsVirtual || IsDelegatedLayout)
            {
                return;
            }

            Display = EnumHelpers.ParseNullable<YogaDisplay>(display) ?? YogaDisplay.Flex;
        }

        /// <summary>
        /// Sets the margins of the shadow node.
        /// </summary>
        /// <param name="index">The spacing type index.</param>
        /// <param name="margin">The margin value.</param>
        [ReactPropGroup(
            ViewProps.Margin,
            ViewProps.MarginVertical,
            ViewProps.MarginHorizontal,
            ViewProps.MarginStart,
            ViewProps.MarginEnd,
            ViewProps.MarginTop,
            ViewProps.MarginBottom,
            ViewProps.MarginLeft,
            ViewProps.MarginRight,
            DefaultSingle = YogaConstants.Undefined)]
        public void SetMargins(int index, JValue margin)
        {
            if (IsVirtual || IsDelegatedLayout)
            {
                return;
            }

            SetMargin(ViewProps.PaddingMarginSpacingTypes[index], ToYogaValue(margin));
        }

        /// <summary>
        /// Sets the paddings of the shadow node.
        /// </summary>
        /// <param name="index">The spacing type index.</param>
        /// <param name="padding">The padding value.</param>
        [ReactPropGroup(
            ViewProps.Padding,
            ViewProps.PaddingVertical,
            ViewProps.PaddingHorizontal,
            ViewProps.PaddingStart,
            ViewProps.PaddingEnd,
            ViewProps.PaddingTop,
            ViewProps.PaddingBottom,
            ViewProps.PaddingLeft,
            ViewProps.PaddingRight,
            DefaultSingle = YogaConstants.Undefined)]
        public virtual void SetPaddings(int index, JValue padding)
        {
            if (IsVirtual || IsDelegatedLayout)
            {
                return;
            }

            SetPadding(ViewProps.PaddingMarginSpacingTypes[index], ToYogaValue(padding));
        }

        /// <summary>
        /// Sets the border width props for the shadow node.
        /// </summary>
        /// <param name="index">The border spacing type index.</param>
        /// <param name="borderWidth">The border width.</param>
        [ReactPropGroup(
            ViewProps.BorderWidth,
            ViewProps.BorderLeftWidth,
            ViewProps.BorderRightWidth,
            ViewProps.BorderTopWidth,
            ViewProps.BorderBottomWidth,
            DefaultSingle = YogaConstants.Undefined)]
        public void SetBorderWidth(int index, float borderWidth)
        {
            if (IsVirtual || IsDelegatedLayout)
            {
                return;
            }

            SetBorder(ViewProps.BorderSpacingTypes[index], borderWidth);
        }

        /// <summary>
        /// Sets the position of the shadow node.
        /// </summary>
        /// <param name="index">The spacing type index.</param>
        /// <param name="position">The position value.</param>
        [ReactPropGroup(
            ViewProps.Start,
            ViewProps.End,
            ViewProps.Left,
            ViewProps.Right,
            ViewProps.Top,
            ViewProps.Bottom,
            DefaultSingle = YogaConstants.Undefined)]
        public void SetPositionValues(int index, JValue position)
        {
            if (IsVirtual || IsDelegatedLayout)
            {
                return;
            }

            SetPosition(ViewProps.PositionSpacingTypes[index], ToYogaValue(position));
        }

        /// <summary>
        /// Sets the position of the shadow node.
        /// </summary>
        /// <param name="position">The position.</param>
        [ReactProp(ViewProps.Position)]
        public void SetPosition(string position)
        {
            if (IsVirtual || IsDelegatedLayout)
            {
                return;
            }
            
            PositionType = EnumHelpers.ParseNullable<YogaPositionType>(position) ?? YogaPositionType.Relative;
        }

        /// <summary>
        /// Sets if the view should send an event on layout.
        /// </summary>
        /// <param name="shouldNotifyOnLayout">
        /// The flag signaling if the view should sent an event on layout.
        /// </param>
        [ReactProp(ViewProps.OnLayout)]
        public void SetShouldNotifyOnLayout(bool shouldNotifyOnLayout)
        {
            ShouldNotifyOnLayout = shouldNotifyOnLayout;
        }

        private static YogaValue ToYogaValue(JValue value)
        {
            if (value == null || value.Type == JTokenType.Null || value.Type == JTokenType.Undefined)
            {
                return YogaValue.Undefined();
            }

            if (value.Type == JTokenType.String)
            {
                var s = value.Value<string>();

                if (s == "auto")
                {
                    return YogaValue.Auto();
                }

                if (s.EndsWith("%"))
                {
                    return YogaValue.Percent(float.Parse(s.Substring(0, s.Length - 1), CultureInfo.InvariantCulture));
                }

                throw new InvalidOperationException(
                    Invariant($"Unknown value: '{s}'"));
            }

            return YogaValue.Point(value.Value<float>());
        }
    }
}
