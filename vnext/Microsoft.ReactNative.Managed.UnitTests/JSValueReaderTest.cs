// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;

namespace Microsoft.ReactNative.Managed.UnitTests
{
  class RobotInfo
  {
    public RobotModel Model { get; set; }
    public string Name { get; set; }
    public int Age;
    public RobotShape Shape { get; set; }
    public RobotShape? Shape2 { get; set; }
    public RobotShape? Shape3 { get; set; }
    public IList<int> Steps { get; set; }
    public IDictionary<string, int> Dimensions { get; set; }
    public Tuple<int, string, bool> Badges { get; set; }
    public RobotTool[] Tools { get; set; }
    public RobotPoint[] Path { get; set; }
    public OneOf2<T2Extra, R2D2Extra> Extra { get; set; }
  }

  enum RobotModel
  {
    T2,
    R2D2,
    C3PO,
  };

  // We are generating ReadValue for it.
  enum RobotShape
  {
    Humanoid,
    Trashcan,
    Beercan,
    Quadrocopter,
  }

  class RobotTool
  {
    public string Name { get; set; }
    public int Weight = 0;
    public bool IsEnabled { get; set; }
  }

  struct RobotPoint
  {
    public int X;
    public int Y;
  }

  class T2Extra
  {
    public string ActorName { get; set; }
    public int MovieYear { get; set; }
  }

  class R2D2Extra
  {
    public string MovieSeries { get; set; }
  }

  static class RobotSerialization
  {
    // Reading RobotInfo value. It could be generated instead.
    public static void ReadValue(this IJSValueReader reader, out RobotInfo value)
    {
      value = new RobotInfo();
      if (reader.ValueType == JSValueType.Object)
      {
        while (reader.GetNextObjectProperty(out string propertyName))
        {
          switch (propertyName)
          {
            case nameof(value.Model): value.Model = reader.ReadValue<RobotModel>(); break;
            case nameof(value.Name): value.Name = reader.ReadValue<string>(); break;
            case nameof(value.Age): value.Age = reader.ReadValue<int>(); break;
            case nameof(value.Shape): value.Shape = reader.ReadValue<RobotShape>(); break;
            case nameof(value.Shape2): value.Shape2 = reader.ReadValue<RobotShape?>(); break;
            case nameof(value.Shape3): value.Shape3 = reader.ReadValue<RobotShape?>(); break;
            case nameof(value.Steps): value.Steps = reader.ReadValue<IList<int>>(); break;
            case nameof(value.Dimensions): value.Dimensions = reader.ReadValue<IDictionary<string, int>>(); break;
            case nameof(value.Badges): value.Badges = reader.ReadValue<Tuple<int, string, bool>>(); break;
            case nameof(value.Tools): value.Tools = reader.ReadValue<RobotTool[]>(); break;
            case nameof(value.Path): value.Path = reader.ReadValue<RobotPoint[]>(); break;
            case nameof(value.Extra): value.Extra = reader.ReadValue<OneOf2<T2Extra, R2D2Extra>>(); break;
            default: reader.ReadValue<JSValue>(); break;
          }
        }
      }
    }

    // Writing RobotInfo value. It could be generated instead.
    public static void WriteValue(this IJSValueWriter writer, RobotInfo value)
    {
      if (value != null)
      {
        writer.WriteObjectBegin();
        writer.WriteObjectProperty(nameof(value.Model), value.Model);
        writer.WriteObjectProperty(nameof(value.Name), value.Name);
        writer.WriteObjectProperty(nameof(value.Age), value.Age);
        writer.WriteObjectProperty(nameof(value.Shape), value.Shape);
        writer.WriteObjectProperty(nameof(value.Shape2), value.Shape2);
        writer.WriteObjectProperty(nameof(value.Shape3), value.Shape3);
        writer.WriteObjectProperty(nameof(value.Steps), value.Steps);
        writer.WriteObjectProperty(nameof(value.Dimensions), value.Dimensions);
        writer.WriteObjectProperty(nameof(value.Badges), value.Badges);
        writer.WriteObjectProperty(nameof(value.Tools), value.Tools);
        writer.WriteObjectProperty(nameof(value.Path), value.Path);
        writer.WriteObjectProperty(nameof(value.Extra), value.Extra);
        writer.WriteObjectEnd();
      }
      else
      {
        writer.WriteNull();
      }
    }

    // Reading RobotModel enum value. It could be generated instead.
    public static void ReadValue(this IJSValueReader reader, out RobotModel value)
    {
      value = (RobotModel)reader.ReadValue<int>();
    }

