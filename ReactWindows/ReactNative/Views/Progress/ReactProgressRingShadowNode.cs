using ReactNative.UIManager;
using Facebook.CSSLayout;
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
            MeasureFunction = MeasureActivityIndicator;
        }

        /// <summary>
        /// Measures the width and height of a <see cref="ProgressRing"/>.
        /// </summary>
        /// <param name="node">The css style of the rendered <see cref="ProgressRing"/>.</param>
        /// <param name="width">The parameterized native width of the control.</param>
        /// <param name="height">The parameterized native height of the control.</param>
        /// <returns></returns>
        private static MeasureOutput MeasureActivityIndicator(CSSNode node, float width, float height)
        {
            if (double.IsNaN(height))
            {
                height = 6;//default height of xaml progressbar
            }
            if (double.IsNaN(width))
            {
                width = 6;//default height of xaml progressbar
            }


            return new MeasureOutput(width, height);
        }
    }
}
