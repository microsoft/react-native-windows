// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
namespace Microsoft.ReactNative.Managed.CodeGen.UnitTests.Analysis
{
  [TestClass]
  public class ModuleSyncMethodAnalysisTests : AnalysisTestBase
  {
    [TestMethod]
    public void MethodSuccess()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactSyncMethod]
        public void Test()
        {
        }
        ");
      var result = codeAnalyzer.TryExtractModule(type, out var module);
      Assert.IsTrue(result);

      Assert.AreEqual(1, module.Methods.Count);
      Assert.AreEqual("Test", module.Methods.First().Name);
    }

    [TestMethod]
    public void MethodWithReturnValuesSuccess()
    {
      var (codeAnalyzer, type) = AnalyzeModuleFile(@"
        [ReactSyncMethod]
        public int Test(string a0, long a1, byte a2)
        {
          return 0;
        }
        ");
      var result = codeAnalyzer.TryExtractModule(type, out var module);
      Assert.IsTrue(result);

      Assert.AreEqual(1, module.Methods.Count);
      Assert.AreEqual("Test", module.Methods.First().Name);
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
      Assert.AreEqual("MyTest", module.Methods.First().Name);
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
      Assert.AreEqual("MyTest", module.Methods.First().Name);
    }
  }
}
