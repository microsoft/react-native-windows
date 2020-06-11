// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
namespace Microsoft.ReactNative.Managed.CodeGen.UnitTests.Analysis
{
  [TestClass]
  public class ModuleConstantProviderAnalysisTests : AnalysisTestBase
  {
    [TestMethod]
    public void Success()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactConstantProvider]
        public void Provider(ReactConstantProvider provider)
        {
        }
        ");
      var result = codeAnalyzer.TryExtractModule(type, out var module);
      Assert.IsTrue(result);

      Assert.AreEqual(1, module.ConstantProviders.Count);
      Assert.AreEqual("Provider", module.ConstantProviders.First().Method.Name);
    }

    [TestMethod]
    public void NoArgs()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactConstantProvider]
        public void Provider()
        {
        }
        ");
      codeAnalyzer.TryExtractModule(type, out _);
      ExpectSingleError(codeAnalyzer, DiagnosticDescriptors.ConstantProviderMustHaveOneParameterOfTypeReactContext);
    }

    [TestMethod]
    public void WrongType()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactConstantProvider]
        public void Provider(ReactContext context)
        {
        }
        ");
      codeAnalyzer.TryExtractModule(type, out var module);
      ExpectSingleError(codeAnalyzer, DiagnosticDescriptors.ConstantProviderMustHaveOneParameterOfTypeReactContext);
    }

    [TestMethod]
    public void ExtraArg()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactConstantProvider]
        public void Provider(ReactConstantProvider x, int y)
        {
        }
        ");
      codeAnalyzer.TryExtractModule(type, out var module);
      ExpectSingleError(codeAnalyzer, DiagnosticDescriptors.ConstantProviderMustHaveOneParameterOfTypeReactContext);
    }

    [TestMethod]
    public void NotVoid()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactConstantProvider]
        public int Provider(ReactConstantProvider provider)
        {
          return 0;
        }
        ");
      codeAnalyzer.TryExtractModule(type, out var module);
      ExpectSingleError(codeAnalyzer, DiagnosticDescriptors.ConstantProviderMustReturnVoid);
    }

    [TestMethod]
    public void NotPublic()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactConstantProvider]
        private void Provider(ReactConstantProvider provider)
        {
        }
        ");
      codeAnalyzer.TryExtractModule(type, out var module);
      ExpectSingleError(codeAnalyzer, DiagnosticDescriptors.ConstantProviderMustBePublicOrInternal);
    }
  }
}
