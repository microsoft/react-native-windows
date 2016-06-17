using Facebook.CSSLayout;
using ReactNative.UIManager;
using Windows.UI.Xaml.Controls;

namespace ReactNative.Views.Progress
{
    /// <summary>
    /// Represents a native <see cref="ProgressRing"/> component.
    /// </summary>
    class ReactProgressRingShadowNode : LayoutShadowNode
    {
        /// <summary>
        /// Initializes an instance of a <see cref="ReactProgressRingShadowNode"/>
        /// </summary>
        public ReactProgressRingShadowNode()
        {
            MeasureFunction = MeasureProgressRing;
        }

        /// <summary>
        /// Measures the width and height of a <see cref="ProgressRing"/>.
        /// </summary>
        /// <param name="node">The css style of the rendered <see cref="ProgressRing"/>.</param>
        /// <param name="width">The parameterized native width of the control.</param>
        /// <param name="widthMode">The width measurement mode.</param>
        /// <param name="height">The parameterized native height of the control.</param>
        /// <param name="heightMode">The height measurement mode.</param>
        /// <returns>The measurement <see cref="MeasureOutput"/> for the <see cref="ProgressRing"/> component.</returns>
        private static MeasureOutput MeasureProgressRing(CSSNode node, float width, CSSMeasureMode widthMode, float height, CSSMeasureMode heightMode)
        {
            var normalizedWidth = !CSSConstants.IsUndefined(width) ? width : 20;
            var normalizedHeight = !CSSConstants.IsUndefined(height) ? height : 20;
            return new MeasureOutput(normalizedWidth, normalizedHeight);
        }
    }
}