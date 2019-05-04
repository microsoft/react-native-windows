// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Facebook.Yoga;
using ReactNative.UIManager;

namespace ReactNative.Views.Switch
{
    /// <summary>
    /// Shadow node class for measuring <see cref="Windows.UI.Xaml.Controls.ToggleSwitch"/> instances.
    /// </summary>
    public class ReactSwitchShadowNode : LayoutShadowNode
    {
        /// <summary>
        /// Instantiates the <see cref="ReactSwitchShadowNode"/>.
        /// </summary>
        public ReactSwitchShadowNode()
        {
            MeasureFunction = MeasureSwitch;
        }

        private static YogaSize MeasureSwitch(YogaNode node, float width, YogaMeasureMode widthMode, float height, YogaMeasureMode heightMode)
        {
            // TODO: figure out how to properly measure the switch.
            // We are currently blocked until we switch to a UWP-specific React
            // JavaScript library as the iOS library we currently use specifies
            // an exact width and height for switch nodes.
            return MeasureOutput.Make(56f, 40f);
        }
    }
}
