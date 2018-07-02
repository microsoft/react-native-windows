using Facebook.Yoga;
using ReactNative.UIManager;

namespace ReactNative.Views.DatePicker
{
    /// <summary>
    /// The shadow node implementation for <see cref="ReactNative.Views.DatePicker.ReactCalendarDatePickerManager" /> views.
    /// </summary>
    public class ReactCalendarDatePickerShadowNode : LayoutShadowNode
    {
        /// <summary>
        /// Instantiates the <see cref="ReactCalendarDatePickerShadowNode"/>.
        /// </summary>
        public ReactCalendarDatePickerShadowNode()
        {
            MeasureFunction = MeasurePicker;
        }

        private static YogaSize MeasurePicker(YogaNode node, float width, YogaMeasureMode widthMode, float height, YogaMeasureMode heightMode)
        {
            return MeasureOutput.Make(width, 32f);
        }
    }
}