    // Writing RobotModel enum value. It could be generated instead.
    public static void WriteValue(this IJSValueWriter writer, RobotModel value)
    {
      writer.WriteValue((int)value);
    }

    // Reading discriminating union requires using JSValue.
    public static void ReadValue(this JSValue jsValue, out OneOf2<T2Extra, R2D2Extra> value)
    {
      value = default(OneOf2<T2Extra, R2D2Extra>);
      if (jsValue.TryGetObjectProperty("Kind", out JSValue kind))
      {
        RobotModel modelType = kind.ReadValue<RobotModel>();
        switch (modelType)
        {
          case RobotModel.T2: value = jsValue.ReadValue<T2Extra>(); break;
          case RobotModel.R2D2: value = jsValue.ReadValue<R2D2Extra>(); break;
        }
      }
    }

    // Writing discriminating union is simpler than reading.
    public static void WriteValue(this IJSValueWriter writer, OneOf2<T2Extra, R2D2Extra> value)
    {
      writer.WriteObjectBegin();
      if (value.TryGet(out T2Extra t2))
      {
        writer.WriteObjectProperty("Kind", RobotModel.T2);
        writer.WriteObjectProperties(t2);
      }
      else if (value.TryGet(out R2D2Extra r2d2))
      {
        writer.WriteObjectProperty("Kind", RobotModel.R2D2);
        writer.WriteObjectProperties(r2d2);
      }
      writer.WriteObjectEnd();
    }
  }

  [TestClass]
  public class JSValueReaderTest
  {
    [TestMethod]
    public void TestReadCustomType()
    {
      JObject jobj = JObject.Parse(@"{
                Model: 1,
                Name: ""Bob"",
                Age: 42,
                Shape: 1,
                Shape2: 2,
                Shape3: null,
                Steps: [1, 2, 3],
                Dimensions: {Width: 24, Height: 78},
                Badges: [2, ""Maverick"", true],
                Tools: [{Name: ""Screwdriver"", Weight: 2, IsEnabled: true}, {Name: ""Electro-shocker"", Weight: 3, IsEnabled: false}],
                Path: [{X: 5, Y: 6}, {X: 45, Y: 90}, {X: 15, Y: 16}],
                Extra: {Kind: 1, MovieSeries: ""Episode 2""}
            }");
      IJSValueReader reader = new JTokenJSValueReader(jobj);

      reader.ReadValue(out RobotInfo robot);
      Assert.AreEqual(RobotModel.R2D2, robot.Model);
      Assert.AreEqual("Bob", robot.Name);
      Assert.AreEqual(42, robot.Age);
      Assert.AreEqual(RobotShape.Trashcan, robot.Shape);
      Assert.AreEqual(RobotShape.Beercan, robot.Shape2.Value);
      Assert.IsFalse(robot.Shape3.HasValue);
      Assert.AreEqual(3, robot.Steps.Count);
      Assert.AreEqual(1, robot.Steps[0]);
      Assert.AreEqual(2, robot.Steps[1]);
      Assert.AreEqual(3, robot.Steps[2]);
      Assert.AreEqual(2, robot.Dimensions.Count);
      Assert.AreEqual(24, robot.Dimensions["Width"]);
      Assert.AreEqual(78, robot.Dimensions["Height"]);
      Assert.AreEqual(2, robot.Badges.Item1);
      Assert.AreEqual("Maverick", robot.Badges.Item2);
      Assert.AreEqual(true, robot.Badges.Item3);
      Assert.AreEqual(2, robot.Tools.Length);
      Assert.AreEqual("Screwdriver", robot.Tools[0].Name);
      Assert.AreEqual(2, robot.Tools[0].Weight);
      Assert.AreEqual(true, robot.Tools[0].IsEnabled);
      Assert.AreEqual("Electro-shocker", robot.Tools[1].Name);
      Assert.AreEqual(3, robot.Tools[1].Weight);
      Assert.AreEqual(false, robot.Tools[1].IsEnabled);
      Assert.AreEqual(3, robot.Path.Length);
      Assert.AreEqual(5, robot.Path[0].X);
      Assert.AreEqual(6, robot.Path[0].Y);
      Assert.AreEqual(45, robot.Path[1].X);
      Assert.AreEqual(90, robot.Path[1].Y);
      Assert.AreEqual(15, robot.Path[2].X);
      Assert.AreEqual(16, robot.Path[2].Y);
      Assert.AreEqual(true, robot.Extra.TryGet<R2D2Extra>(out var r2d2Extra));
      Assert.AreEqual("Episode 2", r2d2Extra.MovieSeries);
    }

