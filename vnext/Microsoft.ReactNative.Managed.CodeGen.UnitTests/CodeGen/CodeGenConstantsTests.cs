// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System.Linq;
using Microsoft.CodeAnalysis;
using Microsoft.ReactNative.Managed.CodeGen.Model;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Microsoft.ReactNative.Managed.CodeGen.UnitTests.CodeGen
{
  [TestClass]
  public class CodeGenConstantsTests : CodeGenTestBase
  {
    [TestMethod]
    public void SingleProperty()
    {
      TestCodeGen<IPropertySymbol>(
        "public int MyPop { get; set; }",
        (codeGen, symbol) =>
          codeGen.AddConstants(new[] { new ReactConstant(symbol, "MyPropWithBetterName") }));
    }

    [TestMethod]
    public void SingleField()
    {
      TestCodeGen<IFieldSymbol>(
        "public int MyField = 10;",
        (codeGen, symbol) =>
          codeGen.AddConstants(new[] { new ReactConstant(symbol, "MyFieldWithBetterName") }));
    }

    [TestMethod]
    public void FieldAndProperty()
    {
      TestMultipleCodeGen<ISymbol>(
        "public int MyField1 = 10; public int MyProp => 11;",
        (codeGen, symbols) =>
          codeGen.AddConstants(
            symbols
            .Where(sym => sym is IFieldSymbol || sym is IPropertySymbol)
            .Select(sym => new ReactConstant(sym, sym.Name)))
        );
    }
  }
}
