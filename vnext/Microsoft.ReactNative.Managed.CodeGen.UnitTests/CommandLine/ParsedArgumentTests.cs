// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.ReactNative.Managed.CodeGen.Commandline;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Microsoft.ReactNative.Managed.CodeGen.UnitTests.CommandLine
{
  [TestClass]
  public class ParsedArgumentTests
  {
    [DataTestMethod]
    [DataRow("/a:b", "a", "a", "b")]
    [DataRow("-a:b", "a", null, "b")]
    [DataRow("--a:b", null, "a", "b")]
    [DataRow("/a", "a", "a", null)]
    [DataRow("-a", "a", null, null)]
    [DataRow("--a", null, "a", null)]
    public void ParseSuccessArg(string arg, string expectedShort, string expectedLong, string expectedValue)
    {
      var pass = ParsedArgument.TryParse(arg, out var parsedArgument);
      Assert.IsTrue(pass);

      Assert.AreEqual(arg, parsedArgument.OriginalArg);
      Assert.AreEqual(expectedShort, parsedArgument.ShortName);
      Assert.AreEqual(expectedLong, parsedArgument.LongName);
      Assert.AreEqual(expectedValue, parsedArgument.Value);
    }

    [DataTestMethod]
    [DataRow("/s:value", true)]
    [DataRow("/long:value", true)]
    [DataRow("-s:value", true)]
    [DataRow("-long:value", false)]
    [DataRow("--s:value", false)]
    [DataRow("--long:value", true)]
    [DataRow("-ss:value", false)]
    [DataRow("/ss:value", false)]
    [DataRow("/S:value", false)]
    [DataRow("/LONG:value", false)]
    [DataRow("-S:value", false)]
    [DataRow("--LONG:value", false)]
    public void MatchesShort(string arg, bool expectMatches)
    {
      var pass = ParsedArgument.TryParse(arg, out var parsedArgument);
      Assert.IsTrue(pass);

      var result = parsedArgument.Matches("s", "long");
      Assert.AreEqual(expectMatches, result);
    }

    [DataTestMethod]
    [DataRow("@foo")]
    [DataRow("")]
    [DataRow("/")]
    [DataRow("-")]
    [DataRow("--")]
    [DataRow("/:")]
    [DataRow("-:")]
    [DataRow("--:")]
    [DataRow("/a:")]
    [DataRow("-a:")]
    [DataRow("--a:")]
    [DataRow("/:b")]
    [DataRow("-:b")]
    [DataRow("--:b")]
    public void ParseFailedArg(string arg)
    {
      var pass = ParsedArgument.TryParse(arg, out _);
      Assert.IsFalse(pass);
    }
  }
}
