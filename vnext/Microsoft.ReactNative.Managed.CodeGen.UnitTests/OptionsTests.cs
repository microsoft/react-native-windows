// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.IO;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Microsoft.ReactNative.Managed.CodeGen.UnitTests
{
  [TestClass]
  public class OptionsTests
  {
    public TestContext TestContext { get; set; }

    [TestMethod]
    public void BadArg()
    {
      var options = new Options();
      var result = options.TryParse(new[]
      {
        "/bad"
      });

      Assert.IsFalse(result);
    }

    [TestMethod]
    public void NoArgs()
    {
      var options = new Options();
      var result = options.TryParse(new string[0]);
      Assert.IsTrue(result);
      Assert.AreEqual(0, options.SourceFiles.Count);
      Assert.AreEqual(0, options.References.Count);
      Assert.AreEqual(0, options.Defines.Count);
      Assert.AreEqual(0, options.SourceFiles.Count);
      Assert.AreEqual("Microsoft.ReactNative.Managed.CodeGen", options.Namespace);
      Assert.AreEqual("Microsoft.ReactNative.Managed.CodeGen.g.cs", options.OutputFile);
    }

    [TestMethod]
    public void SapareteOutFileAndNamespace()
    {
      var options = new Options();
      var result = options.TryParse(new []
      {
        "/ns:TestNs"
      });

      Assert.IsTrue(result);
      Assert.AreEqual("TestNs", options.Namespace);
      Assert.AreEqual("Microsoft.ReactNative.Managed.CodeGen.g.cs", options.OutputFile);
    }

    [TestMethod]
    public void SeparateNamespaceAndOutFile()
    {
      var options = new Options();
      var result = options.TryParse(new[]
      {
        "/o:OutFile.g.cs"
      });

      Assert.IsTrue(result);
      Assert.AreEqual("Microsoft.ReactNative.Managed.CodeGen", options.Namespace);
      Assert.AreEqual("OutFile.g.cs", options.OutputFile);
    }

    [DataTestMethod]
    [DataRow("/s")]
    [DataRow("/r")]
    [DataRow("/d")]
    [DataRow("/ns")]
    [DataRow("/o")]
    public void AllOptionsMustHaveValue(string arg)
    {
      var options = new Options();
      var result = options.TryParse(new[]
      {
        arg
      });

      Assert.IsFalse(result);
    }

    [TestMethod]
    public void NonExistentSourceFile()
    {
      var options = new Options();
      var result = options.TryParse(new[]
      {
        "/s:NonExistent.cs"
      });

      Assert.IsFalse(result);
    }

    [TestMethod]
    public void NonExistentRefereceFile()
    {
      var options = new Options();
      var result = options.TryParse(new[]
      {
        "/r:NonExistent.dll"
      });

      Assert.IsFalse(result);
    }

    [TestMethod]
    public void LastArgWinsAndLongAndShortNames()
    {
      var options = new Options();
      var result = options.TryParse(new[]
      {
        "/o:out1.g.cs",
        "/outputFile:out2.g.cs",

        "/ns:ns1",
        "/nameSpace:ns2",
      });

      Assert.IsTrue(result);
      Assert.AreEqual("ns2", options.Namespace);
      Assert.AreEqual("out2.g.cs", options.OutputFile);
    }

    [TestMethod]
    public void DefinesShortAndLongNoDedupe()
    {
      var options = new Options();
      var result = options.TryParse(new[]
      {
        "/d:DEFINE_0",
        "/define:DEFINE_1",
        "-d:DEFINE_2",
        "--define:DEFINE_3",
        "-d:DEFINE_3",
      });

      Assert.IsTrue(result);
      Assert.AreEqual(5, options.Defines.Count);
      Assert.AreEqual("DEFINE_0", options.Defines[0]);
      Assert.AreEqual("DEFINE_1", options.Defines[1]);
      Assert.AreEqual("DEFINE_2", options.Defines[2]);
      Assert.AreEqual("DEFINE_3", options.Defines[3]);
      Assert.AreEqual("DEFINE_3", options.Defines[4]);
    }

    [DataTestMethod]
    [DataRow("s", "sourceFile", true)]
    [DataRow("r", "reference", false)]
    public void SourcesShortAndLongNoDedupe(string shortName, string longName, bool isSources)
    {
      Environment.CurrentDirectory = TestContext.TestRunResultsDirectory;
      var ext = isSources ? ".cs" : ".dll";
      var content = isSources ? "// Dummy C# file" : "FAKEDLL";

      var f1 = Path.Combine(TestContext.TestRunResultsDirectory, "s1" + ext);
      var f2 = Path.Combine(TestContext.TestRunResultsDirectory, "s2" + ext);
      var f3 = Path.Combine(TestContext.TestRunResultsDirectory, "subFolder", "otherFolder", "s3" + ext);
      var f4 = Path.Combine(TestContext.TestRunResultsDirectory, "sub Folder", "s4" + ext);

      Directory.CreateDirectory(Path.GetDirectoryName(f1));
      Directory.CreateDirectory(Path.GetDirectoryName(f2));
      Directory.CreateDirectory(Path.GetDirectoryName(f3));
      Directory.CreateDirectory(Path.GetDirectoryName(f4));

      File.WriteAllText(f1, content);
      File.WriteAllText(f2, content);
      File.WriteAllText(f3, content);
      File.WriteAllText(f4, content);

      var options = new Options();
      var result = options.TryParse(new[]
      {
        $"/{shortName}:{f1}",
        $"/{longName}:s2{ext}",
        $"-{shortName}:{f3}",
        $"--{longName}:\"sub Folder\\s4{ext}\"",
      });

      Assert.IsTrue(result);
      var collection = isSources ? options.SourceFiles : options.References;

      Assert.AreEqual(4,  collection.Count);
      Assert.AreEqual(f1, collection[0]);
      Assert.AreEqual(f2, collection[1]);
      Assert.AreEqual(f3, collection[2]);
      Assert.AreEqual(f4, collection[3]);
    }
  }
}
