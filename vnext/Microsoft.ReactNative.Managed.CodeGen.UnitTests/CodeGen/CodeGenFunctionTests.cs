// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.CodeAnalysis;
using Microsoft.ReactNative.Managed.CodeGen.Model;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Microsoft.ReactNative.Managed.CodeGen.UnitTests.CodeGen
{
  [TestClass]
  public class CodeGenFunctionTests : CodeGenTestBase
  {
    [TestMethod]
    public void NoArgsProperty()
    {
      TestCodeGen<IPropertySymbol>(
        "public Action MyEvent {get; set;}",
        (codeGen, symbol) =>
          codeGen.AddFunction(new ReactFunction(symbol, ((INamedTypeSymbol)symbol.Type).DelegateInvokeMethod.Parameters, "MyNamedFunction", "MyModuleName")));
    }

    [TestMethod]
    public void NoArgsField()
    {
      TestCodeGen<IFieldSymbol>(
        "public Action MyFunction;",
        (codeGen, symbol) =>
          codeGen.AddFunction(new ReactFunction(symbol, ((INamedTypeSymbol)symbol.Type).DelegateInvokeMethod.Parameters, "MyNamedFunction", "MyModuleName")));
    }

    [TestMethod]
    public void SingleArgsProperty()
    {
      TestCodeGen<IPropertySymbol>(
        "public Action<int> MyEvent {get; set;}",
        (codeGen, symbol) =>
          codeGen.AddFunction(new ReactFunction(symbol, ((INamedTypeSymbol)symbol.Type).DelegateInvokeMethod.Parameters, "MyNamedFunction", "MyModuleName")));
    }

    [TestMethod]
    public void SingleArgsField()
    {
      TestCodeGen<IFieldSymbol>(
        "public Action<int> MyEvent;",
        (codeGen, symbol) =>
          codeGen.AddFunction(new ReactFunction(symbol, ((INamedTypeSymbol)symbol.Type).DelegateInvokeMethod.Parameters, "MyNamedFunction", "MyModuleName")));
    }

    [TestMethod]
    public void MultipleArgsProperty()
    {
      TestCodeGen<IPropertySymbol>(
        "public Action<int, string, double> MyEvent {get; set;}",
        (codeGen, symbol) =>
          codeGen.AddFunction(new ReactFunction(symbol, ((INamedTypeSymbol)symbol.Type).DelegateInvokeMethod.Parameters, "MyNamedFunction", "MyModuleName")));
    }

    [TestMethod]
    public void MultipleArgsField()
    {
      TestCodeGen<IFieldSymbol>(
        "public Action<int, string, double> MyEvent;",
        (codeGen, symbol) =>
          codeGen.AddFunction(new ReactFunction(symbol, ((INamedTypeSymbol)symbol.Type).DelegateInvokeMethod.Parameters, "MyNamedFunction", "MyModuleName")));
    }
  }
}
