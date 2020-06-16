// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
namespace Microsoft.ReactNative.Managed.CodeGen.UnitTests.Analysis
{
  [TestClass]
  public class ModuleConstantAnalysisTests : AnalysisTestBase
  {
    [TestMethod]
    public void SimplePropConstant()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactConstant]
        public double NumberProp => 3.14;
        ");
      codeAnalyzer.TryExtractModule(type, out var module);

      var constant = module.Constants.Single();
      Assert.AreEqual("NumberProp", constant.Name);
    }

    [TestMethod]
    public void SimpleFieldConstant()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactConstant]
        public double NumberField = 3.14;
        ");
      codeAnalyzer.TryExtractModule(type, out var module);

      var constant = module.Constants.Single();
      Assert.AreEqual("NumberField", constant.Name);
    }

    [TestMethod]
    public void NamedConstant()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactConstant(""MyProp"")]
        public double NumberProp => 3.14;
        ");
      codeAnalyzer.TryExtractModule(type, out var module);

      var constant = module.Constants.Single();
      Assert.AreEqual("MyProp", constant.Name);
    }

    [TestMethod]
    public void NamedConstant2()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactConstant(""BadName"", ConstantName=""MyProp"")]
        public double NumberProp => 3.14;
        ");
      codeAnalyzer.TryExtractModule(type, out var module);

      var constant = module.Constants.Single();
      Assert.AreEqual("MyProp", constant.Name);
    }

    [TestMethod]
    public void ConstantCantBePrivate()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactConstant]
        private double NumberProp => 3.14;
        ");
      codeAnalyzer.TryExtractModule(type, out var module);

      ExpectSingleError(codeAnalyzer, DiagnosticDescriptors.ConstantsMustBePublicOrInternal);
    }

    [TestMethod]
    public void ConstantMustBeGettable()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactConstant]
        public double NumberProp {set { } }
        ");
      codeAnalyzer.TryExtractModule(type, out var module);

      ExpectSingleError(codeAnalyzer, DiagnosticDescriptors.ConstantsMustBeGettable);
    }

    [TestMethod]
    public void ConstantMustBeGettable2()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactConstant]
        public double NumberProp {private get; set; }
        ");
      codeAnalyzer.TryExtractModule(type, out var module);

      ExpectSingleError(codeAnalyzer, DiagnosticDescriptors.ConstantsMustBeGettable);
    }

  }
}
