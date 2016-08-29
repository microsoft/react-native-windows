using Facebook.CSSLayout;
using ReactNative.UIManager;
using Windows.UI.Xaml.Controls;

namespace ReactNative.Views.Slider
{
    /// <summary>
    /// The shadow node implementation for Picker views.
    /// </summary>
    public class ReactSliderShadowNode : LayoutShadowNode
    {
        /// <summary>
        /// Instantiates the <see cref="ReactPickerShadowNode"/>.
        /// </summary>
        public ReactSliderShadowNode()
        {
            MeasureFunction = MeasurePicker;
        }

        private static MeasureOutput MeasurePicker(CSSNode node, float width, CSSMeasureMode widthMode, float height, CSSMeasureMode heightMode)
        {
            return new MeasureOutput(width, 40);
        }
    }
}
