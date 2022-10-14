// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Microsoft.ReactNative.Managed.CodeGen.UnitTests.Analysis
{
  [TestClass]
  public class ModuleGetConstantsAnalysisTests : AnalysisTestBase
  {
    [TestMethod]
    public void SuccessReturnStruct()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactGetConstants]
        public System.ValueTuple<string, int> GetMyConstants()
        {
          return new System.ValueTuple<string, int>(""Hello"", 42);
        }
        ");
      var result = codeAnalyzer.TryExtractModule(type, out var module);
      Assert.IsTrue(result);

      Assert.AreEqual(1, module.GetConstants.Count);
      Assert.AreEqual("GetMyConstants", module.GetConstants.First().Method.Name);
    }

    [TestMethod]
    public void SuccessReturnClass()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactGetConstants]
        public System.Tuple<string, int> GetMyConstants()
        {
          return new System.Tuple<string, int>(""Hello"", 42);
        }
        ");
      var result = codeAnalyzer.TryExtractModule(type, out var module);
      Assert.IsTrue(result);

      Assert.AreEqual(1, module.GetConstants.Count);
      Assert.AreEqual("GetMyConstants", module.GetConstants.First().Method.Name);
    }

    [TestMethod]
    public void NotPublic()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactGetConstants]
        private System.ValueTuple<string, int> GetMyConstants(int x)
        {
          return new System.ValueTuple<string, int>(""Hello"", 42);
        }
        ");
      codeAnalyzer.TryExtractModule(type, out _);
      ExpectSingleError(codeAnalyzer, DiagnosticDescriptors.GetConstantsMustBePublicOrInternal);
    }

    [TestMethod]
    public void ReturnsVoid()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactGetConstants]
        public void GetMyConstants()
        {
        }
        ");
      codeAnalyzer.TryExtractModule(type, out _);
      ExpectSingleError(codeAnalyzer, DiagnosticDescriptors.GetConstantsMustNotReturnVoid);
    }

    [TestMethod]
    public void ReturnsInt()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactGetConstants]
        public int GetMyConstants()
        {
          return 42;
        }
        ");
      codeAnalyzer.TryExtractModule(type, out _);
      ExpectSingleError(codeAnalyzer, DiagnosticDescriptors.GetConstantsMustReturnStructOrClass);
    }

    [TestMethod]
    public void ExtraArg()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactGetConstants]
        public System.ValueTuple<string, int> GetMyConstants(int x)
        {
          return new System.ValueTuple<string, int>(""Hello"", 42);
        }
        ");
      codeAnalyzer.TryExtractModule(type, out _);
      ExpectSingleError(codeAnalyzer, DiagnosticDescriptors.GetConstantsMustNotHaveParameters);
    }
  }
}
