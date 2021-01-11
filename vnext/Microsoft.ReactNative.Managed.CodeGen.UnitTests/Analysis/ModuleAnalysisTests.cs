// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
namespace Microsoft.ReactNative.Managed.CodeGen.UnitTests.Analysis
{
  [TestClass]
  public class ModuleAnalysisTests : AnalysisTestBase
  {
    [TestMethod]
    public void NoAttirbuteIsNoModule()
    {
      var (codeAnalyzer, type) = AnalyzeFile(@"
        public class TestClass
        {
        }");
      var result = codeAnalyzer.TryExtractModule(type, out _);
      Assert.IsFalse(result);
    }

    [TestMethod]
    public void EmptyModule()
    {
      var (codeAnalyzer, type) = AnalyzeFile(@"
        using Microsoft.ReactNative.Managed;

        [ReactModule]
        public class TestClass
        {
        }");
      var result = codeAnalyzer.TryExtractModule(type, out var module);
      Assert.IsTrue(result);
      Assert.AreEqual(type, module.ModuleType);
      Assert.AreEqual(type.Name, module.ModuleName);
      Assert.AreEqual(ReactNativeNames.DefaultEventEmitterName, module.EventEmitterName);
      Assert.AreEqual(0, module.Initializers.Count);
      Assert.AreEqual(0, module.Constants.Count);
      Assert.AreEqual(0, module.ConstantProviders.Count);
      Assert.AreEqual(0, module.Methods.Count);
      Assert.AreEqual(0, module.Functions.Count);
      Assert.AreEqual(0, module.Events.Count);
    }

    [TestMethod]
    public void NamedModule()
    {
      var (codeAnalyzer, type) = AnalyzeFile(@"
      using Microsoft.ReactNative.Managed;

      [ReactModule(""MyModule"")]
      public class TestClass
      {
      }");
      var result = codeAnalyzer.TryExtractModule(type, out var module);
      Assert.IsTrue(result);
      Assert.AreEqual("MyModule", module.ModuleName);
      Assert.AreEqual(ReactNativeNames.DefaultEventEmitterName, module.EventEmitterName);
    }

    [TestMethod]
    public void CustomNames()
    {
      var (codeAnalyzer, type) = AnalyzeFile(@"
        using Microsoft.ReactNative.Managed;

        [ReactModule(ModuleName=""MyModule"", EventEmitterName=""MyEmitter"")]
        public class TestClass
        {
        }");
      var result = codeAnalyzer.TryExtractModule(type, out var module);
      Assert.IsTrue(result);
      Assert.AreEqual("MyModule", module.ModuleName);
      Assert.AreEqual("MyEmitter", module.EventEmitterName);
    }
  }
}
