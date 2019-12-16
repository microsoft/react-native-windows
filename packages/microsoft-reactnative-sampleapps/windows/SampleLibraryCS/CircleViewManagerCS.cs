// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Data;

using Microsoft.ReactNative.Bridge;

namespace SampleLibraryCS
{
    class CircleViewManagerCS : IViewManager, IViewManagerWithChildren
    {
        #region IViewManager

        public string Name => "CircleCS";

        public FrameworkElement CreateView()
        {
            Border view = new Border();

            Binding binding = new Binding
            {
                Source = view,
                Path = new PropertyPath("Height"),
                Converter = HeightToCornerRadiusConverter.Instance,
            };

            view.SetBinding(Border.CornerRadiusProperty, binding);

            return view;
        }

        #endregion

        #region IViewManagerWithChildren

        public void AddView(FrameworkElement parent, UIElement child, long index)
        {
            if (parent is Border border)
            {
                border.Child = child;
            }
        }

        public void RemoveAllChildren(FrameworkElement parent)
        {
            if (parent is Border border)
            {
                border.Child = null;
            }
        }

        public void RemoveChildAt(FrameworkElement parent, long index)
        {
            if (parent is Border border)
            {
                border.Child = null;
            }
        }

        public void ReplaceChild(FrameworkElement parent, UIElement oldChild, UIElement newChild)
        {
            if (parent is Border border)
            {
                border.Child = newChild;
            }
        }

        #endregion
    }

    class HeightToCornerRadiusConverter : IValueConverter
    {
        public static readonly IValueConverter Instance = new HeightToCornerRadiusConverter();

        public object Convert(object value, Type targetType, object parameter, string language)
        {
            if (value is double d && !double.IsNaN(d) && targetType == typeof(CornerRadius))
            {
                return new CornerRadius(d);
            }

            return new CornerRadius();
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            return value;
        }
    }
}
