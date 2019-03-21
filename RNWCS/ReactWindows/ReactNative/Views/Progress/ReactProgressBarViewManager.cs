// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

namespace ReactNative.Views.Progress
{
    class ReactProgressBarViewManager : BaseViewManager<ProgressBar, ProgressBarShadowNode>
    {
        public override string Name
        {
            get
            {
                return "WindowsProgressBar";
            }
        }

        [ReactProp("indeterminate")]
        public void SetIndeterminate(ProgressBar view, bool value)
        {
            view.IsIndeterminate = value;
        }

        [ReactProp("progress")]
        public void SetProgress(ProgressBar view, double value)
        {
            view.Value = value;
        }

        [ReactProp(ViewProps.Color, CustomType = "Color")]
        public void SetColor(ProgressBar view, uint? color)
        {
            view.Foreground = color.HasValue
                ? new SolidColorBrush(ColorHelpers.Parse(color.Value))
                : null;
        }

        public override ProgressBarShadowNode CreateShadowNodeInstance()
        {
            return new ProgressBarShadowNode();
        }

        public override void UpdateExtraData(ProgressBar root, object extraData)
        {
        }

        protected override ProgressBar CreateViewInstance(ThemedReactContext reactContext)
        {
            return new ProgressBar();
        }
    }
}
