// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.ReactNative.Bridge;
using System;
using System.Linq.Expressions;
using System.Reflection;
using System.Threading;
using ReflectionMethodInfo = System.Reflection.MethodInfo;

namespace Microsoft.ReactNative.Managed
{
  class ReactEventInfo
  {
    public ReactEventInfo(PropertyInfo propertyInfo, ReactEventAttribute eventAttribute)
    {
      EventName = eventAttribute.EventName ?? propertyInfo.Name;

      Type propertyType = propertyInfo.PropertyType;
      if (!typeof(Delegate).IsAssignableFrom(propertyType))
      {
        throw new ArgumentException("React event must be a delegate", propertyInfo.Name);
      }
      ReflectionMethodInfo eventDelegateMethod = propertyType.GetMethod("Invoke");
      ParameterInfo[] parameters = eventDelegateMethod.GetParameters();
      if (parameters.Length != 1)
      {
        throw new ArgumentException($"React event delegate must have one parameter. Module: {propertyInfo.DeclaringType.FullName}, Event: {propertyInfo.Name}");
      }
      EventImpl = new Lazy<ReactEventImpl>(() => MakeEvent(propertyInfo, parameters[0]), LazyThreadSafetyMode.PublicationOnly);
    }

    private ReactEventImpl MakeEvent(PropertyInfo propertyInfo, ParameterInfo parameter)
    {
      // We need to create a delegate that can assign correct delegate to the event property.
      // This is the shape of the generated code:
      //
      //(object module, RaiseEvent raiseEvent) =>
      //{
      //  ((MyModule)module).eventProperty = (ArgType arg) =>
      //  {
      //    raiseEvent((IJSValueWriter argWriter) => WriteValue(argWriter, arg));
      //  };
      //});
      //

      // Input parameters for generated lambda
      ParameterExpression moduleParameter = Expression.Parameter(typeof(object), "module");
      ParameterExpression raiseEventParameter = Expression.Parameter(typeof(RaiseEvent), "raiseEvent");

      // Create a lambda to be passed to raiseEvent
      ParameterExpression argParameter = Expression.Parameter(parameter.ParameterType, "arg");
      ParameterExpression argWriterParameter = Expression.Parameter(typeof(IJSValueWriter), "argWriter");
      var writeValueCall = Expression.Call(null, JSValueWriter.GetWriteValueMethod(parameter.ParameterType), argWriterParameter, argParameter);
      var eventHandlerLambda = Expression.Lambda<Bridge.EventHandler>(writeValueCall, argWriterParameter);

      // Create a lambda that we assign to the event property
      var raiseEventCall = Expression.Invoke(raiseEventParameter, eventHandlerLambda);
      var eventLambda = Expression.Lambda(propertyInfo.PropertyType, raiseEventCall, argParameter);

      // Create lambda that we return and which assigns the event property
      var eventProperty = Expression.Property(Expression.Convert(moduleParameter, propertyInfo.DeclaringType), propertyInfo);
      var eventPropertyAssignment = Expression.Assign(eventProperty, eventLambda);
      var lambda = Expression.Lambda<ReactEventImpl>(eventPropertyAssignment, moduleParameter, raiseEventParameter);

      // Compile and return the lambda
      return lambda.Compile();
    }

    public delegate void ReactEventImpl(object module, RaiseEvent raiseEvent);

    public string EventName { get; private set; }

    public Lazy<ReactEventImpl> EventImpl { get; private set; }

    public void AddToModuleBuilder(IReactModuleBuilder moduleBuilder, object module)
    {
      moduleBuilder.AddEventSetter(EventName, (RaiseEvent raiseEvent) =>
      {
        EventImpl.Value(module, raiseEvent);
      });
    }
  }
}
