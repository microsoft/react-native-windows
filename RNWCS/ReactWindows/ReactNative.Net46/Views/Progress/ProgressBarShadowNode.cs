// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Facebook.Yoga;
using ReactNative.UIManager;

namespace ReactNative.Views.Progress
{
    class ProgressBarShadowNode : LayoutShadowNode
    {
        public ProgressBarShadowNode()
        {
            MeasureFunction = MeasureProgressBar;
        }

        private static YogaSize MeasureProgressBar(YogaNode node, float width, YogaMeasureMode widthMode, float height, YogaMeasureMode heightMode)
        {
            var adjustedHeight = YogaConstants.IsUndefined(height) ? 4f : height; 
            return MeasureOutput.Make(width, adjustedHeight);
        }
    }
}
