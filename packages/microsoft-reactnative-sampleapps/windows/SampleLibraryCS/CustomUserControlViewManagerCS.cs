// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Controls;

using Microsoft.ReactNative.Managed;

namespace SampleLibraryCS
{
    internal class CustomUserControlViewManagerCS : AttributedViewManager<CustomUserControlCS>
    {
        [ViewManagerPropertySetter("label")]
        public void SetLabel(CustomUserControlCS view, string value)
        {
            if (null != value)
            {
                view.SetValue(CustomUserControlCS.LabelProperty, value);
            }
            else
            {
                view.ClearValue(CustomUserControlCS.LabelProperty);
            }
        }

        [ViewManagerPropertySetter("color")]
        public void SetColor(CustomUserControlCS view, Brush value)
        {
            if (null != value)
            {
                view.SetValue(Control.ForegroundProperty, value);
            }
            else
            {
                view.ClearValue(Control.ForegroundProperty);
            }
        }

        [ViewManagerPropertySetter("backgroundColor")]
        public void SetBackgroundColor(CustomUserControlCS view, Brush value)
        {
            if (null != value)
            {
                view.SetValue(Control.BackgroundProperty, value);
            }
            else
            {
                view.ClearValue(Control.BackgroundProperty);
            }
        }
    }
}
