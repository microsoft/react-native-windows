// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;

namespace ReactNative.UIManager
{
    interface IPropSetter
    {
        string Name { get; }

        string PropType { get; }

        void UpdateShadowNodeProp(ReactShadowNode shadowNode, JObject value);

        void UpdateViewManagerProp(IViewManager viewManager, object view, JObject value);
    }
}
