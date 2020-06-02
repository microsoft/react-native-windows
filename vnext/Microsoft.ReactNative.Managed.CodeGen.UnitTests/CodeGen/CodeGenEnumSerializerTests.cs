// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.CodeAnalysis;
using Microsoft.ReactNative.Managed.CodeGen.Model;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Linq;

namespace Microsoft.ReactNative.Managed.CodeGen.UnitTests.CodeGen
{
  [TestClass]
  public class CodeGenEnumSerializerTests : CodeGenTestBase
  {

    [TestMethod]
    public void EmptyEnum()
    {
      TestCodeGen<INamedTypeSymbol>(@"
        public enum MyEnum
        {
        }
        ",
        (codeGen, symbol) => { return codeGen.CreateEnumSerializers(new[] {symbol}); }
      );
    }

    [TestMethod]
    public void NormalEnum()
    {
      TestCodeGen<INamedTypeSymbol>(@"
        public enum MyEnum
        {
          ValueA,
          ValueB,
          ValueC
        }
        ",
        (codeGen, symbol) =>
        {
          return codeGen.CreateEnumSerializers(new[] { symbol });
        }
      );
    }

    [TestMethod]
    public void FlagsEnum()
    {
      TestCodeGen<INamedTypeSymbol>(@"
        [Flags]
        public enum MyEnum
        {
          ValueA = 0b000,
          ValueB = 0b001,
          ValueC = 0b010,
          ValueD = 0b100,
        }
        ",
        (codeGen, symbol) =>
        {
          return codeGen.CreateEnumSerializers(new[] { symbol });
        }
      );
    }

    [TestMethod]
    public void DifferntBaseType()
    {
      TestCodeGen<INamedTypeSymbol>(@"
        [Flags]
        public enum MyEnum : byte
        {
          ValueA = 0,
          ValueB = 20,
          ValueC = 40,
          ValueD = 255,
        }
        ",
        (codeGen, symbol) =>
        {
          return codeGen.CreateEnumSerializers(new[] { symbol });
        }
      );
    }
  }
}
