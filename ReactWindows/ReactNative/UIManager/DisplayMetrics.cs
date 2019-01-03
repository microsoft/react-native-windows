// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using ReactNative.Modules.DeviceInfo;
using Windows.Graphics.Display;
using Windows.UI.ViewManagement;

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

        public static DisplayMetrics Empty { get; } = new DisplayMetrics(0, 0, 0);

        public static DisplayMetrics GetForDeviceView(DeviceViewInfo viewInfo)
        {
            var bounds = viewInfo.ApplicationView.VisibleBounds;
            var scale = viewInfo.DisplayInformation.RawPixelsPerViewPixel;
            return new DisplayMetrics(bounds.Width, bounds.Height, scale);
        }

        public static DisplayMetrics GetForCurrentView()
        {
            var bounds = ApplicationView.GetForCurrentView().VisibleBounds;
            var scale = DisplayInformation.GetForCurrentView().RawPixelsPerViewPixel;
            return new DisplayMetrics(bounds.Width, bounds.Height, scale);
        }
    }
}
