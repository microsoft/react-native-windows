// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Linq.Expressions;
using System.Reflection;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Media;

namespace Microsoft.ReactNative.Managed
{
  internal abstract class AttributedViewManager<TFrameworkElement> :
    IViewManager,
    IViewManagerWithReactContext,
    IViewManagerWithExportedViewConstants,
    IViewManagerWithNativeProperties,
    IViewManagerWithCommands,
    IViewManagerWithExportedEventTypeConstants
    where TFrameworkElement : FrameworkElement, new()
  {
    public IReactContext ReactContext { get; set; }

    #region IViewManager

    public virtual string Name => typeof(TFrameworkElement).Name;

    public virtual FrameworkElement CreateView() => new TFrameworkElement();

    #endregion

    #region Constants

    public virtual ConstantProvider ExportedViewConstants => _exportedViewConstantProvider ?? (_exportedViewConstantProvider = MakeExportedViewConstantProvider());
    private ConstantProvider _exportedViewConstantProvider;

    private ConstantProvider MakeExportedViewConstantProvider()
    {
      return new ConstantProvider((IJSValueWriter constantWriter) =>
      {
        var typeInfo = GetType().GetTypeInfo();

        var constants = new ReactConstantProvider(constantWriter);

        foreach (var fieldInfo in typeInfo.DeclaredFields)
        {
          var attribute = fieldInfo.GetCustomAttribute<ViewManagerExportedViewConstantAttribute>();
          if (null != attribute)
          {
            constants.Add(attribute.ConstantName ?? fieldInfo.Name, IsEnum(fieldInfo.FieldType) ? Convert.ChangeType(fieldInfo.GetValue(this), typeof(long)) : fieldInfo.GetValue(this));
          }
        }

        foreach (var propertyInfo in typeInfo.DeclaredProperties)
        {
          var attribute = propertyInfo.GetCustomAttribute<ViewManagerExportedViewConstantAttribute>();
          if (null != attribute)
          {
            constants.Add(attribute.ConstantName ?? propertyInfo.Name, IsEnum(propertyInfo.PropertyType) ? Convert.ChangeType(propertyInfo.GetMethod.Invoke(this, null), typeof(long)) : propertyInfo.GetMethod.Invoke(this, null));
          }
        }
      });
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
        Type typeOfPropertyValue = parameters[1].ParameterType;

        if (IsNullableEnum(typeOfPropertyValue, out Type enumType))
        {
          return (view, propertyValue) =>
          {
            methodInfo.Invoke(this, new object[] { view, propertyValue != null ? Enum.Parse(enumType, propertyValue.ToString()) : null });
          };
        }
        else if (IsEnum(typeOfPropertyValue))
        {
          return (view, propertyValue) =>
          {
            methodInfo.Invoke(this, new object[] { view, Enum.Parse(typeOfPropertyValue, propertyValue.ToString()) });
          };
        }
        else
        {
          return (view, propertyValue) =>
          {
            methodInfo.Invoke(this, new object[] { view, propertyValue });
          };
        }
      }

      throw new ArgumentException($"Unable to parse parameters for {methodInfo.Name}.");
    }

