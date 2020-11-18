// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;

#pragma warning disable CS8625 // This file is compiled both without and with nullable
#pragma warning disable CS8618 // This file is compiled both without and with nullable

namespace Microsoft.ReactNative.Managed
{
  /// <summary>
  /// 
  /// </summary>
  /// <publicapi>true</publicapi>
  [AttributeUsage(AttributeTargets.Class)]
  public sealed class ReactModuleAttribute : Attribute
  {
    public ReactModuleAttribute(string moduleName = null)
    {
      ModuleName = moduleName;
    }

    public string ModuleName { get; set; }

    public string EventEmitterName { get; set; }
  }

  /// <summary>
  /// 
  /// </summary>
  /// <publicapi>true</publicapi>
  [AttributeUsage(AttributeTargets.Method)]
  public sealed class ReactInitializerAttribute : Attribute
  {
  }

  /// <summary>
  /// 
  /// </summary>
  /// <publicapi>true</publicapi>
  [AttributeUsage(AttributeTargets.Property | AttributeTargets.Field)]
  public sealed class ReactConstantAttribute : Attribute
  {
    public ReactConstantAttribute(string constantName = null)
    {
      ConstantName = constantName;
    }

    public string ConstantName { get; set; }
  }

  /// <summary>
  /// 
  /// </summary>
  /// <publicapi>true</publicapi>
  [AttributeUsage(AttributeTargets.Method)]
  public sealed class ReactConstantProviderAttribute : Attribute
  {
  }

  /// <summary>
  /// 
  /// </summary>
  /// <publicapi>true</publicapi>
  [AttributeUsage(AttributeTargets.Method)]
  public sealed class ReactMethodAttribute : Attribute
  {
    public ReactMethodAttribute(string methodName = null)
    {
      MethodName = methodName;
    }

    public string MethodName { get; set; }
  }

  /// <summary>
  /// 
  /// </summary>
  /// <publicapi>true</publicapi>
  [AttributeUsage(AttributeTargets.Method)]
  public sealed class ReactSyncMethodAttribute : Attribute
  {
    public ReactSyncMethodAttribute(string methodName = null)
    {
      MethodName = methodName;
    }

    public string MethodName { get; set; }
  }

  /// <summary>
  /// 
  /// </summary>
  /// <publicapi>true</publicapi>
  [AttributeUsage(AttributeTargets.Field | AttributeTargets.Property)]
  public sealed class ReactEventAttribute : Attribute
  {
    public ReactEventAttribute(string eventName = null)
    {
      EventName = eventName;
    }

    public string EventName { get; set; }

    public string EventEmitterName { get; set; }
  }

  /// <summary>
  /// 
  /// </summary>
  /// <publicapi>true</publicapi>
  [AttributeUsage(AttributeTargets.Field | AttributeTargets.Property)]
  public sealed class ReactFunctionAttribute : Attribute
  {
    public ReactFunctionAttribute(string functionName = null)
    {
      FunctionName = functionName;
    }

    public string FunctionName { get; set; }

    public string ModuleName { get; set; }
  }
}