    [TestMethod]
    public void TestWriteCustomType()
    {
      var robot = new RobotInfo
      {
        Model = RobotModel.R2D2,
        Name = "Bob",
        Age = 42,
        Shape = RobotShape.Trashcan,
        Shape2 = RobotShape.Beercan,
        Shape3 = null,
        Steps = new List<int> { 1, 2, 3 },
        Dimensions = new Dictionary<string, int> { ["Width"] = 24, ["Height"] = 78 },
        Badges = Tuple.Create(2, "Maverick", true),
        Tools = new RobotTool[] {
                    new RobotTool { Name = "Screwdriver", Weight = 2, IsEnabled = true},
                    new RobotTool { Name = "Electro-shocker", Weight = 3, IsEnabled = false}
                },
        Path = new RobotPoint[] {
                    new RobotPoint {X = 5, Y = 6},
                    new RobotPoint { X = 45, Y = 90},
                    new RobotPoint { X = 15, Y = 16}
                },
        Extra = new R2D2Extra { MovieSeries = "Episode 2" }
      };

      var writer = new JTokenJSValueWriter();
      writer.WriteValue(robot);
      JToken jValue = writer.TakeValue();

      Assert.AreEqual((int)RobotModel.R2D2, jValue["Model"]);
      Assert.AreEqual("Bob", jValue["Name"]);
      Assert.AreEqual(42, jValue["Age"]);
      Assert.AreEqual((int)RobotShape.Trashcan, jValue["Shape"]);
      Assert.AreEqual((int)RobotShape.Beercan, jValue["Shape2"]);
      Assert.AreEqual(JTokenType.Null, jValue["Shape3"].Type);
      Assert.AreEqual(3, (jValue["Steps"] as JArray).Count);
      Assert.AreEqual(1, jValue["Steps"][0]);
      Assert.AreEqual(2, jValue["Steps"][1]);
      Assert.AreEqual(3, jValue["Steps"][2]);
      Assert.AreEqual(2, (jValue["Dimensions"] as JObject).Count);
      Assert.AreEqual(24, jValue["Dimensions"]["Width"]);
      Assert.AreEqual(78, jValue["Dimensions"]["Height"]);
      Assert.AreEqual(2, jValue["Badges"][0]);
      Assert.AreEqual("Maverick", jValue["Badges"][1]);
      Assert.AreEqual(true, jValue["Badges"][2]);
      Assert.AreEqual(2, (jValue["Tools"] as JArray).Count);
      Assert.AreEqual("Screwdriver", jValue["Tools"][0]["Name"]);
      Assert.AreEqual(2, jValue["Tools"][0]["Weight"]);
      Assert.AreEqual(true, jValue["Tools"][0]["IsEnabled"]);
      Assert.AreEqual("Electro-shocker", jValue["Tools"][1]["Name"]);
      Assert.AreEqual(3, jValue["Tools"][1]["Weight"]);
      Assert.AreEqual(false, jValue["Tools"][1]["IsEnabled"]);
      Assert.AreEqual(3, (jValue["Path"] as JArray).Count);
      Assert.AreEqual(5, jValue["Path"][0]["X"]);
      Assert.AreEqual(6, jValue["Path"][0]["Y"]);
      Assert.AreEqual(45, jValue["Path"][1]["X"]);
      Assert.AreEqual(90, jValue["Path"][1]["Y"]);
      Assert.AreEqual(15, jValue["Path"][2]["X"]);
      Assert.AreEqual(16, jValue["Path"][2]["Y"]);
      Assert.AreEqual("Episode 2", jValue["Extra"]["MovieSeries"]);
    }

