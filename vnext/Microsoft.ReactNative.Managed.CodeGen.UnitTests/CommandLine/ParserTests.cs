// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.ReactNative.Managed.CodeGen.Commandline;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.IO;

namespace Microsoft.ReactNative.Managed.CodeGen.UnitTests.CommandLine
{
  [TestClass]

  public class ParserTests
  {
    public TestContext TestContext { get; set; }

    [TestMethod]
    public void NoArgs()
    {
      var result = Parser.TryParseArgs(new string[0], out var parsedArgs);
      Assert.IsTrue(result);
      Assert.AreEqual(0, parsedArgs.Count);
    }

    [TestMethod]
    public void SingleArg()
    {
      var result = Parser.TryParseArgs(new[] {"/a"}, out var parsedArgs);
      Assert.IsTrue(result);
      Assert.AreEqual(1, parsedArgs.Count);
      Assert.AreEqual("/a", parsedArgs[0].OriginalArg);
    }

    [TestMethod]
    public void MultiArg()
    {
      var result = Parser.TryParseArgs(new[] {"/a", "/b", "/c"}, out var parsedArgs);
      Assert.IsTrue(result);
      Assert.AreEqual(3, parsedArgs.Count);
      Assert.AreEqual("/a", parsedArgs[0].OriginalArg);
      Assert.AreEqual("/b", parsedArgs[1].OriginalArg);
      Assert.AreEqual("/c", parsedArgs[2].OriginalArg);
    }

    [DataTestMethod]
    [DataRow("", false)]
    [DataRow("", true)]
    [DataRow("folder1", false)]
    [DataRow("folder1", true)]
    [DataRow("folder1\\folder2", false)]
    [DataRow("folder1\\folder2", true)]
    [DataRow("fold with spaces", false)]
    [DataRow("fold with spaces", true)]
    public void SingleRspFilePathVariations(string folder, bool useRelativePath)
    {
      var relativePath = Path.Combine(folder, "test.rps");

      var rspPath = Path.Combine(TestContext.TestRunResultsDirectory, relativePath);
      Environment.CurrentDirectory = TestContext.TestRunResultsDirectory;

      try
      {
        Directory.CreateDirectory(Path.GetDirectoryName(rspPath));
        File.WriteAllLines(rspPath, new[]
        {
          "/a",
          "/b"
        });

        var rspArg = "@" + (useRelativePath
                       ? relativePath
                       : rspPath);

        var result = Parser.TryParseArgs(new[] { rspArg }, out var parsedArgs);
        Assert.IsTrue(result);
        Assert.AreEqual(2, parsedArgs.Count);
        Assert.AreEqual("/a", parsedArgs[0].OriginalArg);
        Assert.AreEqual("/b", parsedArgs[1].OriginalArg);
      }
      finally
      {
        File.Delete(rspPath);
      }
    }

    [TestMethod]
    public void RspFileMixedWithOtherArgs()
    {
      var rspPath = Path.Combine(TestContext.TestRunResultsDirectory, "test.rsp");
      Environment.CurrentDirectory = TestContext.TestRunResultsDirectory;

      try
      {
        Directory.CreateDirectory(Path.GetDirectoryName(rspPath));
        File.WriteAllLines(rspPath, new[]
        {
          "/a",
          "/b"
        });

        var result = Parser.TryParseArgs(new[] { "/aa", "@test.rsp", "/bb" }, out var parsedArgs);
        Assert.IsTrue(result);
        Assert.AreEqual(4, parsedArgs.Count);
        Assert.AreEqual("/aa", parsedArgs[0].OriginalArg);
        Assert.AreEqual("/a", parsedArgs[1].OriginalArg);
        Assert.AreEqual("/b", parsedArgs[2].OriginalArg);
        Assert.AreEqual("/bb", parsedArgs[3].OriginalArg);
      }
      finally
      {
        File.Delete(rspPath);
      }
    }

    [TestMethod]
    public void RspFileWithWithespaceAndComments()
    {
      var rspPath = Path.Combine(TestContext.TestRunResultsDirectory, "test.rsp");
      Environment.CurrentDirectory = TestContext.TestRunResultsDirectory;

      try
      {
        Directory.CreateDirectory(Path.GetDirectoryName(rspPath));
        File.WriteAllLines(rspPath, new[]
        {
          "",                         // empty line
          "# Comment",                // comment with hash in front
          "\t/a\t   ",                // arg with whitespace before and after
          "   \t\t   ",               // various whitespace
          "\t# Comment",              // comment with leading whitespace
          "/b:val with space",    // arg with quotes and space value
          "",                         // empty line
          "  \n   \r\n   ",           // various line endings
          "",                         // empty line
          " /c:\"val with space\"  ", // arg with trailing whitespace
          "# Comment",                // ending file on comment
        });

        var result = Parser.TryParseArgs(new[] { "@test.rsp" }, out var parsedArgs);
        Assert.IsTrue(result);
        Assert.AreEqual(3, parsedArgs.Count);
        Assert.AreEqual("/a", parsedArgs[0].OriginalArg);
        Assert.AreEqual("/b:val with space", parsedArgs[1].OriginalArg);
        Assert.AreEqual("val with space", parsedArgs[1].Value);
        Assert.AreEqual("/c:\"val with space\"", parsedArgs[2].OriginalArg);
        Assert.AreEqual("val with space", parsedArgs[2].Value);
      }
      finally
      {
        File.Delete(rspPath);
      }
    }

    [TestMethod]
    public void RspFileWithNestedRsp()
    {
      var rspPath1 = Path.Combine(TestContext.TestRunResultsDirectory, "test1.rsp");
      var rspPath2 = Path.Combine(TestContext.TestRunResultsDirectory, "test2.rsp");
      var rspPath3 = Path.Combine(TestContext.TestRunResultsDirectory, "folder with space", "test3.rsp");
      Environment.CurrentDirectory = TestContext.TestRunResultsDirectory;

      try
      {
        Directory.CreateDirectory(Path.GetDirectoryName(rspPath1));
        Directory.CreateDirectory(Path.GetDirectoryName(rspPath2));
        Directory.CreateDirectory(Path.GetDirectoryName(rspPath3));

        File.WriteAllLines(rspPath1, new[]
        {
          "/a",
          "@test2.rsp",
          "  @\"" + rspPath3 + "\"",
          "/d"
        });

        File.WriteAllLines(rspPath2, new[]
        {
          "/b",
        });

        File.WriteAllLines(rspPath3, new[]
        {
          "/c",
        });

        var result = Parser.TryParseArgs(new[] { "@test1.rsp" }, out var parsedArgs);
        Assert.IsTrue(result);
        Assert.AreEqual(4, parsedArgs.Count);
        Assert.AreEqual("/a", parsedArgs[0].OriginalArg);
        Assert.AreEqual("/b", parsedArgs[1].OriginalArg);
        Assert.AreEqual("/c", parsedArgs[2].OriginalArg);
        Assert.AreEqual("/d", parsedArgs[3].OriginalArg);
      }
      finally
      {
        File.Delete(rspPath1);
        File.Delete(rspPath2);
        File.Delete(rspPath3);
      }
    }

    [TestMethod]
    public void MissingRspFile()
    {
      var result = Parser.TryParseArgs(new[] { "@Missing.rsp" }, out var parsedArgs);
      Assert.IsFalse(result);
    }
  }
}
