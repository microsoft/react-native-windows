// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

using Microsoft.ReactNative.Managed;
using Microsoft.ReactNative.Bridge;

namespace SampleLibraryCS
{
    class StackPanelViewManager : AttributedViewManagerWithChildren<StackPanel>
    {
        public StackPanelViewManager(IReactContext reactContext) : base(reactContext) { }

        [ViewManagerExportedViewConstant]
        public Orientation OrientationVertical = Orientation.Vertical;

        [ViewManagerExportedViewConstant]
        public Orientation OrientationHorizontal = Orientation.Horizontal;


        [ViewManagerProperty("orientation")]
        public void SetOrientation(StackPanel view, Orientation? value)
        {
            if (value.HasValue)
            {
                view.SetValue(StackPanel.OrientationProperty, value);
            }
            else
            {
                view.ClearValue(StackPanel.OrientationProperty);
            }
        }

        public override IList<UIElement> GetChildren(StackPanel parent)
        {
            return parent.Children;
        }
    }
}
