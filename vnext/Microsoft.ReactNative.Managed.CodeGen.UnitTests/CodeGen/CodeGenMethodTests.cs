// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System.Collections.Generic;
using System.Linq;
using Microsoft.CodeAnalysis;
using Microsoft.ReactNative.Managed.CodeGen.Model;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Microsoft.ReactNative.Managed.CodeGen.UnitTests.CodeGen
{
  [TestClass]
  public class CodeGenMethodTests : CodeGenTestBase
  {

    [TestMethod]
    public void NoParamReturnVoid()
    {
      TestMethod("public void Method() {}", ReactMethod.MethodReturnStyle.Void);
    }

    [TestMethod]
    public void OneParamReturnVoid()
    {
      TestMethod("public void Method(int x) {}", ReactMethod.MethodReturnStyle.Void);
    }

    [TestMethod]
    public void ThreeParamReturnVoid()
    {
      TestMethod("public void Method(int x, string y, double z) {}", ReactMethod.MethodReturnStyle.Void);
    }

    [TestMethod]
    public void NoParamReturnInt()
    {
      TestMethod("public int Method() { return 42; }", ReactMethod.MethodReturnStyle.ReturnValue);
    }

    [TestMethod]
    public void OneParamReturnInt()
    {
      TestMethod("public int Method(int x) { return 42; }", ReactMethod.MethodReturnStyle.ReturnValue);
    }

    [TestMethod]
    public void ThreeParamReturnInt()
    {
      TestMethod("public int Method(int x, string y, double z) { return 42; }", ReactMethod.MethodReturnStyle.ReturnValue);
    }

    [TestMethod]
    public void ReadOnlyListOfStringParamReturnReadOnlyList()
    {
      TestMethod("public string Method(IReadOnlyList<string> values) { return string.Empty; }", ReactMethod.MethodReturnStyle.ReturnValue);
    }

    [TestMethod]
    public void PromiseParamReturnVoid()
    {
      TestMethod("public void Method(IReactPromise<int> promise) {}", ReactMethod.MethodReturnStyle.Promise);
    }

    [TestMethod]
    public void PromiseStructParamReturnVoid()
    {
      TestMethod("public void Method(ReactPromise<int> promise) {}", ReactMethod.MethodReturnStyle.Promise);
    }

    [TestMethod]
    public void PromiseStructOneArgParamReturnVoid()
    {
      TestMethod("public void Method(string s, ReactPromise<int> promise) {}", ReactMethod.MethodReturnStyle.Promise);
    }

    [TestMethod]
    public void IntParamAndPromiseParamReturnVoid()
    {
      TestMethod("public void Method(int x, IReactPromise<int> promise) {}", ReactMethod.MethodReturnStyle.Promise);
    }

    [TestMethod]
    public void ReadOnlyListOfStringPromiseReturnVoid()
    {
      TestMethod("public void Method(string s, IReactPromise<IReadOnlyList<string>> p2) {}", ReactMethod.MethodReturnStyle.Promise);
    }

    [TestMethod]
    public void DoublePromiseParamReturnVoid()
    {
      TestMethod("public void Method(IReactPromise<string> p1, IReactPromise<int> p2) {}", ReactMethod.MethodReturnStyle.Promise);
    }

    [TestMethod]
    public void IntParamAndDoublePromiseParamReturnVoid()
    {
      TestMethod("public void Method(int x, IReactPromise<string> p1, IReactPromise<int> p2) {}", ReactMethod.MethodReturnStyle.Promise);
    }

    [TestMethod]
    public void SingleArgCallbackParamReturnVoid()
    {
      TestMethod("public void Method(Action<int> cb1) { }", ReactMethod.MethodReturnStyle.Callback);
    }

    [TestMethod]
    public void SingleArgCallbackWithReadOnlyListParamReturnVoid()
    {
      TestMethod("public void Method(Action<IReadOnlyList<string>> cb1) { }", ReactMethod.MethodReturnStyle.Callback);
    }

    [TestMethod]
    public void IntParamAndSingleArgCallbackParamReturnVoid()
    {
      TestMethod("public void Method(int x, Action<int> cb1) { }", ReactMethod.MethodReturnStyle.Callback);
    }

    [TestMethod]
    public void DoubleArgCallbackParamReturnVoid()
    {
      TestMethod("public void Method(Action<int> cb1, Action<int> cb2) { }", ReactMethod.MethodReturnStyle.TwoCallbacks);
    }

    [TestMethod]
    public void IntParamAndDoubleArgCallbackParamReturnVoid()
    {
      TestMethod("public void Method(int x, Action<int> cb1, Action<int> cb2) { }", ReactMethod.MethodReturnStyle.TwoCallbacks);
    }

    [TestMethod]
    public void ZeroArgTask()
    {
      TestMethod("public Task Method() { return Task.FromResult(42); }", ReactMethod.MethodReturnStyle.Task, isSynchronous: false);
    }

    [TestMethod]
    public void TwoArgsTask()
    {
      TestMethod("public Task Method(int x, int y) { return Task.FromResult(42); }", ReactMethod.MethodReturnStyle.Task, isSynchronous: false);
    }

    [TestMethod]
    public void ZeroArgTaskOfInt()
    {
      TestMethod("public Task<int> Method() { return Task.FromResult(42); }", ReactMethod.MethodReturnStyle.Task, isSynchronous: false);
    }

    [TestMethod]
    public void TwoArgsTaskOfInt()
    {
      TestMethod("public Task<int> Method(int x, int y) { return Task.FromResult(42); }", ReactMethod.MethodReturnStyle.Task, isSynchronous: false);
    }

    [TestMethod]
    public void ZeroArgTaskOfReadOnlyListOfString()
    {
      TestMethod("public Task<IReadOnlyList<string>> Method() { return Task.FromResult<IReadOnlyList<string>>(new string[] { \"s1\", \"s2\" }); }", ReactMethod.MethodReturnStyle.Task, isSynchronous: false);
    }

    private void TestMethod(string methodDecl, ReactMethod.MethodReturnStyle returnStyle)
    {
      TestMethod(methodDecl, returnStyle, isSynchronous: true);
      TestMethod(methodDecl, returnStyle, isSynchronous: false);
    }

    private void TestMethod(string methodDecl, ReactMethod.MethodReturnStyle returnStyle, bool isSynchronous)
    {
      TestCodeGen<IMethodSymbol>(
        methodDecl,
        (codeGen, symbol) =>
        {
          var returnType = symbol.ReturnType;
          var parameters = new List<IParameterSymbol>(symbol.Parameters);
          switch (returnStyle)
          {
            case ReactMethod.MethodReturnStyle.Promise:
            case ReactMethod.MethodReturnStyle.Callback:
              returnType = ((INamedTypeSymbol)parameters[parameters.Count - 1].Type).TypeArguments[0];
              parameters.RemoveAt(parameters.Count - 1);
              break;
            case ReactMethod.MethodReturnStyle.TwoCallbacks:
              returnType = ((INamedTypeSymbol)parameters[parameters.Count - 2].Type).TypeArguments[0];
              parameters.RemoveRange(parameters.Count - 2, 2);
              break;
            case ReactMethod.MethodReturnStyle.Task:
              var namedReturnType = ((INamedTypeSymbol) returnType);
              returnType = namedReturnType.TypeArguments.Any()
                ? ((INamedTypeSymbol) returnType).TypeArguments[0]
                : codeGen.ReactTypes.SystemVoid;
              break;
          }
          return codeGen.AddMethod(new ReactMethod(
            symbol,
            "MyMethod",
            isSynchronous: isSynchronous,
            returnStyle: returnStyle,
            effectiveReturnType: returnType,
            effectiveParameters: parameters));
        },
        lkgName: isSynchronous ? "Sync" : "Async");
    }

  }
}
