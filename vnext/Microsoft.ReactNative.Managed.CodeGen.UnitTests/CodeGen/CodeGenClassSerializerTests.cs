// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.CodeAnalysis;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Microsoft.ReactNative.Managed.CodeGen.UnitTests.CodeGen
{
  [TestClass]
  public class CodeGenClassSerializerTests : CodeGenTestBase
  {

    [TestMethod]
    public void EmptyClass()
    {
      TestCodeGen<INamedTypeSymbol>(@"
        public class MyClass
        {
        }
        ",
        (codeGen, symbol) => { return codeGen.CreateObjectSerializers(new[] { symbol }); }
      );
    }

    [TestMethod]
    public void AllPropertyTypes()
    {
      TestCodeGen<INamedTypeSymbol>(@"
        public class MyClass
        {
          public int X => 42;
          public int Y {set {} }
          public int Z {get; set;}

          protected int B1 { get; set; }
          private int B2 { get; set; }
        }
        ",
        (codeGen, symbol) => { return codeGen.CreateObjectSerializers(new[] { symbol }); }
      );
    }

    [TestMethod]
    public void AllFieldTypes()
    {
      TestCodeGen<INamedTypeSymbol>(@"
        public class MyClass
        {
          public readonly int X;
          public const int Y = 42;
          public int Z;

          protected int B1;
          private int B2;
        }
        ",
        (codeGen, symbol) => { return codeGen.CreateObjectSerializers(new[] { symbol }); }
      );
    }

    [TestMethod]
    public void NoStatics()
    {
      TestCodeGen<INamedTypeSymbol>(@"
        public class MyClass
        {
          public static int Prop {get;set;}
          public static int Field;
        }
        ",
        (codeGen, symbol) => { return codeGen.CreateObjectSerializers(new[] { symbol }); }
      );
    }

    [TestMethod]
    public void NoExtern()
    {
      TestCodeGen<INamedTypeSymbol>(@"
        public class MyClass
        {
          public extern static int Prop {get;set;}
        }
        ",
        (codeGen, symbol) => { return codeGen.CreateObjectSerializers(new[] { symbol }); }
      );
    }
  }
}
