using Facebook.Yoga;

namespace ReactNative.UIManager
{
    static class ReactShadowNodeExtensions
    {
        public static float GetLeftBorderWidth(this ReactShadowNode node)
        {
            var width = node.GetBorder(EdgeSpacing.Left);
            if (!YogaConstants.IsUndefined(width))
            {
                return width;
            }

            width = node.GetBorder(EdgeSpacing.Horizontal);
            if (!YogaConstants.IsUndefined(width))
            {
                return width;
            }

            width = node.GetBorder(EdgeSpacing.All);
            if (!YogaConstants.IsUndefined(width))
            {
                return width;
            }

            return 0.0f;
        }

        public static float GetPaddingValue(this ReactShadowNode node, int spacingType)
        {
            var padding = node.GetPadding(spacingType);
            if (!YogaConstants.IsUndefined(padding))
            {
                return padding;
            }

            if (spacingType == EdgeSpacing.Left || spacingType == EdgeSpacing.Right)
            {
                padding = node.GetPadding(EdgeSpacing.Horizontal);
            }

            if (!YogaConstants.IsUndefined(padding))
            {
                return padding;
            }

            if (spacingType == EdgeSpacing.Top || spacingType == EdgeSpacing.Bottom)
            {
                padding = node.GetPadding(EdgeSpacing.Vertical);
            }

            if (!YogaConstants.IsUndefined(padding))
            {
                return padding;
            }

            return node.GetPadding(EdgeSpacing.All);
        }

        public static float GetPaddingSpace(this ReactShadowNode node, int spacingType)
        {
            var padding = node.GetPaddingValue(spacingType);
            return YogaConstants.IsUndefined(padding)
                ? 0.0f
                : padding;
        }

        public static float GetTopBorderWidth(this ReactShadowNode node)
        {
            var width = node.GetBorder(EdgeSpacing.Top);
            if (!YogaConstants.IsUndefined(width))
            {
                return width;
            }

            width = node.GetBorder(EdgeSpacing.Vertical);
            if (!YogaConstants.IsUndefined(width))
            {
                return width;
            }

            width = node.GetBorder(EdgeSpacing.All);
            if (!YogaConstants.IsUndefined(width))
            {
                return width;
            }

            return 0.0f;
        }
    }
}
