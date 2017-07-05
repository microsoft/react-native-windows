using Facebook.Yoga;
using ReactNative.UIManager;

namespace ReactNative.Views.DatePicker
{
    /// <summary>
    /// The shadow node implementation for DatePicker views.
    /// </summary>
    public class ReactDatePickerShadowNode : LayoutShadowNode
    {
        /// <summary>
        /// INstantiates the <see cref="ReactDatePickerShadowNode"/> 
        /// </summary>
        public ReactDatePickerShadowNode()
        {
            MeasureFunction = MeasurePicker;
        }

        private static YogaSize MeasurePicker(YogaNode node, float width, YogaMeasureMode widthMode, float height, YogaMeasureMode heightMode)
        {
            return MeasureOutput.Make(width, 40f);
        }
    }
}
