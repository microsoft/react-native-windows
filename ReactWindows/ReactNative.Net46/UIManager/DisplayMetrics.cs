// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Windows;

namespace ReactNative.UIManager
{
    class DisplayMetrics
    {
        private DisplayMetrics(double width, double height, double scale)
        {
            Width = width;
            Height = height;
            Scale = scale;
        }

        public double Width { get; }

        public double Height { get; }

        public double Scale { get; }

        public static DisplayMetrics GetForCurrentView()
        {
            var content = (FrameworkElement)Application.Current.MainWindow.Content;
            return new DisplayMetrics(
                content?.ActualWidth ?? 0.0,
                content?.ActualHeight ?? 0.0,
                1.0);
        }
    }
}
