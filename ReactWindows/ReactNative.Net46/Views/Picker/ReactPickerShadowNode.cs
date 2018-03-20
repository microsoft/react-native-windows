// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Facebook.Yoga;
using ReactNative.UIManager;

namespace ReactNative.Views.Picker
{
    /// <summary>
    /// The shadow node implementation for Picker views.
    /// </summary>
    public class ReactPickerShadowNode : LayoutShadowNode
    {
        /// <summary>
        /// Instantiates the <see cref="ReactPickerShadowNode"/>.
        /// </summary>
        public ReactPickerShadowNode()
        {
            MeasureFunction = MeasurePicker;
        }

        private static YogaSize MeasurePicker(YogaNode node, float width, YogaMeasureMode widthMode, float height, YogaMeasureMode heightMode)
        {
            return MeasureOutput.Make(width, 40f);
        }
    }
}
