// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
// TODO

using Facebook.Yoga;
using ReactNative.UIManager;

namespace ReactNative.Views.Slider
{
    /// <summary>
    /// The shadow node implementation for Slider views.
    /// </summary>
    public class ReactSliderShadowNode : LayoutShadowNode
    {
        /// <summary>
        /// Instantiates the <see cref="ReactSliderShadowNode"/>.
        /// </summary>
        public ReactSliderShadowNode()
        {
            MeasureFunction = MeasurePicker;
        }

        private static YogaSize MeasurePicker(YogaNode node, float width, YogaMeasureMode widthMode, float height, YogaMeasureMode heightMode)
        {
            return MeasureOutput.Make(width, 40f);
        }
    }
}
