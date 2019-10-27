// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Diagnostics;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Controls;

using Microsoft.ReactNative.Managed;
using System.Collections.Generic;

namespace SampleLibraryCS
{
    internal class CustomUserControlViewManagerCS : AttributedViewManager<CustomUserControlCS>
    {
        [ViewManagerProperty("label")]
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

        [ViewManagerProperty("color")]
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

        [ViewManagerProperty("backgroundColor")]
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

        [ViewManagerCommand]
        public void CustomCommand(CustomUserControlCS view, IReadOnlyList<object> args)
        {
            Debug.WriteLine($"{Name}.{nameof(CustomCommand)}({args[0].ToString()})");
        }
    }
}
