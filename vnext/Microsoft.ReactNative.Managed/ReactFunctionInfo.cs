// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Linq;
using System.Reflection;
using System.Threading;
using static Microsoft.ReactNative.Managed.JSValueGenerator;
using static Microsoft.ReactNative.Managed.ReactContextGenerator;
using static System.Linq.Expressions.Expression;

namespace Microsoft.ReactNative.Managed
{
  internal class ReactFunctionInfo
  {
    public ReactFunctionInfo(PropertyInfo propertyInfo, ReactFunctionAttribute functionAttribute, string moduleName)
    {
      var functionName = functionAttribute.FunctionName ?? propertyInfo.Name;
      moduleName = functionAttribute.ModuleName ?? moduleName;
      var functionArgTypes = GetFunctionArgTypes(propertyInfo.PropertyType,
        formatError: message => $"{message} Module: {propertyInfo.DeclaringType.Name}, Property: {propertyInfo.Name}");
      FunctionImpl = new Lazy<ReactFunctionImpl>(() =>
        MakeFunction(propertyInfo, moduleName, functionName, functionArgTypes), LazyThreadSafetyMode.PublicationOnly);
    }

    public ReactFunctionInfo(FieldInfo fieldInfo, ReactFunctionAttribute functionAttribute, string moduleName)
    {
      var functionName = functionAttribute.FunctionName ?? fieldInfo.Name;
      moduleName = functionAttribute.ModuleName ?? moduleName;
      var functionArgTypes = GetFunctionArgTypes(fieldInfo.FieldType,
        formatError: message => $"{message} Module: {fieldInfo.DeclaringType.Name}, Field: {fieldInfo.Name}");
      FunctionImpl = new Lazy<ReactFunctionImpl>(() =>
        MakeFunction(fieldInfo, moduleName, functionName, functionArgTypes), LazyThreadSafetyMode.PublicationOnly);
    }

    private static Type[] GetFunctionArgTypes(Type functionType, Func<string, string> formatError)
    {
      if (!typeof(Delegate).IsAssignableFrom(functionType))
      {
        throw new InvalidOperationException(formatError("React function must be a delegate."));
      }

      MethodInfo functionDelegateMethod = functionType.GetMethod("Invoke");
      ParameterInfo[] parameters = functionDelegateMethod.GetParameters();

      return parameters.Select(p => p.ParameterType).ToArray();
    }

    static MethodInfo CallJSFunction() =>
      typeof(IReactContext).GetMethod(nameof(IReactContext.CallJSFunction));

    private ReactFunctionImpl MakeFunction(PropertyInfo propertyInfo, string moduleName, string functionName, Type[] functionArgTypes)
    {
      // Generate code that looks like:
      //
      // (object module, IReactContext reactContext) =>
      // {
      //   (module as MyModule).FunctionProperty = (ArgType0 arg0, ArgsType1 arg1) =>
      //     reactContext.CallJSFunction(moduleName, functionName, arg0, arg1);
      // });

      return CompileLambda<ReactFunctionImpl>(
        Parameter(typeof(object), out var module),
        Parameter(typeof(IReactContext), out var reactContext),
        module.CastTo(propertyInfo.DeclaringType).SetProperty(propertyInfo,
          AutoLambda(propertyInfo.PropertyType,
            Parameters(functionArgTypes, out var args),
            New(ReactContextConstructor(), reactContext).Call(
              CallJSFunctionOf(functionArgTypes), Constant(moduleName), Constant(functionName), args))));
    }

    private ReactFunctionImpl MakeFunction(FieldInfo fieldInfo, string moduleName, string functionName, Type[] functionArgTypes)
    {
      // Generate code that looks like:
      //
      // (object module, IReactContext reactContext) =>
      // {
      //   (module as MyModule).FunctionField = (ArgType0 arg0, ArgsType1 arg1) =>
      //     reactContext.CallJSFunction(moduleName, functionName, arg0, arg1);
      // });

      return CompileLambda<ReactFunctionImpl>(
        Parameter(typeof(object), out var module),
        Parameter(typeof(IReactContext), out var reactContext),
        module.CastTo(fieldInfo.DeclaringType).SetField(fieldInfo,
          AutoLambda(fieldInfo.FieldType,
            Parameters(functionArgTypes, out var args),
            New(ReactContextConstructor(), reactContext).Call(
              CallJSFunctionOf(functionArgTypes), Constant(moduleName), Constant(functionName), args))));
    }

    public delegate void ReactFunctionImpl(object module, IReactContext reactContext);

    public Lazy<ReactFunctionImpl> FunctionImpl { get; private set; }

    public void AddToModuleBuilder(IReactModuleBuilder moduleBuilder, object module)
    {
      moduleBuilder.AddInitializer((IReactContext reactContext) => FunctionImpl.Value(module, reactContext));
    }
  }
}
