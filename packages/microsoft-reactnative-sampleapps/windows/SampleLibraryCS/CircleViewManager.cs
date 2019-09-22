// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Collections.Generic;

using Windows.UI.Xaml;
using Windows.UI.Xaml.Shapes;

using Microsoft.ReactNative.Bridge;
using Microsoft.ReactNative.Managed;

namespace SampleLibraryCS
{
    // Example using IManagedViewManager with attributes (recommended)
    public sealed class CircleViewManager : IManagedViewManager
    {
        public string Name => "Circle";

        public FrameworkElement CreateView() => new Ellipse();

        [ViewManagerPropertySetter(Name = "radius")]
        public static void SetRadius(Ellipse circle, double? value)
        {
            if (value.HasValue)
            {
                circle.Width = value.Value * 2.0;
                circle.Height = value.Value * 2.0;
            }
            else
            {
                circle.ClearValue(FrameworkElement.WidthProperty);
                circle.ClearValue(FrameworkElement.HeightProperty);
            }
        }
    }

    // Example using IViewManager directly (ABI-safe but not recommended)
    public sealed class CircleViewManagerABI : IViewManager
    {
        public string Name => "CircleABI";

        public FrameworkElement CreateView() => new Ellipse();

        public IReadOnlyDictionary<string, string> NativeProps => _nativeProps ?? (_nativeProps = new Dictionary<string, string>(1)
        {
            { "radius", "double" },
        });
        private IReadOnlyDictionary<string, string> _nativeProps;

        public void UpdateProperties(FrameworkElement view, IReadOnlyDictionary<string, object> propertyMap)
        {
            if (view is Ellipse circle)
            {
                foreach (var property in propertyMap)
                {
                    switch (property.Key)
                    {
                        case "radius":
                            if (property.Value is double propertyValue)
                            {
                                circle.Width = propertyValue * 2.0;
                                circle.Height = propertyValue * 2.0;
                            }
                            else if (null == property.Value)
                            {
                                circle.ClearValue(FrameworkElement.WidthProperty);
                                circle.ClearValue(FrameworkElement.HeightProperty);
                            }
                            break;
                    }
                }
            }
        }
    }
}
