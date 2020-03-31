// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace Microsoft.ReactNative.Managed
{
  [AttributeUsage(AttributeTargets.Class)]
  class ReactModuleAttribute : Attribute
  {
    public ReactModuleAttribute(string moduleName = null)
    {
      ModuleName = moduleName;
    }

    public string ModuleName { get; set; }

    public string EventEmitterName { get; set; }
  }

  [AttributeUsage(AttributeTargets.Method)]
  class ReactInitializerAttribute : Attribute
  {
  }

  [AttributeUsage(AttributeTargets.Property | AttributeTargets.Field)]
  class ReactConstantAttribute : Attribute
  {
    public ReactConstantAttribute(string constantName = null)
    {
      ConstantName = constantName;
    }

    public string ConstantName { get; set; }
  }


  [AttributeUsage(AttributeTargets.Method)]
  class ReactConstantProviderAttribute : Attribute
  {
  }

  [AttributeUsage(AttributeTargets.Method)]
  class ReactMethodAttribute : Attribute
  {
    public ReactMethodAttribute(string methodName = null)
    {
      MethodName = methodName;
    }

    public string MethodName { get; set; }
  }

  [AttributeUsage(AttributeTargets.Method)]
  class ReactSyncMethodAttribute : Attribute
  {
    public ReactSyncMethodAttribute(string methodName = null)
    {
      MethodName = methodName;
    }

    public string MethodName { get; set; }
  }

  [AttributeUsage(AttributeTargets.Field | AttributeTargets.Property)]
  class ReactEventAttribute : Attribute
  {
    public ReactEventAttribute(string eventName = null)
    {
      EventName = eventName;
    }

    public string EventName { get; set; }

    public string EventEmitterName { get; set; }
  }

  [AttributeUsage(AttributeTargets.Field | AttributeTargets.Property)]
  class ReactFunctionAttribute : Attribute
  {
    public ReactFunctionAttribute(string functionName = null)
    {
      FunctionName = functionName;
    }

    public string FunctionName { get; set; }

    public string ModuleName { get; set; }
  }
}
