using Facebook.Yoga;
using ReactNative.UIManager;

namespace ReactNative.Views.DatePicker
{
    /// <summary>
    /// The shadow node implementation for <see cref="ReactNative.Views.DatePicker.ReactDateTimePickerManager" /> views.
    /// </summary>
    public class ReactDateTimePickerShadowNode : LayoutShadowNode
    {
        /// <summary>
        /// Instantiates the <see cref="ReactDatePickerShadowNode"/>
        /// </summary>
        public ReactDateTimePickerShadowNode()
        {
            MeasureFunction = MeasurePicker;
        }

        private static YogaSize MeasurePicker(YogaNode node, float width, YogaMeasureMode widthMode, float height, YogaMeasureMode heightMode)
        {
            return MeasureOutput.Make(width, 80f);
        }
    }
}
