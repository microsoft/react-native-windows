// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
namespace Microsoft.ReactNative.Managed.CodeGen.UnitTests.Analysis
{
  [TestClass]
  public class ModuleInitializerAnalysisTests : AnalysisTestBase
  {
    [TestMethod]
    public void InitializerSuccess()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactInitializer]
        public void Init(ReactContext context)
        {
        }
        ");
      var result = codeAnalyzer.TryExtractModule(type, out var module);
      Assert.IsTrue(result);

      Assert.AreEqual(1, module.Initializers.Count);
      Assert.AreEqual("Init", module.Initializers.First().Method.Name);
    }

    [TestMethod]
    public void InitializersSuccess()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactInitializer]
        public void Init1(ReactContext context)
        {
        }

        [ReactInitializer]
        public void Init2(ReactContext context)
        {
        }
        ");
      var result = codeAnalyzer.TryExtractModule(type, out var module);
      Assert.IsTrue(result);

      Assert.AreEqual(2, module.Initializers.Count);
      Assert.AreEqual("Init1", module.Initializers.First().Method.Name);
      Assert.AreEqual("Init2", module.Initializers.Skip(1).First().Method.Name);
    }

    [TestMethod]
    public void InitializerMustNotBePrivate()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactInitializer]
        private void Init(ReactContext context)
        {
        }
        ");
      codeAnalyzer.TryExtractModule(type, out var module);

      ExpectSingleError(codeAnalyzer, DiagnosticDescriptors.InitializersMustBePublicOrInternal);
    }

    [TestMethod]
    public void InitializerMustReturnVoid()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactInitializer]
        public int Init(ReactContext context)
        {
          return 0;
        }
        ");
      codeAnalyzer.TryExtractModule(type, out var module);

      ExpectSingleError(codeAnalyzer, DiagnosticDescriptors.InitializersMustReturnVoid);
    }

    [TestMethod]
    public void InitializerMustHaveSingleArgNotZero()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactInitializer]
        public void Init()
        {
        }
        ");
      codeAnalyzer.TryExtractModule(type, out var module);

      ExpectSingleError(codeAnalyzer, DiagnosticDescriptors.InitializersMustHaveOneParameterOfTypeReactContext);
    }

    [TestMethod]
    public void InitializerMustHaveSingleArgNotOneAsInt()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactInitializer]
        public void Init(int arg)
        {
        }
        ");
      codeAnalyzer.TryExtractModule(type, out var module);

      ExpectSingleError(codeAnalyzer, DiagnosticDescriptors.InitializersMustHaveOneParameterOfTypeReactContext);
    }

    [TestMethod]
    public void InitializerMustHaveSingleArgNotTwo()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactInitializer]
        public void Init(ReactContext context, int arg)
        {
        }
        ");
      codeAnalyzer.TryExtractModule(type, out var module);

      ExpectSingleError(codeAnalyzer, DiagnosticDescriptors.InitializersMustHaveOneParameterOfTypeReactContext);
    }
  }
}
