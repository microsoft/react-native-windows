// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.CodeAnalysis;
using Microsoft.ReactNative.Managed.CodeGen.Model;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Linq;

namespace Microsoft.ReactNative.Managed.CodeGen.UnitTests.CodeGen
{
  [TestClass]
  public class CodeGenViewManager : CodeGenTestBase
  {

    [TestMethod]
    public void SingleModuleWithSingleConstant()
    {
      TestCodeGen<INamedTypeSymbol>(@"
        public class MyViewManager
        {
        }
        ",
        (codeGen, symbol) =>
        {
          return codeGen.CreateViewManagers(new[] {symbol});
        }
      );
    }

    [TestMethod]
    public void TwoModuleWithSingleConstant()
    {
      TestMultipleCodeGen<INamedTypeSymbol>(@"
        public class ViewManager1
        {
        }
        public class ViewManager2
        {
        }
        ",
        (codeGen, symbols) =>
        {
          return codeGen.CreateViewManagers(symbols);
        }
      );
    }
  }
}
