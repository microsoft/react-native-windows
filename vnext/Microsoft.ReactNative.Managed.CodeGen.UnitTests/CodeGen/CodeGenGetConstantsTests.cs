// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.CodeAnalysis;
using Microsoft.ReactNative.Managed.CodeGen.Model;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Microsoft.ReactNative.Managed.CodeGen.UnitTests.CodeGen
{
  [TestClass]
  public class CodeGenGetConstantsTests : CodeGenTestBase
  {
    [TestMethod]
    public void RegularCase()
    {
      TestCodeGen<IMethodSymbol>(
        @"public System.ValueTuple<string,int> MyMethod() { return new System.ValueTuple<string,int>(""Hello"", 42); }",
        (codeGen, symbol) =>
          codeGen.AddGetConstants(new ReactGetConstants(symbol)));
    }
  }
}
