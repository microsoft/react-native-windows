// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.CodeAnalysis;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Microsoft.ReactNative.Managed.CodeGen.UnitTests.CodeGen
{
  [TestClass]
  public class CodeGenStructSerializerTests : CodeGenTestBase
  {

    [TestMethod]
    public void EmptyClass()
    {
      TestCodeGen<INamedTypeSymbol>(@"
        public struct MyStruct
        {
        }
        ",
        (codeGen, symbol) => { return codeGen.CreateObjectSerializers(new[] {symbol}); }
      );
    }

    [TestMethod]
    public void AllPropertyTypes()
    {
      TestCodeGen<INamedTypeSymbol>(@"
        public struct MyStruct
        {
          public int X => 42;
          public int Y {set {} }
          public int Z {get; set;}

          private int B2 { get; set; }
        }
        ",
        (codeGen, symbol) => { return codeGen.CreateObjectSerializers(new[] {symbol}); }
      );
    }

    [TestMethod]
    public void AllFieldTypes()
    {
      TestCodeGen<INamedTypeSymbol>(@"
        public struct MyStruct
        {
          public readonly int X;
          public const int Y = 42;
          public int Z;

          private int B2;
        }
        ",
        (codeGen, symbol) => { return codeGen.CreateObjectSerializers(new[] {symbol}); }
      );
    }

    [TestMethod]
    public void AllFieldTypesWithReactPropertyAttribute()
    {
      TestCodeGen<INamedTypeSymbol>(@"
        public struct MyStruct
        {
          [ReactProperty(""x1"")]
          public readonly int X1;

          [ReactProperty(""y1"")]
          public const int Y1 = 42;

          [ReactProperty(""z1"")]
          public int Z1;

          [ReactProperty(""x2"")]
          public int X2 => 42;

          [ReactProperty(""y2"")]
          public int Y2 { set {} }

          [ReactProperty(""z2"")]
          public int Z2 { get; set; }

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
        public struct MyStruct
        {
          public static int Prop {get;set;}
          public static int Field;
        }
        ",
        (codeGen, symbol) => { return codeGen.CreateObjectSerializers(new[] {symbol}); }
      );
    }
    
    [TestMethod]
    public void NoExtern()
    {
      TestCodeGen<INamedTypeSymbol>(@"
        public struct MyStruct
        {
          public extern static int Prop {get;set;}
        }
        ",
        (codeGen, symbol) => { return codeGen.CreateObjectSerializers(new[] {symbol}); }
      );
    }
  }
}
