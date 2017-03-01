using Facebook.Yoga;
using ReactNative.Reflection;
using ReactNative.UIManager.Annotations;

namespace ReactNative.UIManager
{
    /// <summary>
    /// Shadow node subclass that supplies setters for base view layout
    /// properties such as width, height, flex properties, borders, etc.
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
        public LayoutShadowNode(bool isVirtual)
            : base(isVirtual)
        {
        }

        /// <summary>
        /// Set the width of the shadow node.
        /// </summary>
        /// <param name="width">The width.</param>
        [ReactProp(ViewProps.Width, DefaultSingle = YogaConstants.Undefined)]
        public void SetWidth(float width)
        {
            if (IsVirtual)
            {
                return;
            }

            StyleWidth = width;
        }

        /// <summary>
        /// Sets the minimum width of the shadow node.
        /// </summary>
        /// <param name="minWidth">The minimum width.</param>
        [ReactProp(ViewProps.MinWidth, DefaultSingle = YogaConstants.Undefined)]
        public void SetMinWidth(float minWidth)
        {
            if (IsVirtual)
            {
                return;
            }

            StyleMinWidth = minWidth;
        }

        /// <summary>
        /// Sets the maximum width of the shadow node.
        /// </summary>
        /// <param name="maxWidth">The maximum width.</param>
        [ReactProp(ViewProps.MaxWidth, DefaultSingle = YogaConstants.Undefined)]
        public void SetMaxWidth(float maxWidth)
        {
            if (IsVirtual)
            {
                return;
            }

            StyleMaxWidth = maxWidth;
        }

        /// <summary>
        /// Set the heigth of the shadow node.
        /// </summary>
        /// <param name="height">The height.</param>
        [ReactProp(ViewProps.Height, DefaultSingle = YogaConstants.Undefined)]
        public void SetHeight(float height)
        {
            if (IsVirtual)
            {
                return;
            }

            StyleHeight = height;
        }

        /// <summary>
        /// Sets the minimum height of the shadow node.
        /// </summary>
        /// <param name="minHeight">The minimum height.</param>
        [ReactProp(ViewProps.MinHeight, DefaultSingle = YogaConstants.Undefined)]
        public void SetMinHeight(float minHeight)
        {
            if (IsVirtual)
            {
                return;
            }

            StyleMinHeight = minHeight;
        }

        /// <summary>
        /// Sets the maximum height of the shadow node.
        /// </summary>
        /// <param name="maxHeight">The maximum height.</param>
        [ReactProp(ViewProps.MaxHeight, DefaultSingle = YogaConstants.Undefined)]
        public void SetMaxHeight(float maxHeight)
        {
            if (IsVirtual)
            {
                return;
            }

            StyleMaxHeight = maxHeight;
        }

        /// <summary>
        /// Sets the flex of the shadow node.
        /// </summary>
        /// <param name="flex">The flex value.</param>
        [ReactProp(ViewProps.Flex, DefaultSingle = 0f)]
        public void SetFlex(float flex)
        {
            if (IsVirtual)
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
            if (IsVirtual)
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
            if (IsVirtual)
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
            if (IsVirtual)
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
            StyleAspectRatio = aspectRatio;
        }

        /// <summary>
        /// Sets the flex direction of the shadow node.
        /// </summary>
        /// <param name="flexDirection">The flex direction.</param>
        [ReactProp(ViewProps.FlexDirection)]
        public void SetFlexDirection(string flexDirection)
        {
            if (IsVirtual)
            {
                return;
            }

            FlexDirection = EnumHelpers.ParseNullable<YogaFlexDirection>(flexDirection) ?? YogaFlexDirection.Column;
        }

        /// <summary>
        /// Sets the wrap property on the shadow node.
        /// </summary>
        /// <param name="flexWrap">The wrap.</param>
        [ReactProp(ViewProps.FlexWrap)]
        public void SetFlexWrap(string flexWrap)
        {
            if (IsVirtual)
            {
                return;
            }

            FlexWrap = EnumHelpers.ParseNullable<YogaWrap>(flexWrap) ?? YogaWrap.NoWrap;
        }

        /// <summary>
        /// Sets the self alignment of the shadow node.
        /// </summary>
        /// <param name="alignSelf">The align self property.</param>
        [ReactProp(ViewProps.AlignSelf)]
        public void SetAlignSelf(string alignSelf)
        {
            if (IsVirtual)
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
            AlignItems = EnumHelpers.ParseNullable<YogaAlign>(alignItems) ?? YogaAlign.Stretch;
        }

        /// <summary>
        /// Sets the content justification.
        /// </summary>
        /// <param name="justifyContent">The content justification.</param>
        [ReactProp(ViewProps.JustifyContent)]
        public void SetJustifyContent(string justifyContent)
        {
            if (IsVirtual)
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
            if (IsVirtual)
            {
                return;
            }

            Overflow = EnumHelpers.ParseNullable<YogaOverflow>(overflow) ?? YogaOverflow.Visible;
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
            ViewProps.MarginLeft,
            ViewProps.MarginRight,
            ViewProps.MarginTop,
            ViewProps.MarginBottom,
            DefaultSingle = YogaConstants.Undefined)]
        public void SetMargins(int index, float margin)
        {
            if (IsVirtual)
            {
                return;
            }

            SetMargin(ViewProps.PaddingMarginSpacingTypes[index], margin);
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
            ViewProps.PaddingLeft,
            ViewProps.PaddingRight,
            ViewProps.PaddingTop,
            ViewProps.PaddingBottom,
            DefaultSingle = YogaConstants.Undefined)]
        public virtual void SetPaddings(int index, float padding)
        {
            SetPadding(ViewProps.PaddingMarginSpacingTypes[index], padding);
        }

        /// <summary>
        /// Sets the border width properties for the shadow node.
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
            SetBorder(ViewProps.BorderSpacingTypes[index], borderWidth);
        }

        /// <summary>
        /// Sets the position of the shadow node.
        /// </summary>
        /// <param name="index">The spacing type index.</param>
        /// <param name="position">The position value.</param>
        [ReactPropGroup(
            ViewProps.Left,
            ViewProps.Right,
            ViewProps.Top,
            ViewProps.Bottom,
            DefaultSingle = YogaConstants.Undefined)]
        public void SetPositionValues(int index, float position)
        {
            if (IsVirtual)
            {
                return;
            }

            SetPosition(ViewProps.PositionSpacingTypes[index], position);
        }

        /// <summary>
        /// Sets the position of the shadow node.
        /// </summary>
        /// <param name="position">The position.</param>
        [ReactProp(ViewProps.Position)]
        public void SetPosition(string position)
        {
            PositionType = EnumHelpers.ParseNullable<YogaPositionType>(position) ?? YogaPositionType.Relative;
        }

        /// <summary>
        /// Sets if the view should send an event on layout.
        /// </summary>
        /// <param name="shouldNotifyOnLayout">
        /// The flag signaling if the view should sent an event on layout.
        /// </param>
        [ReactProp("onLayout")]
        public void SetShouldNotifyOnLayout(bool shouldNotifyOnLayout)
        {
            ShouldNotifyOnLayout = shouldNotifyOnLayout;
        }
    }
}
