// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
namespace Microsoft.ReactNative.Managed.CodeGen.UnitTests.Analysis
{
  [TestClass]
  public class ModuleFunctionAnalysisTests : AnalysisTestBase
  {
    [TestMethod]
    public void FunctionPropertySuccess()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactFunction]
        public Action<int> TimedEvent { get; set; }
        ");
      var result = codeAnalyzer.TryExtractModule(type, out var module);
      Assert.IsTrue(result);

      Assert.AreEqual(1, module.Functions.Count);
      Assert.AreEqual("TimedEvent", module.Functions.First().Name);
    }

    [TestMethod]
    public void FunctionFieldSuccess()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactFunction]
        public Action<int> TimedEvent;
        ");
      var result = codeAnalyzer.TryExtractModule(type, out var module);
      Assert.IsTrue(result);

      Assert.AreEqual(1, module.Functions.Count);
      Assert.AreEqual("TimedEvent", module.Functions.First().Name);
    }

    [TestMethod]
    public void NamedFields()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactFunction(""Name1"", FunctionName=""NameOverride"", ModuleName=""EmitterName"")]
        public Action<int> TimedEvent { get; set; }
        ");
      var result = codeAnalyzer.TryExtractModule(type, out var module);
      Assert.IsTrue(result);

      Assert.AreEqual(1, module.Functions.Count);
      Assert.AreEqual("NameOverride", module.Functions.First().Name);
      Assert.AreEqual("EmitterName", module.Functions.First().CallbackContextName);
    }

    [TestMethod]
    public void CantBePrivate()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactFunction]
        private Action<int> TimedEvent;
        ");
      codeAnalyzer.TryExtractModule(type, out var module);

      ExpectSingleError(codeAnalyzer, DiagnosticDescriptors.CallbackMustBePublicOrInternal);
    }

    [TestMethod]
    public void MystBeSettable()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactFunction]
        public Action<int> TimedEvent { get; }
        ");
      codeAnalyzer.TryExtractModule(type, out var module);

      ExpectSingleError(codeAnalyzer, DiagnosticDescriptors.CallbacksMustBeSettable);
    }

    [TestMethod]
    public void MustBeSettable2()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactFunction]
        public Action<int> TimedEvent { get; private set; }
        ");
      codeAnalyzer.TryExtractModule(type, out var module);

      ExpectSingleError(codeAnalyzer, DiagnosticDescriptors.CallbacksMustBeSettable);
    }

    [TestMethod]
    public void MustBeDelegate()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactFunction]
        public int TimedEvent { get; set; }
        ");
      codeAnalyzer.TryExtractModule(type, out var module);

      ExpectSingleError(codeAnalyzer, DiagnosticDescriptors.CallbackTypeMustBeDelegate);
    }
  }
}