    [TestMethod]
    public void TestReadValueDefaultExtensions()
    {
      JObject jobj = JObject.Parse(@"{
                StringValue1: """",
                StringValue2: ""5"",
                StringValue3: ""Hello"",
                BoolValue1: false,
                BoolValue2: true,
                IntValue1: 0,
                IntValue2: 42,
                FloatValue: 3.14,
                NullValue: null
            }");
      IJSValueReader reader = new JTokenJSValueReader(jobj);

      Assert.AreEqual(JSValueType.Object, reader.ValueType);
      int properyCount = 0;
      while (reader.GetNextObjectProperty(out string propertyName))
      {
        switch (propertyName)
        {
          case "StringValue1":
            Assert.AreEqual("", reader.ReadValue<string>());
            Assert.AreEqual(false, reader.ReadValue<bool>());
            Assert.AreEqual((sbyte)0, reader.ReadValue<sbyte>());
            Assert.AreEqual((short)0, reader.ReadValue<short>());
            Assert.AreEqual(0, reader.ReadValue<int>());
            Assert.AreEqual(0, reader.ReadValue<long>());
            Assert.AreEqual((byte)0, reader.ReadValue<byte>());
            Assert.AreEqual((ushort)0, reader.ReadValue<ushort>());
            Assert.AreEqual((uint)0, reader.ReadValue<uint>());
            Assert.AreEqual((ulong)0, reader.ReadValue<ulong>());
            Assert.AreEqual(0, reader.ReadValue<float>());
            Assert.AreEqual(0, reader.ReadValue<double>());
            break;
          case "StringValue2":
            Assert.AreEqual("5", reader.ReadValue<string>());
            Assert.AreEqual(true, reader.ReadValue<bool>());
            Assert.AreEqual((sbyte)5, reader.ReadValue<sbyte>());
            Assert.AreEqual((short)5, reader.ReadValue<short>());
            Assert.AreEqual(5, reader.ReadValue<int>());
            Assert.AreEqual(5, reader.ReadValue<long>());
            Assert.AreEqual((byte)5, reader.ReadValue<byte>());
            Assert.AreEqual((ushort)5, reader.ReadValue<ushort>());
            Assert.AreEqual((uint)5, reader.ReadValue<uint>());
            Assert.AreEqual((ulong)5, reader.ReadValue<ulong>());
            Assert.AreEqual(5, reader.ReadValue<float>());
            Assert.AreEqual(5, reader.ReadValue<double>());
            break;
          case "StringValue3":
            Assert.AreEqual("Hello", reader.ReadValue<string>());
            Assert.AreEqual(true, reader.ReadValue<bool>());
            Assert.AreEqual((sbyte)0, reader.ReadValue<sbyte>());
            Assert.AreEqual((short)0, reader.ReadValue<short>());
            Assert.AreEqual(0, reader.ReadValue<int>());
            Assert.AreEqual(0, reader.ReadValue<long>());
            Assert.AreEqual((byte)0, reader.ReadValue<byte>());
            Assert.AreEqual((ushort)0, reader.ReadValue<ushort>());
            Assert.AreEqual((uint)0, reader.ReadValue<uint>());
            Assert.AreEqual((ulong)0, reader.ReadValue<ulong>());
            Assert.AreEqual(0, reader.ReadValue<float>());
            Assert.AreEqual(0, reader.ReadValue<double>());
            break;
          case "BoolValue1":
            Assert.AreEqual("false", reader.ReadValue<string>());
            Assert.AreEqual(false, reader.ReadValue<bool>());
            Assert.AreEqual((sbyte)0, reader.ReadValue<sbyte>());
            Assert.AreEqual((short)0, reader.ReadValue<short>());
            Assert.AreEqual(0, reader.ReadValue<int>());
            Assert.AreEqual(0, reader.ReadValue<long>());
            Assert.AreEqual((byte)0, reader.ReadValue<byte>());
            Assert.AreEqual((ushort)0, reader.ReadValue<ushort>());
            Assert.AreEqual((uint)0, reader.ReadValue<uint>());
            Assert.AreEqual((ulong)0, reader.ReadValue<ulong>());
            Assert.AreEqual(0, reader.ReadValue<float>());
            Assert.AreEqual(0, reader.ReadValue<double>());
            break;
          case "BoolValue2":
            Assert.AreEqual("true", reader.ReadValue<string>());
            Assert.AreEqual(true, reader.ReadValue<bool>());
            Assert.AreEqual((sbyte)1, reader.ReadValue<sbyte>());
            Assert.AreEqual((short)1, reader.ReadValue<short>());
            Assert.AreEqual(1, reader.ReadValue<int>());
            Assert.AreEqual(1, reader.ReadValue<long>());
            Assert.AreEqual((byte)1, reader.ReadValue<byte>());
            Assert.AreEqual((ushort)1, reader.ReadValue<ushort>());
            Assert.AreEqual((uint)1, reader.ReadValue<uint>());
            Assert.AreEqual((ulong)1, reader.ReadValue<ulong>());
            Assert.AreEqual(1, reader.ReadValue<float>());
            Assert.AreEqual(1, reader.ReadValue<double>());
            break;
          case "IntValue1":
            Assert.AreEqual("0", reader.ReadValue<string>());
            Assert.AreEqual(false, reader.ReadValue<bool>());
            Assert.AreEqual((sbyte)0, reader.ReadValue<sbyte>());
            Assert.AreEqual((short)0, reader.ReadValue<short>());
            Assert.AreEqual(0, reader.ReadValue<int>());
            Assert.AreEqual(0, reader.ReadValue<long>());
            Assert.AreEqual((byte)0, reader.ReadValue<byte>());
            Assert.AreEqual((ushort)0, reader.ReadValue<ushort>());
            Assert.AreEqual((uint)0, reader.ReadValue<uint>());
            Assert.AreEqual((ulong)0, reader.ReadValue<ulong>());
            Assert.AreEqual(0, reader.ReadValue<float>());
            Assert.AreEqual(0, reader.ReadValue<double>());
            break;
          case "IntValue2":
            Assert.AreEqual("42", reader.ReadValue<string>());
            Assert.AreEqual(true, reader.ReadValue<bool>());
            Assert.AreEqual((sbyte)42, reader.ReadValue<sbyte>());
            Assert.AreEqual((short)42, reader.ReadValue<short>());
            Assert.AreEqual(42, reader.ReadValue<int>());
            Assert.AreEqual(42, reader.ReadValue<long>());
            Assert.AreEqual((byte)42, reader.ReadValue<byte>());
            Assert.AreEqual((ushort)42, reader.ReadValue<ushort>());
            Assert.AreEqual((uint)42, reader.ReadValue<uint>());
            Assert.AreEqual((ulong)42, reader.ReadValue<ulong>());
            Assert.AreEqual(42, reader.ReadValue<float>());
            Assert.AreEqual(42, reader.ReadValue<double>());
            break;
          case "FloatValue":
            Assert.AreEqual("3.14", reader.ReadValue<string>());
            Assert.AreEqual(true, reader.ReadValue<bool>());
            Assert.AreEqual((sbyte)3, reader.ReadValue<sbyte>());
            Assert.AreEqual((short)3, reader.ReadValue<short>());
            Assert.AreEqual(3, reader.ReadValue<int>());
            Assert.AreEqual(3, reader.ReadValue<long>());
            Assert.AreEqual((byte)3, reader.ReadValue<byte>());
            Assert.AreEqual((ushort)3, reader.ReadValue<ushort>());
            Assert.AreEqual((uint)3, reader.ReadValue<uint>());
            Assert.AreEqual((ulong)3, reader.ReadValue<ulong>());
            Assert.AreEqual((float)3.14, reader.ReadValue<float>());
            Assert.AreEqual(3.14, reader.ReadValue<double>());
            break;
          case "NullValue":
            Assert.AreEqual("", reader.ReadValue<string>());
            Assert.AreEqual(false, reader.ReadValue<bool>());
            Assert.AreEqual((sbyte)0, reader.ReadValue<sbyte>());
            Assert.AreEqual((short)0, reader.ReadValue<short>());
            Assert.AreEqual(0, reader.ReadValue<int>());
            Assert.AreEqual(0, reader.ReadValue<long>());
            Assert.AreEqual((byte)0, reader.ReadValue<byte>());
            Assert.AreEqual((ushort)0, reader.ReadValue<ushort>());
            Assert.AreEqual((uint)0, reader.ReadValue<uint>());
            Assert.AreEqual((ulong)0, reader.ReadValue<ulong>());
            Assert.AreEqual(0, reader.ReadValue<float>());
            Assert.AreEqual(0, reader.ReadValue<double>());
            break;
        }
        ++properyCount;
      }
      Assert.AreEqual(9, properyCount);
    }

    [TestMethod]
    public void TestWriteValueDefaultExtensions()
    {
      var writer = new JTokenJSValueWriter();
      writer.WriteObjectBegin();
      writer.WriteObjectProperty("StringValue1", "");
      writer.WriteObjectProperty("StringValue2", "5");
      writer.WriteObjectProperty("StringValue3", "Hello");
      writer.WriteObjectProperty("BoolValue1", false);
      writer.WriteObjectProperty("BoolValue2", true);
      writer.WriteObjectProperty("IntValue1", 0);
      writer.WriteObjectProperty("IntValue2", 42);
      writer.WriteObjectProperty("FloatValue", 3.14);
      writer.WriteObjectProperty("NullValue", JSValue.Null);
      writer.WriteObjectEnd();
      JToken jValue = writer.TakeValue();

      Assert.AreEqual("", jValue["StringValue1"]);
      Assert.AreEqual("5", jValue["StringValue2"]);
      Assert.AreEqual("Hello", jValue["StringValue3"]);
      Assert.AreEqual(false, jValue["BoolValue1"]);
      Assert.AreEqual(true, jValue["BoolValue2"]);
      Assert.AreEqual(0, jValue["IntValue1"]);
      Assert.AreEqual(42, jValue["IntValue2"]);
      Assert.AreEqual(3.14, jValue["FloatValue"]);
      Assert.AreEqual(JTokenType.Null, jValue["NullValue"].Type);
    }
  }
}
