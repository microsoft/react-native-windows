using Facebook.Yoga;
using ReactNative.Modules.I18N;

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

            if (!I18NUtil.IsRightToLeft)
            {
                width = node.GetBorder(YogaEdge.Start);
                if (!YogaConstants.IsUndefined(width))
                {
                    return width;
                }
            }
            else
            {
                width = node.GetBorder(YogaEdge.End);
                if (!YogaConstants.IsUndefined(width))
                {
                    return width;
                }
            }

            width = node.GetBorder(YogaEdge.All);
            if (!YogaConstants.IsUndefined(width))
            {
                return width;
            }

            return 0.0f;
        }

        public static float GetTopBorderWidth(this ReactShadowNode node)
        {
            var width = node.GetBorder(YogaEdge.Top);
            if (!YogaConstants.IsUndefined(width))
            {
                return width;
            }

            width = node.GetBorder(YogaEdge.All);
            if (!YogaConstants.IsUndefined(width))
            {
                return width;
            }

            return 0.0f;
        }
    }
}
