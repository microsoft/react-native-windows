// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SampleLibraryCS
{
    public sealed class CustomUserControlCS : Control
    {
        public static DependencyProperty LabelProperty { get; private set; }

        public string Label
        {
            get
            {
                return (string)GetValue(LabelProperty);
            }
            set
            {
                SetValue(LabelProperty, value);
            }
        }

        static CustomUserControlCS()
        {
            LabelProperty = DependencyProperty.Register(
                nameof(Label),
                typeof(string),
                typeof(CustomUserControlCS),
                new PropertyMetadata(default(string))
                );
        }

        public CustomUserControlCS()
        {
            DefaultStyleKey = typeof(CustomUserControlCS);
        }
    }
}
