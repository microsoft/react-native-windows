// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace ReactNative.UIManager
{
    interface IPropertySetter
    {
        string Name { get; }

        string PropertyType { get; }

        void UpdateShadowNodeProperty(ReactShadowNode shadowNode, ReactStylesDiffMap value);

        void UpdateViewManagerProperty(IViewManager viewManager, object view, ReactStylesDiffMap value);
    }
}
