// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
namespace Microsoft.ReactNative.Managed.CodeGen.UnitTests.Analysis
{
  [TestClass]
  public class ModuleEventsAnalysisTests : AnalysisTestBase
  {
    [TestMethod]
    public void EventsPropertySuccess()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactEvent]
        public Action<int> TimedEvent { get; set; }
        ");
      var result = codeAnalyzer.TryExtractModule(type, out var module);
      Assert.IsTrue(result);

      Assert.AreEqual(1, module.Events.Count);
      Assert.AreEqual("TimedEvent", module.Events.First().Name);
    }

    [TestMethod]
    public void EventsFieldSuccess()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactEvent]
        public Action<int> TimedEvent;
        ");
      var result = codeAnalyzer.TryExtractModule(type, out var module);
      Assert.IsTrue(result);

      Assert.AreEqual(1, module.Events.Count);
      Assert.AreEqual("TimedEvent", module.Events.First().Name);
    }

    [TestMethod]
    public void NamedFields()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactEvent(""Name1"", EventEmitterName=""EmitterName"")]
        public Action<int> TimedEvent { get; set; }
        ");
      var result = codeAnalyzer.TryExtractModule(type, out var module);
      Assert.IsTrue(result);

      Assert.AreEqual(1, module.Events.Count);
      Assert.AreEqual("Name1", module.Events.First().Name);
      Assert.AreEqual("EmitterName", module.Events.First().CallbackContextName);
    }

    [TestMethod]
    public void ExplicitNamedFields()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactEvent(""Name1"", EventName=""OverrideName"", EventEmitterName=""EmitterName"")]
        public Action<int> TimedEvent { get; set; }
        ");
      var result = codeAnalyzer.TryExtractModule(type, out var module);
      Assert.IsTrue(result);

      Assert.AreEqual(1, module.Events.Count);
      Assert.AreEqual("OverrideName", module.Events.First().Name);
      Assert.AreEqual("EmitterName", module.Events.First().CallbackContextName);
    }

    [TestMethod]
    public void CantBePrivate()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactEvent]
        private Action<int> TimedEvent;
        ");
      codeAnalyzer.TryExtractModule(type, out var module);

      ExpectSingleError(codeAnalyzer, DiagnosticDescriptors.CallbackMustBePublicOrInternal);
    }

    [TestMethod]
    public void MystBeSettable()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactEvent]
        public Action<int> TimedEvent { get; }
        ");
      codeAnalyzer.TryExtractModule(type, out var module);

      ExpectSingleError(codeAnalyzer, DiagnosticDescriptors.CallbacksMustBeSettable);
    }

    [TestMethod]
    public void MustBeSettable2()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactEvent]
        public Action<int> TimedEvent { get; private set; }
        ");
      codeAnalyzer.TryExtractModule(type, out var module);

      ExpectSingleError(codeAnalyzer, DiagnosticDescriptors.CallbacksMustBeSettable);
    }

    [TestMethod]
    public void MustBeDelegate()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactEvent]
        public int TimedEvent { get; set; }
        ");
      codeAnalyzer.TryExtractModule(type, out var module);

      ExpectSingleError(codeAnalyzer, DiagnosticDescriptors.CallbackTypeMustBeDelegate);
    }

    [TestMethod]
    public void CallbackMustReturnVoid()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactEvent]
        public Func<int> TimedEvent { get; set; }
        ");
      codeAnalyzer.TryExtractModule(type, out var module);

      ExpectSingleError(codeAnalyzer, DiagnosticDescriptors.CallbackDelegateMustReturnVoid);
    }
  }
}
