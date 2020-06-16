// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.CodeAnalysis;
using Microsoft.ReactNative.Managed.CodeGen.Model;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Linq;

namespace Microsoft.ReactNative.Managed.CodeGen.UnitTests.CodeGen
{
  [TestClass]
  public class CodeGenModule : CodeGenTestBase
  {

    [TestMethod]
    public void SingleModuleWithSingleConstant()
    {
      TestCodeGen<INamedTypeSymbol>(@"
        public class MyModule
        {
          public int MyConstant => 10;
        }
        ",
        (codeGen, symbol) =>
        {
          var constant = symbol.GetMembers().OfType<IPropertySymbol>().First();
          var module = new ReactModule(symbol, "MyMod", "EventEmitter");
          module.Constants.Add(new ReactConstant(constant, "MyConst"));

          return codeGen.CreateModules(new[] {module});
        }
      );
    }

    [TestMethod]
    public void TwoModuleWithSingleConstant()
    {
      TestMultipleCodeGen<INamedTypeSymbol>(@"
        public class MyModule1
        {
          public int MyConstant1 => 10;
        }

        public class MyModule2
        {
          public int MyConstant2 => 10;
        }
        ",
        (codeGen, symbols) =>
        {
          var modules = symbols.Select(symbol =>
            {
              var constant = symbol.GetMembers().OfType<IPropertySymbol>().First();
              var module = new ReactModule(symbol, symbol.Name, "EventEmitter");
              module.Constants.Add(new ReactConstant(constant, constant.Name));
              return module;
            });

          return codeGen.CreateModules(modules);
        }
      );
    }
  }
}
