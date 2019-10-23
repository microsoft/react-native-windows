// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Reflection;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Media;

using Microsoft.ReactNative.Bridge;

namespace Microsoft.ReactNative.Managed
{
  internal abstract class AttributedViewManager<T> : IViewManager where T : FrameworkElement, new()
  {
    public virtual string Name => typeof(T).Name;

    public virtual FrameworkElement CreateView() => new T();

    public IReadOnlyDictionary<string, object> Commands => _commands ?? (_commands = GetConstantsByAttribute<ViewManagerCommandAttribute>());
    private IReadOnlyDictionary<string, object> _commands;

    public IReadOnlyDictionary<string, object> ExportedViewConstants => _exportedViewConstants ?? (_exportedViewConstants = GetConstantsByAttribute<ViewManagerExportedViewConstantAttribute>());
    private IReadOnlyDictionary<string, object> _exportedViewConstants;

    public IReadOnlyDictionary<string, ViewManagerPropertyType> NativeProps
    {
      get
      {
        if (null == _nativeProps)
        {
          var nativeProps = new Dictionary<string, ViewManagerPropertyType>();
          foreach (var kvp in PropertySetters)
          {
            nativeProps.Add(kvp.Key, kvp.Value.Type);
          }
          _nativeProps = nativeProps;
        }
        return _nativeProps;
      }
    }
    private IReadOnlyDictionary<string, ViewManagerPropertyType> _nativeProps;

    internal Dictionary<string, ViewManagerPropertySetter<T>> PropertySetters
    {
      get
      {
        if (null == _propertySetters)
        {
          _propertySetters = new Dictionary<string, ViewManagerPropertySetter<T>>();

          foreach (var methodInfo in GetType().GetTypeInfo().DeclaredMethods)
          {
            var setterAttribute = methodInfo.GetCustomAttribute<ViewManagerPropertySetterAttribute>();
            if (null != setterAttribute)
            {
              var setter = new ViewManagerPropertySetter<T>();
              setter.Name = setterAttribute.Name;
              setter.Type = setterAttribute.Type ?? TypeToViewManagerPropertyType(methodInfo.GetParameters()[1].ParameterType);
              setter.Method = (view, propertyValue) =>
              {
                methodInfo.Invoke(this, new object[] { view, propertyValue });
              };
              _propertySetters.Add(setter.Name, setter);
            }
          }
        }

        return _propertySetters;
      }
    }
    private Dictionary<string, ViewManagerPropertySetter<T>> _propertySetters;

    public void UpdateProperties(FrameworkElement view, IReadOnlyDictionary<string, object> propertyMap)
    {
      if (view is T viewAsT)
      {
        foreach (var property in propertyMap)
        {
          if (PropertySetters.TryGetValue(property.Key, out ViewManagerPropertySetter<T> setter))
          {
            setter.Method(viewAsT, property.Value);
          }
        }
      }
      else
      {
        throw new ArgumentOutOfRangeException(nameof(view));
      }
    }

    private static ViewManagerPropertyType TypeToViewManagerPropertyType(Type t)
    {
      if (t == typeof(bool) || t == typeof(bool?))
      {
        return ViewManagerPropertyType.Boolean;
      }
      else if (t == typeof(string))
      {
        return ViewManagerPropertyType.String;
      }
      else if (t == typeof(double) || t == typeof(float) || t == typeof(long) || t == typeof(int) || t == typeof(byte) ||
               t == typeof(double?) || t == typeof(float?) || t == typeof(long?) || t == typeof(int?) || t == typeof(byte?))
      {
        return ViewManagerPropertyType.Number;
      }
      else if (t == typeof(object[]))
      {
        return ViewManagerPropertyType.Array;
      }
      else if (t == typeof(Brush))
      {
        return ViewManagerPropertyType.Color;
      }

      throw new ArgumentOutOfRangeException(nameof(t));
    }

    private IReadOnlyDictionary<string, object> GetConstantsByAttribute<U>() where U : ViewManagerNamedAttribute
    {
      var typeInfo = GetType().GetTypeInfo();

      var constants = new Dictionary<string, object>();

      foreach (var fieldInfo in typeInfo.DeclaredFields)
      {
        var attribute = fieldInfo.GetCustomAttribute<U>();
        if (null != attribute)
        {
          constants.Add(attribute.Name ?? fieldInfo.Name, fieldInfo.GetValue(this));
        }
      }

      foreach (var propertyInfo in typeInfo.DeclaredProperties)
      {
        var attribute = propertyInfo.GetCustomAttribute<U>();
        if (null != attribute)
        {
          constants.Add(attribute.Name ?? propertyInfo.Name, propertyInfo.GetMethod.Invoke(this, null));
        }
      }

      return constants;
    }

    internal struct ViewManagerPropertySetter<U> where U : T
    {
      public string Name;
      public ViewManagerPropertyType Type;
      public Action<U, object> Method;
    }
  }
}
