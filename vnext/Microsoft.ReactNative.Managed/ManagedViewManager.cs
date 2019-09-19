// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Reflection;

using Windows.UI.Xaml;

using Microsoft.ReactNative.Bridge;

namespace Microsoft.ReactNative.Managed
{
  public interface IManagedViewManager
  {
    string Name { get; }
    FrameworkElement CreateView();
  }

  public sealed class ManagedViewManager : FrameworkElementViewManager
  {
    public IManagedViewManager ViewManager { get; private set; }

    internal TypeInfo TypeInfo { get; private set; }

    internal Dictionary<string, System.Reflection.MethodInfo> PropertySetters
    {
      get
      {
        if (null == _propertySetters)
        {
          _propertySetters = new Dictionary<string, System.Reflection.MethodInfo>();

          foreach (var methodInfo in TypeInfo.DeclaredMethods)
          {
            var setterAttribute = methodInfo.GetCustomAttribute<ViewManagerPropertySetterAttribute>();
            if (null != setterAttribute)
            {
              _propertySetters.Add(setterAttribute.Name, methodInfo);
            }
          }
        }

        return _propertySetters;
      }
    }
    private Dictionary<string, System.Reflection.MethodInfo> _propertySetters;

    public ManagedViewManager(IManagedViewManager viewManager)
    {
      ViewManager = viewManager ?? throw new ArgumentNullException(nameof(viewManager));
      TypeInfo = viewManager.GetType().GetTypeInfo();
    }

    // IView Manager

    protected override string GetNameCore() => ViewManager.Name;

    protected override FrameworkElement CreateViewCore() => ViewManager.CreateView();

    protected override void UpdatePropertiesCore(FrameworkElement view, IReadOnlyDictionary<string, object> propertyMap)
    {
      foreach (var property in propertyMap)
      {
        if (PropertySetters.TryGetValue(property.Key, out System.Reflection.MethodInfo methodInfo))
        {
          methodInfo.Invoke(ViewManager, new object[] { view, property.Value });
        }
      }
    }
  }
}
