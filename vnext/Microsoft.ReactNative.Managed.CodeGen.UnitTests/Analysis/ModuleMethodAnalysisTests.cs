// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System.Linq;
using Microsoft.ReactNative.Managed.CodeGen.Model;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using SymbolDisplayFormat = Microsoft.CodeAnalysis.SymbolDisplayFormat;

namespace Microsoft.ReactNative.Managed.CodeGen.UnitTests.Analysis
{
  [TestClass]
  public class ModuleMethodAnalysisTests : AnalysisTestBase
  {
    [TestMethod]
    public void MethodSuccess()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactMethod]
        public void Test()
        {
        }
        ");
      var result = codeAnalyzer.TryExtractModule(type, out var module);
      Assert.IsTrue(result);

      Assert.AreEqual(1, module.Methods.Count);
      var method = module.Methods.First();
      Assert.AreEqual("Test", method.Name);
      Assert.AreEqual(0, method.EffectiveParameters.Count);
      Assert.AreEqual("void", method.EffectiveReturnType.ToDisplayString(SymbolDisplayFormat.CSharpShortErrorMessageFormat));
      Assert.AreEqual(ReactMethod.MethodReturnStyle.Void, method.ReturnStyle);
    }

    [TestMethod]
    public void MethodWithReturnValuesSuccess()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactMethod]
        public int Test(string a0, long a1, byte a2)
        {
          return 0;
        }
        ");
      var result = codeAnalyzer.TryExtractModule(type, out var module);
      Assert.IsTrue(result);

      Assert.AreEqual(1, module.Methods.Count);
      var method = module.Methods.First();
      Assert.AreEqual("Test", method.Name);
      Assert.AreEqual(3, method.EffectiveParameters.Count);
      Assert.AreEqual("void", method.EffectiveReturnType.ToDisplayString(SymbolDisplayFormat.CSharpShortErrorMessageFormat));
      Assert.AreEqual(ReactMethod.MethodReturnStyle.ReturnValue, method.ReturnStyle);
    }

    [TestMethod]
    public void NamedMethodSuccess()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactMethod(""MyTest"")]
        public void Test()
        {
        }
        ");
      var result = codeAnalyzer.TryExtractModule(type, out var module);
      Assert.IsTrue(result);

      Assert.AreEqual(1, module.Methods.Count);
      var method = module.Methods.First();
      Assert.AreEqual("MyTest", method.Name);
      Assert.AreEqual(0, method.EffectiveParameters.Count);
      Assert.AreEqual("void", method.EffectiveReturnType.ToDisplayString(SymbolDisplayFormat.CSharpShortErrorMessageFormat));
      Assert.AreEqual(ReactMethod.MethodReturnStyle.Void, method.ReturnStyle);
    }

    [TestMethod]
    public void NamedExplicitMethodSuccess()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactMethod(MethodName = ""MyTest"")]
        public void Test()
        {
        }
        ");
      var result = codeAnalyzer.TryExtractModule(type, out var module);
      Assert.IsTrue(result);

      Assert.AreEqual(1, module.Methods.Count);
      var method = module.Methods.First();
      Assert.AreEqual("MyTest", method.Name);
      Assert.AreEqual(0, method.EffectiveParameters.Count);
      Assert.AreEqual("void", method.EffectiveReturnType.ToDisplayString(SymbolDisplayFormat.CSharpShortErrorMessageFormat));
      Assert.AreEqual(ReactMethod.MethodReturnStyle.Void, method.ReturnStyle);
    }
    
    [TestMethod]
    public void TaskMethod()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactMethod]
        public Task Test()
        {
          return Task.FromResult(0);
        }
        ");
      var result = codeAnalyzer.TryExtractModule(type, out var module);
      Assert.IsTrue(result);

      Assert.AreEqual(1, module.Methods.Count);
      var method = module.Methods.First();
      Assert.AreEqual("Test", method.Name);
      Assert.AreEqual(0, method.EffectiveParameters.Count);
      Assert.AreEqual("void", method.EffectiveReturnType.ToDisplayString(SymbolDisplayFormat.CSharpShortErrorMessageFormat));
      Assert.AreEqual(ReactMethod.MethodReturnStyle.Task, method.ReturnStyle);
    }
    
    [TestMethod]
    public void TaskOfIntMethod()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactMethod]
        public Task<int> Test()
        {
          return Task.FromResult(0);
        }
        ");
      var result = codeAnalyzer.TryExtractModule(type, out var module);
      Assert.IsTrue(result);

      Assert.AreEqual(1, module.Methods.Count);
      var method = module.Methods.First();
      Assert.AreEqual("Test", method.Name);
      Assert.AreEqual(0, method.EffectiveParameters.Count);
      Assert.AreEqual("int", method.EffectiveReturnType.ToDisplayString(SymbolDisplayFormat.CSharpShortErrorMessageFormat));
      Assert.AreEqual(ReactMethod.MethodReturnStyle.Task, method.ReturnStyle);
    }
    
    [TestMethod]
    public void PromiseMethodOnlyArg()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactMethod]
        public void Test(IReactPromise<int> promise)
        {
        }
        ");
      var result = codeAnalyzer.TryExtractModule(type, out var module);
      Assert.IsTrue(result);

      Assert.AreEqual(1, module.Methods.Count);
      var method = module.Methods.First();
      Assert.AreEqual("Test", method.Name);
      Assert.AreEqual(0, method.EffectiveParameters.Count);
      Assert.AreEqual("int", method.EffectiveReturnType.ToDisplayString(SymbolDisplayFormat.CSharpShortErrorMessageFormat));
      Assert.AreEqual(ReactMethod.MethodReturnStyle.Promise, method.ReturnStyle);
    }

    [TestMethod]
    public void PromiseMethodTwoArgs()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactMethod]
        public string Test(int z, string a0, IReactPromise<int> promise)
        {
          return string.Empty;
        }
        ");
      var result = codeAnalyzer.TryExtractModule(type, out var module);
      Assert.IsTrue(result);

      Assert.AreEqual(1, module.Methods.Count);
      var method = module.Methods.First();
      Assert.AreEqual("Test", method.Name);
      Assert.AreEqual(2, method.EffectiveParameters.Count);
      Assert.AreEqual("int", method.EffectiveReturnType.ToDisplayString(SymbolDisplayFormat.CSharpShortErrorMessageFormat));
      Assert.AreEqual(ReactMethod.MethodReturnStyle.Promise, method.ReturnStyle);
    }
    
    [TestMethod]
    public void SingleCallbackMethodOnlyArg()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactMethod]
        public void Test(Action<int> callback)
        {
        }
        ");
      var result = codeAnalyzer.TryExtractModule(type, out var module);
      Assert.IsTrue(result);

      Assert.AreEqual(1, module.Methods.Count);
      var method = module.Methods.First();
      Assert.AreEqual("Test", method.Name);
      Assert.AreEqual(0, method.EffectiveParameters.Count);
      Assert.AreEqual("int", method.EffectiveReturnType.ToDisplayString(SymbolDisplayFormat.CSharpShortErrorMessageFormat));
      Assert.AreEqual(ReactMethod.MethodReturnStyle.Callback, method.ReturnStyle);
    }

    [TestMethod]
    public void SingleCallbackMethodTwoArgs()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactMethod]
        public string Test(int z, string a0, Action<int> callback)
        {
          return string.Empty;
        }
        ");
      var result = codeAnalyzer.TryExtractModule(type, out var module);
      Assert.IsTrue(result);

      Assert.AreEqual(1, module.Methods.Count);
      var method = module.Methods.First();
      Assert.AreEqual("Test", method.Name);
      Assert.AreEqual(2, method.EffectiveParameters.Count);
      Assert.AreEqual("int", method.EffectiveReturnType.ToDisplayString(SymbolDisplayFormat.CSharpShortErrorMessageFormat));
      Assert.AreEqual(ReactMethod.MethodReturnStyle.Callback, method.ReturnStyle);
    }

    [TestMethod]
    public void DoubleCallbackMethodOnlyArgs()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactMethod]
        public void Test(Action<int> callback, Action<string> reject)
        {
        }
        ");
      var result = codeAnalyzer.TryExtractModule(type, out var module);
      Assert.IsTrue(result);

      Assert.AreEqual(1, module.Methods.Count);
      var method = module.Methods.First();
      Assert.AreEqual("Test", method.Name);
      Assert.AreEqual(0, method.EffectiveParameters.Count);
      Assert.AreEqual("int", method.EffectiveReturnType.ToDisplayString(SymbolDisplayFormat.CSharpShortErrorMessageFormat));
      Assert.AreEqual(ReactMethod.MethodReturnStyle.TwoCallbacks, method.ReturnStyle);
    }

    [TestMethod]
    public void DoubleCallbackMethodTwoArgs()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactMethod]
        public string Test(int z, string a0, Action<int> callback, Action<string> reject)
        {
          return string.Empty;
        }
        ");
      var result = codeAnalyzer.TryExtractModule(type, out var module);
      Assert.IsTrue(result);

      Assert.AreEqual(1, module.Methods.Count);
      var method = module.Methods.First();
      Assert.AreEqual("Test", method.Name);
      Assert.AreEqual(2, method.EffectiveParameters.Count);
      Assert.AreEqual("int", method.EffectiveReturnType.ToDisplayString(SymbolDisplayFormat.CSharpShortErrorMessageFormat));
      Assert.AreEqual(ReactMethod.MethodReturnStyle.TwoCallbacks, method.ReturnStyle);
    }

    [TestMethod]
    public void TaskNotAllowedOnSyncMethods()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactSyncMethod]
        public Task TaskNoArgs()
        {
          return Task.FromResult(42);
        }
        ");
      codeAnalyzer.TryExtractModule(type, out var module);

      ExpectSingleError(codeAnalyzer, DiagnosticDescriptors.MethodShouldNotReturnTaskWhenSynchronous);
    }

    [TestMethod]
    public void TaskOfTNotAllowedOnSyncMethods()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactSyncMethod]
        public Task<int> TaskOfTNoArgs()
        {
          return Task.FromResult(42);
        }
        ");
      codeAnalyzer.TryExtractModule(type, out var module);

      ExpectSingleError(codeAnalyzer, DiagnosticDescriptors.MethodShouldNotReturnTaskWhenSynchronous);
    }
  }
}
