// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

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
      TestMethod("public void Method() {}");
    }

    [TestMethod]
    public void OneParamReturnVoid()
    {
      TestMethod("public void Method(int x) {}");
    }

    [TestMethod]
    public void ThreeParamReturnVoid()
    {
      TestMethod("public void Method(int x, string y, double z) {}");
    }

    [TestMethod]
    public void NoParamReturnInt()
    {
      TestMethod("public int Method() { return 42; }");
    }

    [TestMethod]
    public void OneParamReturnInt()
    {
      TestMethod("public int Method(int x) { return 42; }");
    }

    [TestMethod]
    public void ThreeParamReturnInt()
    {
      TestMethod("public int Method(int x, string y, double z) { return 42; }");
    }

    [TestMethod]
    public void PromiseParamReturnVoid()
    {
      TestMethod("public void Method(IReactPromise<int> promise) {}");
    }

    [TestMethod]
    public void IntParamAndPromiseParamReturnVoid()
    {
      TestMethod("public void Method(int x, IReactPromise<int> promise) {}");
    }

    [TestMethod]
    public void DoublePromiseParamReturnVoid()
    {
      TestMethod("public void Method(IReactPromise<string> p1, IReactPromise<int> p2) {}");
    }
    
    [TestMethod]
    public void IntParamAndDoublePromiseParamReturnVoid()
    {
      TestMethod("public void Method(int x, IReactPromise<string> p1, IReactPromise<int> p2) {}");
    }

    [TestMethod]
    public void SingleArgCallbackParamReturnVoid()
    {
      TestMethod("public void Method(Action<int> cb1) { }");
    }

    [TestMethod]
    public void IntParamAndSingleArgCallbackParamReturnVoid()
    {
      TestMethod("public void Method(int x, Action<int> cb1) { }");
    }

    [TestMethod]
    public void DoubleArgCallbackParamReturnVoid()
    {
      TestMethod("public void Method(Action<int> cb1, Action<int> cb2) { }");
    }

    [TestMethod]
    public void IntParamAndDoubleArgCallbackParamReturnVoid()
    {
      TestMethod("public void Method(int x, Action<int> cb1, Action<int> cb2) { }");
    }

    private void TestMethod(string methodDecl)
    {
      TestMethod(methodDecl, isSynchronous: true);
      TestMethod(methodDecl, isSynchronous: false);
    }

    private void TestMethod(string methodDecl, bool isSynchronous)
    {
      TestCodeGen<IMethodSymbol>(
        methodDecl,
        (codeGen, symbol) =>
          codeGen.AddMethod(new ReactMethod(symbol, "MyMethod", isSynchronous: isSynchronous)),
        lkgName: isSynchronous ? "Sync" : "Async");
    }

  }
}
