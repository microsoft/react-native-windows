// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Reflection;
using System.Threading;
using static Microsoft.ReactNative.Managed.JSValueGenerator;
using static System.Linq.Expressions.Expression;

namespace Microsoft.ReactNative.Managed
{
  class ReactEventInfo
  {
    public ReactEventInfo(PropertyInfo propertyInfo, ReactEventAttribute eventAttribute)
    {
      EventName = eventAttribute.EventName ?? propertyInfo.Name;
      var eventArgType = GetEventArgType(propertyInfo.PropertyType,
        formatError: message => $"{message} Module: {propertyInfo.DeclaringType.Name}, Field: {propertyInfo.Name}");
      EventImpl = new Lazy<ReactEventImpl>(() => MakeEvent(propertyInfo, eventArgType), LazyThreadSafetyMode.PublicationOnly);
    }

    public ReactEventInfo(FieldInfo fieldInfo, ReactEventAttribute eventAttribute)
    {
      EventName = eventAttribute.EventName ?? fieldInfo.Name;
      var eventArgType = GetEventArgType(fieldInfo.FieldType,
        formatError: message => $"{message} Module: {fieldInfo.DeclaringType.Name}, Field: {fieldInfo.Name}");
      EventImpl = new Lazy<ReactEventImpl>(() => MakeEvent(fieldInfo, eventArgType), LazyThreadSafetyMode.PublicationOnly);
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

    private ReactEventImpl MakeEvent(PropertyInfo propertyInfo, Type eventArgType)
    {
      // Generate code that looks like:
      //
      // (object module, ReactEventHandler eventHandler) =>
      // {
      //   (module as MyModule).eventProperty = (ArgType arg) => eventHandler(ReactEvent.ArgWriter(arg));
      // });

      return CompileLambda<ReactEventImpl>(
        Parameter(typeof(object), out var module),
        Parameter(typeof(ReactEventHandler), out var eventHandler),
        module.CastTo(propertyInfo.DeclaringType).SetProperty(propertyInfo,
          AutoLambda(propertyInfo.PropertyType,
            Parameter(eventArgType, out var arg),
            eventHandler.Invoke(Call(ArgWriterOf(eventArgType), arg)))));
    }

    private ReactEventImpl MakeEvent(FieldInfo fieldInfo, Type eventArgType)
    {
      // Generate code that looks like:
      //
      // (object module, ReactEventHandler eventHandler) =>
      // {
      //   (module as MyModule).eventField = (ArgType arg) => eventHandler(ReactEvent.ArgWriter(arg));
      // });

      return CompileLambda<ReactEventImpl>(
        Parameter(typeof(object), out var module),
        Parameter(typeof(ReactEventHandler), out var eventHandler),
        module.CastTo(fieldInfo.DeclaringType).SetField(fieldInfo,
          AutoLambda(fieldInfo.FieldType,
            Parameter(eventArgType, out var arg),
            eventHandler.Invoke(Call(ArgWriterOf(eventArgType), arg)))));
    }

    public delegate void ReactEventImpl(object module, ReactEventHandler eventHandler);

    public string EventName { get; private set; }

    public Lazy<ReactEventImpl> EventImpl { get; private set; }

    public void AddToModuleBuilder(IReactModuleBuilder moduleBuilder, object module)
    {
      moduleBuilder.AddEventHandlerSetter(EventName,
        (ReactEventHandler eventHandler) => EventImpl.Value(module, eventHandler));
    }

    static MethodInfo ArgWriterOf(Type typeArg) =>
      typeof(ReactEventHelper).GetMethod(nameof(ReactEventHelper.ArgWriter)).MakeGenericMethod(typeArg);
  }
}
