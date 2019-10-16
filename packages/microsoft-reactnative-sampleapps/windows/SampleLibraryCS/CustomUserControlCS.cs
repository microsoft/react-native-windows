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

        public static void OnLabelChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            if (d is CustomUserControlCS control)
            {

            }
        }

        static CustomUserControlCS()
        {
            LabelProperty = DependencyProperty.Register(
                nameof(Label),
                typeof(string),
                typeof(CustomUserControlCS),
                new PropertyMetadata(default(string), new PropertyChangedCallback(OnLabelChanged))
                );
        }

        public CustomUserControlCS()
        {
            this.DefaultStyleKey = typeof(CustomUserControlCS);
        }
    }
}
