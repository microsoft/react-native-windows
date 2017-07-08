using Facebook.Yoga;
using ReactNative.UIManager;

namespace ReactNative.Views.DatePicker
{
    /// <summary>
    /// The shadow node implementation for TimePicker views.
    /// </summary>
    public class ReactTimePickerShadowNode : LayoutShadowNode
    {
        /// <summary>
        /// Instantiates the <see cref="ReactTimePickerShadowNode"/> 
        /// </summary>
        public ReactTimePickerShadowNode()
        {
            MeasureFunction = MeasurePicker;
        }

        private static YogaSize MeasurePicker(YogaNode node, float width, YogaMeasureMode widthMode, float height, YogaMeasureMode heightMode)
        {
            return MeasureOutput.Make(width, 40f);
        }
    }
}
