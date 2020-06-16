// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.CodeAnalysis;
using Microsoft.ReactNative.Managed.CodeGen.Model;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Microsoft.ReactNative.Managed.CodeGen.UnitTests.CodeGen
{
  [TestClass]
  public class CodeGenConstantProviderTests : CodeGenTestBase
  {
    [TestMethod]
    public void RegularCase()
    {
      TestCodeGen<IMethodSymbol>(
        "public void MyMethod(ReactConstantProvider provider) {  }",
        (codeGen, symbol) =>
          codeGen.AddConstantProvider(new ReactConstantProvider(symbol)));
    }
  }
}