    private static ViewManagerPropertyType TypeToViewManagerPropertyType(Type t)
    {
      t = Nullable.GetUnderlyingType(t) ?? t;

      if (t == typeof(bool))
      {
        return ViewManagerPropertyType.Boolean;
      }
      else if (t == typeof(string))
      {
        return ViewManagerPropertyType.String;
      }
      else if (t == typeof(decimal) || t == typeof(double) || t == typeof(float) || t == typeof(long) || t == typeof(int) || t == typeof(short) || t == typeof(sbyte) || t == typeof(ulong) || t == typeof(uint) || t == typeof(ushort) || t == typeof(byte) || t.GetTypeInfo().IsEnum)
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

    private static bool IsNullableEnum(Type t, out Type enumType)
    {
      enumType = Nullable.GetUnderlyingType(t);
      return null != enumType && enumType.GetTypeInfo().IsEnum;
    }

    private static bool IsEnum(Type t)
    {
      return IsNullableEnum(t, out Type underlyingType) || t.GetTypeInfo().IsEnum;
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

    public virtual void DispatchCommand(FrameworkElement view, long commandId, IJSValueReader commandArgsReader)
    {
      if (view is TFrameworkElement viewAsT)
      {
        if (ViewManagerCommands.TryGetValue(commandId, out ViewManagerCommand<TFrameworkElement> command))
        {
          command.CommandMethod(viewAsT, commandArgsReader);
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
      public Action<U, IJSValueReader> CommandMethod;
    }

    private Action<TFrameworkElement, IJSValueReader> MakeCommandMethod(MethodInfo methodInfo)
    {
      var parameters = methodInfo.GetParameters();
      if (parameters.Length == 2 && parameters[0].ParameterType == typeof(TFrameworkElement))
      {
        if (parameters[1].ParameterType == typeof(IJSValueReader))
        {
          return (view, commandArgsReader) =>
          {
            methodInfo.Invoke(this, new object[] { view, commandArgsReader });
          };
        }
        else
        {
          MethodInfo genericReadValue = ReadValueMethodInfo.MakeGenericMethod(parameters[1].ParameterType);

          return (view, commandArgsReader) =>
          {
            var result = genericReadValue.Invoke(null, new object[] { commandArgsReader });
            methodInfo.Invoke(this, new object[] { view, result });
          };
        }
      }

      throw new ArgumentException($"Unable to parse parameters for {methodInfo.Name}.");
    }

    #endregion

    #region Events

    public virtual ConstantProvider ExportedCustomBubblingEventTypeConstants => _exportedCustomBubblingEventTypeConstantProvider ?? (_exportedCustomBubblingEventTypeConstantProvider = MakeExportedCustomBubblingEventTypeConstants());
    private ConstantProvider _exportedCustomBubblingEventTypeConstantProvider;

    public virtual ConstantProvider ExportedCustomDirectEventTypeConstants => _exportedCustomDirectEventTypeConstantProvider ?? (_exportedCustomDirectEventTypeConstantProvider = MakeExportedDirectEventTypeConstants());
    private ConstantProvider _exportedCustomDirectEventTypeConstantProvider;

    private ConstantProvider MakeExportedCustomBubblingEventTypeConstants()
    {
      return new ConstantProvider((IJSValueWriter constantWriter) =>
      {
        var typeInfo = GetType().GetTypeInfo();

        foreach (var fieldInfo in typeInfo.DeclaredFields)
        {
          var attribute = fieldInfo.GetCustomAttribute<ViewManagerExportedBubblingEventTypeConstantAttribute>();
          if (TryMakeBubblingEvent(attribute, fieldInfo, fieldInfo.FieldType, constantWriter, out Delegate memberValue))
          {
            fieldInfo.SetValue(this, memberValue);
          }
        }

        foreach (var propertyInfo in typeInfo.DeclaredProperties)
        {
          var attribute = propertyInfo.GetCustomAttribute<ViewManagerExportedBubblingEventTypeConstantAttribute>();
          if (TryMakeBubblingEvent(attribute, propertyInfo, propertyInfo.PropertyType, constantWriter, out Delegate memberValue))
          {
            propertyInfo.SetValue(this, memberValue);
          }
        }
      });
    }

    private bool TryMakeBubblingEvent(ViewManagerExportedBubblingEventTypeConstantAttribute attribute, MemberInfo memberInfo, Type memberType, IJSValueWriter constantWriter, out Delegate memberValue)
    {
      if (null != attribute && null != memberInfo && TryGetEventDataType(memberType, out Type eventDataType))
      {
        var eventName = attribute.EventName ?? "top" + memberInfo.Name;
        var bubbleName = attribute.BubbleCallbackName ?? "on" + memberInfo.Name;
        var captureName = attribute.CaptureCallbackName ?? bubbleName + "Capture";

        constantWriter.WritePropertyName(eventName);

        constantWriter.WriteObjectBegin();

        constantWriter.WritePropertyName("phasedRegistrationNames");
        constantWriter.WriteObjectBegin();
        constantWriter.WriteObjectProperty("bubbled", bubbleName);
        constantWriter.WriteObjectProperty("captured", captureName);
        constantWriter.WriteObjectEnd();

        constantWriter.WriteObjectEnd();

        memberValue = MakeEventDelegate(eventName, memberType, eventDataType);

        return true;
      }

      memberValue = default(Delegate);

      return false;
    }

    private ConstantProvider MakeExportedDirectEventTypeConstants()
    {
      return new ConstantProvider((IJSValueWriter constantWriter) =>
      {
        var typeInfo = GetType().GetTypeInfo();

        foreach (var fieldInfo in typeInfo.DeclaredFields)
        {
          var attribute = fieldInfo.GetCustomAttribute<ViewManagerExportedDirectEventTypeConstantAttribute>();
          if (TryMakeDirectEvent(attribute, fieldInfo, fieldInfo.FieldType, constantWriter, out Delegate memberValue))
          {
            fieldInfo.SetValue(this, memberValue);
          }
        }

        foreach (var propertyInfo in typeInfo.DeclaredProperties)
        {
          var attribute = propertyInfo.GetCustomAttribute<ViewManagerExportedDirectEventTypeConstantAttribute>();
          if (TryMakeDirectEvent(attribute, propertyInfo, propertyInfo.PropertyType, constantWriter, out Delegate memberValue))
          {
            propertyInfo.SetValue(this, memberValue);
          }
        }
      });
    }

    private bool TryMakeDirectEvent(ViewManagerExportedDirectEventTypeConstantAttribute attribute, MemberInfo memberInfo, Type memberType, IJSValueWriter constantWriter, out Delegate memberValue)
    {
      if (null != attribute && null != memberInfo && TryGetEventDataType(memberType, out Type eventDataType))
      {
        var eventName = attribute.EventName ?? "top" + memberInfo.Name;
        var callbackName = attribute.CallbackName ?? "on" + memberInfo.Name;

        constantWriter.WritePropertyName(eventName);
        constantWriter.WriteObjectBegin();
        constantWriter.WriteObjectProperty("registrationName", callbackName);
        constantWriter.WriteObjectEnd();

        memberValue = MakeEventDelegate(eventName, memberType, eventDataType);

        return true;
      }

      memberValue = default(Delegate);

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

      eventDataType = default(Type);
      return false;
    }

    private Delegate MakeEventDelegate(string eventName, Type memberType, Type eventDataType)
    {
      //
      // (TFrameworkElement view, TEventData eventData) =>
      // {
      //   ReactContext.DispatchEvent(view, eventName, ReactEventHelpers.ArgWriter(eventData));
      // };
      //

      ParameterExpression viewParameter = Expression.Parameter(typeof(TFrameworkElement), "view");
      ParameterExpression eventDataParameter = Expression.Parameter(eventDataType, "eventData");

      MemberExpression thisReactContext = Expression.Property(Expression.Constant(this), "ReactContext");
      MethodCallExpression dispatchCall = Expression.Call(thisReactContext, typeof(IReactContext).GetMethod("DispatchEvent"), viewParameter, Expression.Constant(eventName, typeof(string)), Expression.Call(ReactEventInfo.ArgWriterOf(eventDataType), eventDataParameter));

      return Expression.Lambda(memberType, dispatchCall, viewParameter, eventDataParameter).Compile();
    }

    #endregion

    internal static MethodInfo ReadValueMethodInfo
    {
      get
      {
        if (null == _readValueMethodInfo)
        {
          foreach (var methodInfo in typeof(JSValueReader).GetMethods())
          {
            var parameters = methodInfo.GetParameters();
            if (methodInfo.Name == "ReadValue"
              && methodInfo.IsGenericMethodDefinition
              && methodInfo.ReturnType.IsGenericParameter
              && parameters.Length == 1
              && parameters[0].ParameterType == typeof(IJSValueReader))
            {
              _readValueMethodInfo = methodInfo;
              break;
            }
          }
        }
        return _readValueMethodInfo;
      }
    }
    private static MethodInfo _readValueMethodInfo;

  }
}
