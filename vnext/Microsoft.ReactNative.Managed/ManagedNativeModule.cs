// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;

using Microsoft.ReactNative.Bridge;

namespace Microsoft.ReactNative.Managed
{
  public interface IManagedNativeModule
  {
    string Name { get; }
    void Initialize();
  }

  public sealed class ManagedNativeModule : INativeModule
  {
    public IManagedNativeModule NativeModule { get; private set; }

    internal TypeInfo TypeInfo { get; private set; }

    public ManagedNativeModule(IManagedNativeModule nativeModule)
    {
      NativeModule = nativeModule ?? throw new ArgumentNullException(nameof(nativeModule));
      TypeInfo = nativeModule.GetType().GetTypeInfo();
    }

    // INativeModule

    public string Name => NativeModule.Name;

    public void Initialize() => NativeModule.Initialize();

    public IReadOnlyDictionary<string, object> Constants
    {
      get
      {
        if (null == _constants)
        {
          var constants = new Dictionary<string, object>();

          foreach (var fieldInfo in TypeInfo.DeclaredFields)
          {
            var constantAttribute = fieldInfo.GetCustomAttribute<NativeModuleConstantAttribute>();
            if (null != constantAttribute)
            {
              constants.Add(constantAttribute.Name ?? fieldInfo.Name, fieldInfo.GetValue(NativeModule));
            }
          }

          foreach (var propertyInfo in TypeInfo.DeclaredProperties)
          {
            var constantAttribute = propertyInfo.GetCustomAttribute<NativeModuleConstantAttribute>();
            if (null != constantAttribute)
            {
              constants.Add(constantAttribute.Name ?? propertyInfo.Name, propertyInfo.GetMethod.Invoke(NativeModule, null));
            }
          }

          _constants = constants;
        }
        return _constants;
      }
    }
    private IReadOnlyDictionary<string, object> _constants;

    public IReadOnlyList<Bridge.MethodInfo> Methods
    {
      get
      {
        if (null == _methods)
        {
          var methods = new List<Bridge.MethodInfo>();

          foreach (var methodInfo in TypeInfo.DeclaredMethods)
          {
            var methodAttribute = methodInfo.GetCustomAttribute<NativeModuleMethodAttribute>();
            if (null != methodAttribute)
            {
              string methodName = methodAttribute.Name ?? methodInfo.Name;

              if (methodInfo.ReturnType != typeof(void))
              {
                methods.Add(new Bridge.MethodInfo(methodName, ReturnType.Promise, (args, resolve, reject) =>
                {
                  try
                  {
                    resolve(new object[] { methodInfo.Invoke(NativeModule, args.ToArray()) });
                  }
                  catch (Exception ex)
                  {
                    reject(new object[] { "0x80000000", ex.Message, ex.StackTrace, ex });
                  }
                }));
              }
            }
          }

          _methods = methods;
        }
        return _methods;
      }
    }
    private IReadOnlyList<Bridge.MethodInfo> _methods;
  }
}
