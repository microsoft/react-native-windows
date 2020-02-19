// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Reflection;
using System.Threading;
using static Microsoft.ReactNative.Managed.JSValueGenerator;
using static Microsoft.ReactNative.Managed.ReactContextGenerator;
using static System.Linq.Expressions.Expression;

namespace Microsoft.ReactNative.Managed
{
  class ReactEventInfo
  {
    public ReactEventInfo(PropertyInfo propertyInfo, ReactEventAttribute eventAttribute, string eventEmitterName)
    {
      var eventName = eventAttribute.EventName ?? propertyInfo.Name;
      eventEmitterName = eventAttribute.EventEmitterName ?? eventEmitterName;
      var eventArgType = GetEventArgType(propertyInfo.PropertyType,
        formatError: message => $"{message} Module: {propertyInfo.DeclaringType.Name}, Field: {propertyInfo.Name}");
      EventImpl = new Lazy<ReactEventImpl>(() =>
        MakeEvent(propertyInfo, eventEmitterName, eventName, eventArgType), LazyThreadSafetyMode.PublicationOnly);
    }

    public ReactEventInfo(FieldInfo fieldInfo, ReactEventAttribute eventAttribute, string eventEmitterName)
    {
      var eventName = eventAttribute.EventName ?? fieldInfo.Name;
      eventEmitterName = eventAttribute.EventEmitterName ?? eventEmitterName;
      var eventArgType = GetEventArgType(fieldInfo.FieldType,
        formatError: message => $"{message} Module: {fieldInfo.DeclaringType.Name}, Field: {fieldInfo.Name}");
      EventImpl = new Lazy<ReactEventImpl>(() =>
        MakeEvent(fieldInfo, eventEmitterName, eventName, eventArgType), LazyThreadSafetyMode.PublicationOnly);
    }

    private static Type GetEventArgType(Type eventType, Func<string, string> formatError)
    {
      if (!typeof(Delegate).IsAssignableFrom(eventType))
      {
        throw new InvalidOperationException(formatError("React event must be a delegate."));
      }

      MethodInfo eventDelegateMethod = eventType.GetMethod("Invoke");
      ParameterInfo[] parameters = eventDelegateMethod.GetParameters();
      if (parameters.Length != 1)
      {
        throw new InvalidOperationException(formatError("React event delegate must have one parameter."));
      }

      return parameters[0].ParameterType;
    }

    static MethodInfo EmitJSEvent() =>
      typeof(IReactContext).GetMethod(nameof(IReactContext.EmitJSEvent));

    private ReactEventImpl MakeEvent(PropertyInfo propertyInfo, string eventEmitterName, string eventName, Type eventArgType)
    {
      // Generate code that looks like:
      //
      // (object module, IReactContext reactContext) =>
      // {
      //   (module as MyModule).EventProperty = (ArgType arg) =>
      //     reactContext.EmitJSEvent(eventEmitterName, eventName, arg);
      // });

      return CompileLambda<ReactEventImpl>(
        Parameter(typeof(object), out var module),
        Parameter(typeof(IReactContext), out var reactContext),
        module.CastTo(propertyInfo.DeclaringType).SetProperty(propertyInfo,
          AutoLambda(propertyInfo.PropertyType,
            Parameter(eventArgType, out var arg),
            reactContext.CallExt(EmitJSEventOf(eventArgType), Constant(eventEmitterName), Constant(eventName), arg))));
    }

    private ReactEventImpl MakeEvent(FieldInfo fieldInfo, string eventEmitterName, string eventName, Type eventArgType)
    {
      // Generate code that looks like:
      //
      // (object module, IReactContext reactContext) =>
      // {
      //   (module as MyModule).EventField = (ArgType arg) =>
      //     reactContext.EmitJSEvent(eventEmitterName, eventName, arg);
      // });

      return CompileLambda<ReactEventImpl>(
        Parameter(typeof(object), out var module),
        Parameter(typeof(IReactContext), out var reactContext),
        module.CastTo(fieldInfo.DeclaringType).SetField(fieldInfo,
          AutoLambda(fieldInfo.FieldType,
            Parameter(eventArgType, out var arg),
            reactContext.CallExt(EmitJSEventOf(eventArgType), Constant(eventEmitterName), Constant(eventName), arg))));
    }

    public delegate void ReactEventImpl(object module, IReactContext reactContext);

    public Lazy<ReactEventImpl> EventImpl { get; private set; }

    public void AddToModuleBuilder(IReactModuleBuilder moduleBuilder, object module)
    {
      moduleBuilder.AddInitializer((IReactContext reactContext) => EventImpl.Value(module, reactContext));
    }
  }
}
