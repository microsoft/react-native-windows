// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.UIManager;
using System;
using System.Collections.Generic;

namespace ReactNative.Tests
{
    class MockViewManager : IViewManager
    {
        public virtual IReadOnlyDictionary<string, object> CommandsMap
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        public virtual IReadOnlyDictionary<string, object> ExportedCustomBubblingEventTypeConstants
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        public virtual IReadOnlyDictionary<string, object> ExportedCustomDirectEventTypeConstants
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        public virtual IReadOnlyDictionary<string, object> ExportedViewConstants
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        public virtual string Name
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        public virtual IReadOnlyDictionary<string, string> NativeProps
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        public virtual Type ShadowNodeType
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        public virtual ReactShadowNode CreateShadowNodeInstance()
        {
            throw new NotImplementedException();
        }

        public virtual object CreateView(ThemedReactContext reactContext)
        {
            throw new NotImplementedException();
        }

        public Dimensions GetDimensions(object view)
        {
            throw new NotImplementedException();
        }

        public virtual void OnDropViewInstance(ThemedReactContext reactContext, object view)
        {
            throw new NotImplementedException();
        }

        public virtual void ReceiveCommand(object view, int commandId, JArray args)
        {
            throw new NotImplementedException();
        }

        public void SetDimensions(object view, Dimensions dimensions)
        {
            throw new NotImplementedException();
        }

        public virtual void UpdateExtraData(object root, object extraData)
        {
            throw new NotImplementedException();
        }

        public virtual void UpdateProps(object viewToUpdate, JObject props)
        {
            throw new NotImplementedException();
        }
    }
}
