// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.CodeAnalysis;
using Microsoft.ReactNative.Managed.CodeGen.Model;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Microsoft.ReactNative.Managed.CodeGen.UnitTests.CodeGen
{
  [TestClass]
  public class CodeGenEventTests : CodeGenTestBase
  {
    [TestMethod]
    public void NoArgsProperty()
    {
      TestCodeGen<IPropertySymbol>(
        "public Action MyEvent {get; set;}",
        (codeGen, symbol) =>
          codeGen.AddEvent(new ReactEvent(symbol, ((INamedTypeSymbol)symbol.Type).DelegateInvokeMethod.Parameters, "MyNamedEvent", "MyEmitter")));
    }

    [TestMethod]
    public void NoArgsField()
    {
      TestCodeGen<IFieldSymbol>(
        "public Action MyEvent;",
        (codeGen, symbol) =>
          codeGen.AddEvent(new ReactEvent(symbol, ((INamedTypeSymbol)symbol.Type).DelegateInvokeMethod.Parameters, "MyNamedEvent", "MyEmitter")));
    }

    [TestMethod]
    public void SingleArgsProperty()
    {
      TestCodeGen<IPropertySymbol>(
        "public Action<int> MyEvent {get; set;}",
        (codeGen, symbol) =>
          codeGen.AddEvent(new ReactEvent(symbol, ((INamedTypeSymbol)symbol.Type).DelegateInvokeMethod.Parameters, "MyNamedEvent", "MyEmitter")));
    }

    [TestMethod]
    public void SingleArgsField()
    {
      TestCodeGen<IFieldSymbol>(
        "public Action<int> MyEvent;",
        (codeGen, symbol) =>
          codeGen.AddEvent(new ReactEvent(symbol, ((INamedTypeSymbol)symbol.Type).DelegateInvokeMethod.Parameters, "MyNamedEvent", "MyEmitter")));
    }

    [TestMethod]
    public void MultipleArgsProperty()
    {
      TestCodeGen<IPropertySymbol>(
        "public Action<int, string, double> MyEvent {get; set;}",
        (codeGen, symbol) =>
          codeGen.AddEvent(new ReactEvent(symbol, ((INamedTypeSymbol)symbol.Type).DelegateInvokeMethod.Parameters, "MyNamedEvent", "MyEmitter")));
    }

    [TestMethod]
    public void MultipleArgsField()
    {
      TestCodeGen<IFieldSymbol>(
        "public Action<int, string, double> MyEvent;",
        (codeGen, symbol) =>
          codeGen.AddEvent(new ReactEvent(symbol, ((INamedTypeSymbol)symbol.Type).DelegateInvokeMethod.Parameters, "MyNamedEvent", "MyEmitter")));
    }
  }
}
