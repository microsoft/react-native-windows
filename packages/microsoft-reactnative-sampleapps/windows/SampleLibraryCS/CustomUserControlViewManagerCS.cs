// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Diagnostics;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Controls;

using Microsoft.ReactNative.Managed;
using Microsoft.ReactNative.Bridge;
using Windows.UI.Xaml;

namespace SampleLibraryCS
{
    internal class CustomUserControlViewManagerCS : AttributedViewManager<CustomUserControlCS>
    {
        public CustomUserControlViewManagerCS(IReactContext reactContext) : base(reactContext) { }

        public override FrameworkElement CreateView()
        {
            var view = new CustomUserControlCS();
            view.RegisterPropertyChangedCallback(CustomUserControlCS.LabelProperty, (obj, prop) =>
            {
                if (obj is CustomUserControlCS c)
                {
                    LabelChanged(c, c.Label);
                }
            });

            return view;
        }

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
        public void CustomCommand(CustomUserControlCS view, string arg)
        {
            Debug.WriteLine($"{Name}.{nameof(CustomCommand)}({view.Tag}, \"{arg}\")");
        }

        [ViewManagerExportedDirectEventTypeConstant]
        public ViewManagerEvent<CustomUserControlCS, string> LabelChanged;
    }
}
