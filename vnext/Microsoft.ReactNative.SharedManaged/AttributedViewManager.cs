// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Linq.Expressions;
using System.Reflection;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Media;

using Microsoft.ReactNative.Bridge;

namespace Microsoft.ReactNative.Managed
{
  internal abstract class AttributedViewManager<TFrameworkElement> :
    IViewManager,
    IViewManagerWithExportedViewConstants,
    IViewManagerWithNativeProperties,
    IViewManagerWithCommands,
    IViewManagerWithExportedEventTypeConstants
    where TFrameworkElement : FrameworkElement, new()
  {
    public IReactContext ReactContext { get; private set; }

    protected AttributedViewManager(IReactContext reactContext)
    {
      ReactContext = reactContext;
    }

    #region IViewManager

    public virtual string Name => typeof(TFrameworkElement).Name;

    public virtual FrameworkElement CreateView() => new TFrameworkElement();

    #endregion

    #region Constants

    public virtual IReadOnlyDictionary<string, object> ExportedViewConstants => _exportedViewConstants ?? (_exportedViewConstants = GetExportedViewConstants());
    private IReadOnlyDictionary<string, object> _exportedViewConstants;

    private IReadOnlyDictionary<string, object> GetExportedViewConstants()
    {
      var typeInfo = GetType().GetTypeInfo();

      var constants = new Dictionary<string, object>();

      foreach (var fieldInfo in typeInfo.DeclaredFields)
      {
        var attribute = fieldInfo.GetCustomAttribute<ViewManagerExportedViewConstantAttribute>();
        if (null != attribute)
        {
          constants.Add(attribute.ConstantName ?? fieldInfo.Name, fieldInfo.GetValue(this));
        }
      }

      foreach (var propertyInfo in typeInfo.DeclaredProperties)
      {
        var attribute = propertyInfo.GetCustomAttribute<ViewManagerExportedViewConstantAttribute>();
        if (null != attribute)
        {
          constants.Add(attribute.ConstantName ?? propertyInfo.Name, propertyInfo.GetMethod.Invoke(this, null));
        }
      }

      return constants;
    }

    #endregion

    #region Properties

    public virtual IReadOnlyDictionary<string, ViewManagerPropertyType> NativeProps
    {
      get
      {
        if (null == _nativeProps)
        {
          var nativeProps = new Dictionary<string, ViewManagerPropertyType>();

          foreach (var kvp in ViewManagerProperties)
          {
            nativeProps.Add(kvp.Key, kvp.Value.Type);
          }
          _nativeProps = nativeProps;
        }

        return _nativeProps;
      }
    }
    private IReadOnlyDictionary<string, ViewManagerPropertyType> _nativeProps;

    public virtual void UpdateProperties(FrameworkElement view, IReadOnlyDictionary<string, object> propertyMap)
    {
      if (view is TFrameworkElement viewAsT)
      {
        foreach (var property in propertyMap)
        {
          if (ViewManagerProperties.TryGetValue(property.Key, out ViewManagerProperty<TFrameworkElement> setter))
          {
            setter.Setter(viewAsT, property.Value);
          }
        }
      }
      else
      {
        throw new ArgumentOutOfRangeException(nameof(view));
      }
    }

    internal Dictionary<string, ViewManagerProperty<TFrameworkElement>> ViewManagerProperties
    {
      get
      {
        if (null == _properties)
        {
          var properties = new Dictionary<string, ViewManagerProperty<TFrameworkElement>>();

          foreach (var methodInfo in GetType().GetTypeInfo().DeclaredMethods)
          {
            var propertyAttribute = methodInfo.GetCustomAttribute<ViewManagerPropertyAttribute>();
            if (null != propertyAttribute)
            {
              var setter = new ViewManagerProperty<TFrameworkElement>();
              setter.Name = propertyAttribute.PropertyName ?? methodInfo.Name;
              setter.Type = propertyAttribute.PropertyType ?? TypeToViewManagerPropertyType(methodInfo.GetParameters()[1].ParameterType);
              setter.Setter = MakePropertySetterMethod(methodInfo);

              properties.Add(setter.Name, setter);
            }
          }

          _properties = properties;
        }

        return _properties;
      }
    }
    private Dictionary<string, ViewManagerProperty<TFrameworkElement>> _properties;

    internal struct ViewManagerProperty<U> where U : TFrameworkElement
    {
      public string Name;
      public ViewManagerPropertyType Type;
      public Action<U, object> Setter;
    }

    private Action<TFrameworkElement, object> MakePropertySetterMethod(MethodInfo methodInfo)
    {
      var parameters = methodInfo.GetParameters();

      if (parameters.Length == 2
        && parameters[0].ParameterType == typeof(TFrameworkElement))
      {
        return (view, propertyValue) =>
        {
          methodInfo.Invoke(this, new object[] { view, propertyValue });
        };
      }

      throw new ArgumentException($"Unable to parse parameters for {methodInfo.Name}.");
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
      else if (t == typeof(decimal) || t == typeof(double) || t == typeof(float) || t == typeof(long) || t == typeof(int) || t == typeof(short) || t == typeof(sbyte) || t == typeof(ulong) || t == typeof(uint) || t == typeof(ushort) || t == typeof(byte) ||
               t == typeof(decimal?) || t == typeof(double?) || t == typeof(float?) || t == typeof(long?) || t == typeof(int?) || t == typeof(sbyte?) || t == typeof(ulong?) || t == typeof(uint?) || t == typeof(ushort?) || t == typeof(byte?))
      {
        return ViewManagerPropertyType.Number;
      }
      else if (t == typeof(IReadOnlyList<object>))
      {
        return ViewManagerPropertyType.Array;
      }
      else if (t == typeof(IReadOnlyDictionary<string, object>))
      {
        return ViewManagerPropertyType.Map;
      }
      else if (t == typeof(Brush))
      {
        return ViewManagerPropertyType.Color;
      }

      throw new ArgumentOutOfRangeException(nameof(t));
    }

    #endregion

    #region Commands

    public virtual IReadOnlyDictionary<string, long> Commands
    {
      get
      {
        if (null == _commands)
        {
          var commands = new Dictionary<string, long>();

          foreach (var kvp in ViewManagerCommands)
          {
            commands.Add(kvp.Value.CommandName, kvp.Value.CommandId);
          }

          _commands = commands;
        }
        return _commands;
      }
    }
    private IReadOnlyDictionary<string, long> _commands;

    public virtual void DispatchCommand(FrameworkElement view, long commandId, IReadOnlyList<object> commandArgs)
    {
      if (view is TFrameworkElement viewAsT)
      {
        if (ViewManagerCommands.TryGetValue(commandId, out ViewManagerCommand<TFrameworkElement> command))
        {
          command.CommandMethod(viewAsT, commandArgs);
        }
      }
      else
      {
        throw new ArgumentOutOfRangeException(nameof(view));
      }
    }

    internal Dictionary<long, ViewManagerCommand<TFrameworkElement>> ViewManagerCommands
    {
      get
      {
        if (null == _viewManagerCommands)
        {
          var viewManagerCommands = new Dictionary<long, ViewManagerCommand<TFrameworkElement>>();

          foreach (var methodInfo in GetType().GetTypeInfo().DeclaredMethods)
          {
            var commandAttribute = methodInfo.GetCustomAttribute<ViewManagerCommandAttribute>();
            if (null != commandAttribute)
            {
              var command = new ViewManagerCommand<TFrameworkElement>();
              command.CommandName = commandAttribute.CommandName ?? methodInfo.Name;
              command.CommandId = viewManagerCommands.Count;
              command.CommandMethod = MakeCommandMethod(methodInfo);
              viewManagerCommands.Add(command.CommandId, command);
            }
          }

          _viewManagerCommands = viewManagerCommands;
        }
        return _viewManagerCommands;
      }
    }
    private Dictionary<long, ViewManagerCommand<TFrameworkElement>> _viewManagerCommands;

    internal struct ViewManagerCommand<U> where U : TFrameworkElement
    {
      public string CommandName;
      public long CommandId;
      public Action<U, IReadOnlyList<object>> CommandMethod;
    }

    private Action<TFrameworkElement, IReadOnlyList<object>> MakeCommandMethod(MethodInfo methodInfo)
    {
      var parameters = methodInfo.GetParameters();
      if (parameters.Length == 2
        && parameters[0].ParameterType == typeof(TFrameworkElement)
        && parameters[1].ParameterType == typeof(IReadOnlyList<object>))
      {
        return (view, commandArgs) =>
        {
          methodInfo.Invoke(this, new object[] { view, commandArgs });
        };
      }
      else if (parameters.Length >= 2
        && parameters[0].ParameterType == typeof(TFrameworkElement))
      {
        return (view, commandArgs) =>
        {
          var invokeArgs = new List<object>(parameters.Length);
          invokeArgs.Add(view);
          invokeArgs.AddRange(commandArgs);

          methodInfo.Invoke(this, invokeArgs.ToArray());
        };
      }

      throw new ArgumentException($"Unable to parse parameters for {methodInfo.Name}.");
    }

    #endregion

    #region Events

    public virtual IReadOnlyDictionary<string, object> ExportedCustomBubblingEventTypeConstants => _exportedCustomBubblingEventTypeConstants ?? (_exportedCustomBubblingEventTypeConstants = GetExportedCustomBubblingEventTypeConstants());
    private IReadOnlyDictionary<string, object> _exportedCustomBubblingEventTypeConstants;

    public virtual IReadOnlyDictionary<string, object> ExportedCustomDirectEventTypeConstants => _exportedCustomDirectEventTypeConstants ?? (_exportedCustomDirectEventTypeConstants = GetExportedDirectEventTypeConstants());
    private IReadOnlyDictionary<string, object> _exportedCustomDirectEventTypeConstants;

    private IReadOnlyDictionary<string, object> GetExportedCustomBubblingEventTypeConstants()
    {
      var typeInfo = GetType().GetTypeInfo();

      var constants = new Dictionary<string, object>();

      foreach (var fieldInfo in typeInfo.DeclaredFields)
      {
        var attribute = fieldInfo.GetCustomAttribute<ViewManagerExportedBubblingEventTypeConstantAttribute>();
        if (TryMakeBubblingEvent(attribute, fieldInfo, fieldInfo.FieldType, out string key, out object value, out Delegate memberValue))
        {
          constants.Add(key, value);
          fieldInfo.SetValue(this, memberValue);
        }
      }

      foreach (var propertyInfo in typeInfo.DeclaredProperties)
      {
        var attribute = propertyInfo.GetCustomAttribute<ViewManagerExportedBubblingEventTypeConstantAttribute>();
        if (TryMakeBubblingEvent(attribute, propertyInfo, propertyInfo.PropertyType, out string key, out object value, out Delegate memberValue))
        {
          constants.Add(key, value);
          propertyInfo.SetValue(this, memberValue);
        }
      }

      return constants;
    }

    private bool TryMakeBubblingEvent(ViewManagerExportedBubblingEventTypeConstantAttribute attribute, MemberInfo memberInfo, Type memberType, out string constantKey, out object constantValue, out Delegate memberValue)
    {
      if (null != attribute && null != memberInfo && TryGetEventDataType(memberType, out Type eventDataType))
      {
        var eventName = attribute.EventName ?? "top" + memberInfo.Name;
        var bubbleName = attribute.BubbleCallbackName ?? "on" + memberInfo.Name;
        var captureName = attribute.CaptureCallbackName ?? bubbleName + "Capture";

        var registration = new Dictionary<string, object>
        {
          { "phasedRegistrationNames", new Dictionary<string, object>()
            {
              { "bubbled", bubbleName },
              { "captured", captureName },
            }
          }
        };

        constantKey = eventName;
        constantValue = registration;
        memberValue = MakeEventDelegate(eventName, memberType, eventDataType);

        return true;
      }

      constantKey = default;
      constantValue = default;
      memberValue = default;

      return false;
    }

    private IReadOnlyDictionary<string, object> GetExportedDirectEventTypeConstants()
    {
      var typeInfo = GetType().GetTypeInfo();

      var constants = new Dictionary<string, object>();

      foreach (var fieldInfo in typeInfo.DeclaredFields)
      {
        var attribute = fieldInfo.GetCustomAttribute<ViewManagerExportedDirectEventTypeConstantAttribute>();
        if (TryMakeDirectEvent(attribute, fieldInfo, fieldInfo.FieldType, out string key, out object value, out Delegate memberValue))
        {
          constants.Add(key, value);
          fieldInfo.SetValue(this, memberValue);
        }
      }

      foreach (var propertyInfo in typeInfo.DeclaredProperties)
      {
        var attribute = propertyInfo.GetCustomAttribute<ViewManagerExportedDirectEventTypeConstantAttribute>();
        if (TryMakeDirectEvent(attribute, propertyInfo, propertyInfo.PropertyType, out string key, out object value, out Delegate memberValue))
        {
          constants.Add(key, value);
          propertyInfo.SetValue(this, memberValue);
        }
      }

      return constants;
    }

    private bool TryMakeDirectEvent(ViewManagerExportedDirectEventTypeConstantAttribute attribute, MemberInfo memberInfo, Type memberType, out string constantKey, out object constantValue, out Delegate memberValue)
    {
      if (null != attribute && null != memberInfo && TryGetEventDataType(memberType, out Type eventDataType))
      {
        var eventName = attribute.EventName ?? "top" + memberInfo.Name;
        var callbackName = attribute.CallbackName ?? "on" + memberInfo.Name;

        var registration = new Dictionary<string, object>
        {
          { "registrationName", callbackName }
        };

        constantKey = eventName;
        constantValue = registration;
        memberValue = MakeEventDelegate(eventName, memberType, eventDataType);

        return true;
      }

      constantKey = default;
      constantValue = default;
      memberValue = default;

      return false;
    }

    private bool TryGetEventDataType(Type eventType, out Type eventDataType)
    {
      if (null != eventType && typeof(Delegate).IsAssignableFrom(eventType))
      {
        var eventDelegateMethod = eventType.GetMethod("Invoke");
        ParameterInfo[] parameters = eventDelegateMethod.GetParameters();

        if (parameters.Length == 2 && parameters[0].ParameterType == typeof(TFrameworkElement))
        {
          eventDataType = parameters[1].ParameterType;
          return true;
        }
      }

      eventDataType = default;
      return false;
    }

    private Delegate MakeEventDelegate(string eventName, Type memberType, Type eventDataType)
    {
      //
      // (TFrameworkElement view, TEventData eventData) =>
      // {
      //   ReactContext.DispatchEvent(view, eventName, eventData);
      // };
      //

      ParameterExpression viewParameter = Expression.Parameter(typeof(TFrameworkElement), "view");
      ParameterExpression eventDataParameter = Expression.Parameter(eventDataType, "eventData");

      MemberExpression thisReactContext = Expression.Property(Expression.Constant(this), "ReactContext");

      MethodCallExpression body = Expression.Call(thisReactContext, typeof(IReactContext).GetMethod("DispatchEvent"), viewParameter, Expression.Constant(eventName, typeof(string)), eventDataParameter);

      return Expression.Lambda(memberType, body, viewParameter, eventDataParameter).Compile();
    }

    #endregion
  }
}
