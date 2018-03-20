// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#if WINDOWS_UWP
using Windows.UI.Xaml;
#else
using System.Windows;
#endif

namespace ReactNative.UIManager
{
    interface IPropertySetter
    {
        string Name { get; }

        string PropertyType { get; }

        void UpdateShadowNodeProperty(ReactShadowNode shadowNode, ReactStylesDiffMap value);

        void UpdateViewManagerProperty(IViewManager viewManager, DependencyObject view, ReactStylesDiffMap value);
    }
}
