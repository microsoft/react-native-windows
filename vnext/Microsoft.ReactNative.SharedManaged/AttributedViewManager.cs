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

    public IReadOnlyDictionary<string, object> ExportedViewConstants => _exportedViewConstants ?? (_exportedViewConstants = GetConstantsByAttribute<ViewManagerExportedViewConstantAttribute>());
    private IReadOnlyDictionary<string, object> _exportedViewConstants;

    #region Properties

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
          var propertySetters = new Dictionary<string, ViewManagerPropertySetter<T>>();

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
              propertySetters.Add(setter.Name, setter);
            }
          }

          _propertySetters = propertySetters;
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

    internal struct ViewManagerPropertySetter<U> where U : T
    {
      public string Name;
      public ViewManagerPropertyType Type;
      public Action<U, object> Method;
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
      else if (t == typeof(decimal) || t == typeof(double) || t == typeof(float) || t == typeof(long) || t == typeof(int) || t== typeof(short) || t == typeof(sbyte) || t == typeof(ulong) || t == typeof(uint) || t == typeof(ushort) || t == typeof(byte) ||
               t == typeof(decimal?) || t == typeof(double?) || t == typeof(float?) || t == typeof(long?) || t == typeof(int?) || t == typeof(sbyte?) || t == typeof(ulong?) || t == typeof(uint?) || t == typeof(ushort?) || t == typeof(byte?))
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

    #endregion

    #region Commands

    public IReadOnlyDictionary<string, long> Commands
    {
      get
      {
        if (null == _commands)
        {
          var commands = new Dictionary<string, long>();

          foreach (var kvp in ViewManagerCommands)
          {
            commands.Add(kvp.Value.Name, kvp.Value.Id);
          }

          _commands = commands;
        }
        return _commands;
      }
    }
    private IReadOnlyDictionary<string, long> _commands;

    internal Dictionary<long, ViewManagerCommand<T>> ViewManagerCommands
    {
      get
      {
        if (null == _viewManagerCommands)
        {
          var viewManagerCommands = new Dictionary<long, ViewManagerCommand<T>>();

          foreach (var methodInfo in GetType().GetTypeInfo().DeclaredMethods)
          {
            var commandAttribute = methodInfo.GetCustomAttribute<ViewManagerCommandAttribute>();
            if (null != commandAttribute)
            {
              var command = new ViewManagerCommand<T>();
              command.Name = commandAttribute.Name ?? methodInfo.Name;
              command.Id = commandAttribute.CommandId ?? viewManagerCommands.Count;
              command.Method = (view, commandArgs) =>
              {
                methodInfo.Invoke(this, new object[] { view, commandArgs });
              };
              viewManagerCommands.Add(command.Id, command);
            }
          }

          _viewManagerCommands = viewManagerCommands;
        }
        return _viewManagerCommands;
      }
    }
    private Dictionary<long, ViewManagerCommand<T>> _viewManagerCommands;

    public void DispatchCommand(FrameworkElement view, long commandId, IReadOnlyList<object> commandArgs)
    {
      if (view is T viewAsT)
      {
        if (ViewManagerCommands.TryGetValue(commandId, out ViewManagerCommand<T> command))
        {
          command.Method(viewAsT, commandArgs);
        }
      }
      else
      {
        throw new ArgumentOutOfRangeException(nameof(view));
      }
    }

    internal struct ViewManagerCommand<U> where U : T
    {
      public string Name;
      public long Id;
      public Action<U, IReadOnlyList<object>> Method;
    }

    #endregion

    #region Reflection Helpers

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

    #endregion
  }
}
