// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.CodeAnalysis;
using Microsoft.ReactNative.Managed.CodeGen.Model;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Microsoft.ReactNative.Managed.CodeGen.UnitTests.CodeGen
{
  [TestClass]
  public class CodeGenInitializerTests : CodeGenTestBase
  {

    [TestMethod]
    public void RegularCase()
    {
      TestCodeGen<IMethodSymbol>(
        "public void Initialize(ReactContext context) {}",
        (codeGen, symbol) =>
          codeGen.AddInitializer(new ReactInitializer(symbol)));
    }

  }
}
