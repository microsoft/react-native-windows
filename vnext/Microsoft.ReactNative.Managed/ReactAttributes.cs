// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;

#pragma warning disable CS8625 // This file is compiled both without and with nullable
#pragma warning disable CS8618 // This file is compiled both without and with nullable

namespace Microsoft.ReactNative.Managed
{
  [AttributeUsage(AttributeTargets.Class)]
  public class ReactModuleAttribute : Attribute
  {
    public ReactModuleAttribute(string moduleName = null)
    {
      ModuleName = moduleName;
    }

    public string ModuleName { get; set; }

    public string EventEmitterName { get; set; }
  }

  [AttributeUsage(AttributeTargets.Method)]
  public class ReactInitializerAttribute : Attribute
  {
  }

  [AttributeUsage(AttributeTargets.Property | AttributeTargets.Field)]
  public class ReactConstantAttribute : Attribute
  {
    public ReactConstantAttribute(string constantName = null)
    {
      ConstantName = constantName;
    }

    public string ConstantName { get; set; }
  }

  [AttributeUsage(AttributeTargets.Method)]
  public class ReactConstantProviderAttribute : Attribute
  {
  }

  [AttributeUsage(AttributeTargets.Method)]
  public class ReactMethodAttribute : Attribute
  {
    public ReactMethodAttribute(string methodName = null)
    {
      MethodName = methodName;
    }

    public string MethodName { get; set; }
  }

  [AttributeUsage(AttributeTargets.Method)]
  public class ReactSyncMethodAttribute : Attribute
  {
    public ReactSyncMethodAttribute(string methodName = null)
    {
      MethodName = methodName;
    }

    public string MethodName { get; set; }
  }

  [AttributeUsage(AttributeTargets.Field | AttributeTargets.Property)]
  public class ReactEventAttribute : Attribute
  {
    public ReactEventAttribute(string eventName = null)
    {
      EventName = eventName;
    }

    public string EventName { get; set; }

    public string EventEmitterName { get; set; }
  }

  [AttributeUsage(AttributeTargets.Field | AttributeTargets.Property)]
  public class ReactFunctionAttribute : Attribute
  {
    public ReactFunctionAttribute(string functionName = null)
    {
      FunctionName = functionName;
    }

    public string FunctionName { get; set; }

    public string ModuleName { get; set; }
  }
}
