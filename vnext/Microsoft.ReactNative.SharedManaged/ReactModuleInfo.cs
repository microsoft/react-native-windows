// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.ReactNative.Bridge;
using System;
using System.Collections.Generic;
using System.Reflection;
using System.Threading;

namespace Microsoft.ReactNative.Managed
{
  class ReactModuleInfo
  {
    public ReactModuleInfo(Type moduleType, string moduleName, string eventEmitterName)
    {
      ModuleName = moduleName;
      ModuleType = moduleType;
      ModuleProvider = (IReactModuleBuilder moduleBuilder) =>
      {
        if (!string.IsNullOrEmpty(eventEmitterName))
        {
          moduleBuilder.SetEventEmitterName(eventEmitterName);
        }
        object module = Activator.CreateInstance(moduleType);
        AddModuleMembers(moduleBuilder, module);
        return module;
      };

      m_methodInfos = new Lazy<List<ReactMethodInfo>>(InitMethodInfos, LazyThreadSafetyMode.PublicationOnly);
      m_syncMethodInfos = new Lazy<List<ReactSyncMethodInfo>>(InitSyncMethodInfos, LazyThreadSafetyMode.PublicationOnly);
      m_eventInfos = new Lazy<List<ReactEventInfo>>(InitEventInfos, LazyThreadSafetyMode.PublicationOnly);
      m_constantInfos = new Lazy<List<ReactConstantInfo>>(InitConstantInfos, LazyThreadSafetyMode.PublicationOnly);
      m_constantProviderInfos = new Lazy<List<ReactConstantProviderInfo>>(InitConstantProviderInfos, LazyThreadSafetyMode.PublicationOnly);
    }

    internal static ReactModuleInfo GetOrAddModuleInfo(Type moduleType, ReactModuleAttribute moduleAttribute)
    {
      lock (s_moduleInfoMutex)
      {
        if (s_moduleInfos is null)
        {
          s_moduleInfos = new Dictionary<Type, ReactModuleInfo>();
        }

        ReactModuleInfo moduleInfo;
        if (s_moduleInfos.TryGetValue(moduleType, out moduleInfo))
        {
          return moduleInfo;
        }

        moduleInfo = new ReactModuleInfo(
            moduleType,
            moduleAttribute.ModuleName ?? moduleType.Name,
            moduleAttribute.EventEmitterName);
        s_moduleInfos.Add(moduleType, moduleInfo);
        return moduleInfo;
      }
    }

    private void AddModuleMembers(IReactModuleBuilder moduleBuilder, object module)
    {
      foreach (var methodInfo in m_methodInfos.Value)
      {
        methodInfo.AddToModuleBuilder(moduleBuilder, module);
      }

      foreach (var syncMethodInfo in m_syncMethodInfos.Value)
      {
        syncMethodInfo.AddToModuleBuilder(moduleBuilder, module);
      }

      foreach (var eventInfo in m_eventInfos.Value)
      {
        eventInfo.AddToModuleBuilder(moduleBuilder, module);
      }

      foreach (var constantInfo in m_constantInfos.Value)
      {
        constantInfo.AddToModuleBuilder(moduleBuilder, module);
      }

      foreach (var constantProviderInfo in m_constantProviderInfos.Value)
      {
        constantProviderInfo.AddToModuleBuilder(moduleBuilder, module);
      }
    }

    private List<ReactMethodInfo> InitMethodInfos()
    {
      var result = new List<ReactMethodInfo>();
      foreach (var method in ModuleType.GetMethods())
      {
        var methodAttribute = method.GetCustomAttribute<ReactMethodAttribute>();
        if (methodAttribute != null)
        {
          var methodInfo = new ReactMethodInfo(method, methodAttribute);
          result.Add(methodInfo);
        }
      }

      return result;
    }

    private List<ReactSyncMethodInfo> InitSyncMethodInfos()
    {
      var result = new List<ReactSyncMethodInfo>();
      foreach (var method in ModuleType.GetMethods())
      {
        var syncMethodAttribute = method.GetCustomAttribute<ReactSyncMethodAttribute>();
        if (syncMethodAttribute != null)
        {
          var syncMethodInfo = new ReactSyncMethodInfo(method, syncMethodAttribute);
          result.Add(syncMethodInfo);
        }
      }

      return result;
    }

    private List<ReactEventInfo> InitEventInfos()
    {
      var result = new List<ReactEventInfo>();
      foreach (var propertyInfo in ModuleType.GetProperties())
      {
        var eventAttribute = propertyInfo.GetCustomAttribute<ReactEventAttribute>();
        if (eventAttribute != null)
        {
          var eventInfo = new ReactEventInfo(propertyInfo, eventAttribute);
          result.Add(eventInfo);
        }
      }

      return result;
    }

    private List<ReactConstantInfo> InitConstantInfos()
    {
      var result = new List<ReactConstantInfo>();
      foreach (var propertyInfo in ModuleType.GetProperties())
      {
        var constantAttribute = propertyInfo.GetCustomAttribute<ReactConstantAttribute>();
        if (constantAttribute != null)
        {
          var constantInfo = new ReactConstantInfo(propertyInfo, constantAttribute);
          result.Add(constantInfo);
        }
      }

      foreach (var fieldInfo in ModuleType.GetFields())
      {
        var constantAttribute = fieldInfo.GetCustomAttribute<ReactConstantAttribute>();
        if (constantAttribute != null)
        {
          var constantInfo = new ReactConstantInfo(fieldInfo, constantAttribute);
          result.Add(constantInfo);
        }
      }

      return result;
    }

    private List<ReactConstantProviderInfo> InitConstantProviderInfos()
    {
      var result = new List<ReactConstantProviderInfo>();

      foreach (var methodInfo in ModuleType.GetMethods())
      {
        var constantProviderAttribute = methodInfo.GetCustomAttribute<ReactConstantProviderAttribute>();
        if (constantProviderAttribute != null)
        {
          var constantInfo = new ReactConstantProviderInfo(methodInfo);
          result.Add(constantInfo);
        }
      }

      return result;
    }

    public string ModuleName { get; private set; }

    public Type ModuleType { get; private set; }

    public ReactModuleProvider ModuleProvider { get; private set; }

    private static object s_moduleInfoMutex = new object();
    private static Dictionary<Type, ReactModuleInfo> s_moduleInfos;
    private Lazy<List<ReactConstantInfo>> m_constantInfos;
    private Lazy<List<ReactConstantProviderInfo>> m_constantProviderInfos;
    private Lazy<List<ReactMethodInfo>> m_methodInfos;
    private Lazy<List<ReactSyncMethodInfo>> m_syncMethodInfos;
    private Lazy<List<ReactEventInfo>> m_eventInfos;
  }
}
