// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "JSValueReader.h"
#include <variant>
#include "JSValueWriter.h"
#include "JsonJSValueReader.h"
#include "catch.hpp"

namespace winrt::Microsoft::ReactNative {

enum struct RobotModel {
  T2,
  R2D2,
  C3PO,
};

// We are generating ReadValue for it.
enum struct RobotShape {
  Humanoid,
  Trashcan,
  Beercan,
  Quadrocopter,
};

struct RobotTool {
  std::string Name;
  int Weight;
  bool IsEnabled;
};

// This is how we can provide "reflection" information about type outside of struct.
FieldMap GetStructInfo(RobotTool *) {
  return {{L"Name", &RobotTool::Name}, {L"Weight", &RobotTool::Weight}, {L"IsEnabled", &RobotTool::IsEnabled}};
}

struct RobotPoint {
  int X;
  int Y;
};

FieldMap GetStructInfo(RobotPoint *) {
  return {{L"X", &RobotPoint::X}, {L"Y", &RobotPoint::Y}};
}

// We can provide "reflection" information about struct using REACT_STRUCT and REACT_FIELD macros.
REACT_STRUCT(T2Extra)
struct T2Extra {
  REACT_FIELD(ActorName)
  std::string ActorName;

  REACT_FIELD(MovieYear)
  int MovieYear;
};

REACT_STRUCT(R2D2Extra)
struct R2D2Extra {
  REACT_FIELD(MovieSeries)
  std::string MovieSeries;
};

struct RobotInfo {
  RobotModel Model;
  std::string Name;
  int Age;
  RobotShape Shape;
  std::optional<RobotShape> Shape2;
  std::optional<RobotShape> Shape3;
  std::vector<int> Steps;
  std::map<std::string, int> Dimensions;
  std::tuple<int, std::string, bool> Badges;
  std::vector<RobotTool> Tools;
  std::vector<RobotPoint> Path;
  std::variant<T2Extra, R2D2Extra> Extra;
};

// Reading RobotModel enum value. We could use template-based version instead.
void ReadValue(IJSValueReader const &reader, RobotModel &value) noexcept {
  value = static_cast<RobotModel>(ReadValue<int>(reader));
}

// Writing RobotModel enum value. We could use template-based version instead.
void WriteValue(IJSValueWriter const &writer, RobotModel value) noexcept {
  WriteValue(writer, static_cast<int>(value));
}

// Reading discriminating union requires using JSValue.
void ReadValue(const JSValue &jsValue, std::variant<T2Extra, R2D2Extra> &value) noexcept {
  switch (ReadValue<RobotModel>(jsValue["Kind"])) {
    case RobotModel::T2:
      value = ReadValue<T2Extra>(jsValue);
      break;
    case RobotModel::R2D2:
      value = ReadValue<R2D2Extra>(jsValue);
      break;
  }
}

// Writing discriminating union.
void WriteValue(IJSValueWriter const &writer, std::variant<T2Extra, R2D2Extra> const &value) noexcept {
  writer.WriteObjectBegin();
  if (const T2Extra *t2 = std::get_if<T2Extra>(&value)) {
    WriteProperty(writer, L"Kind", RobotModel::T2);
    WriteProperties(writer, *t2);
  } else if (const R2D2Extra *r2d2 = std::get_if<R2D2Extra>(&value)) {
    WriteProperty(writer, L"Kind", RobotModel::R2D2);
    WriteProperties(writer, *r2d2);
  }
  writer.WriteObjectEnd();
}

// Reading RobotInfo value. It could be generated instead.
void ReadValue(IJSValueReader const &reader, RobotInfo &value) noexcept {
  value = RobotInfo();
  if (reader.ValueType() == JSValueType::Object) {
    hstring propertyName;
    while (reader.GetNextObjectProperty(/*out*/ propertyName)) {
      if (propertyName == L"Model")
        ReadValue(reader, value.Model);
      else if (propertyName == L"Name")
        ReadValue(reader, value.Name);
      else if (propertyName == L"Age")
        ReadValue(reader, value.Age);
      else if (propertyName == L"Shape")
        ReadValue(reader, value.Shape);
      else if (propertyName == L"Shape2")
        ReadValue(reader, value.Shape2);
      else if (propertyName == L"Shape3")
        ReadValue(reader, value.Shape3);
      else if (propertyName == L"Steps")
        ReadValue(reader, value.Steps);
      else if (propertyName == L"Dimensions")
        ReadValue(reader, value.Dimensions);
      else if (propertyName == L"Badges")
        ReadValue(reader, value.Badges);
      else if (propertyName == L"Tools")
        ReadValue(reader, value.Tools);
      else if (propertyName == L"Path")
        ReadValue(reader, value.Path);
      else if (propertyName == L"Extra")
        ReadValue(reader, value.Extra);
      else
        ReadValue<JSValue>(reader);
    }
  }
}

// Writing RobotInfo value. It could be generated instead.
void WriteValue(IJSValueWriter const &writer, RobotInfo const &value) noexcept {
  writer.WriteObjectBegin();
  WriteProperty(writer, L"Model", value.Model);
  WriteProperty(writer, L"Name", value.Name);
  WriteProperty(writer, L"Age", value.Age);
  WriteProperty(writer, L"Shape", value.Shape);
  WriteProperty(writer, L"Shape2", value.Shape2);
  WriteProperty(writer, L"Shape3", value.Shape3);
  WriteProperty(writer, L"Steps", value.Steps);
  WriteProperty(writer, L"Dimensions", value.Dimensions);
  WriteProperty(writer, L"Badges", value.Badges);
  WriteProperty(writer, L"Tools", value.Tools);
  WriteProperty(writer, L"Path", value.Path);
  WriteProperty(writer, L"Extra", value.Extra);
  writer.WriteObjectEnd();
}

TEST_CASE("TestReadCustomType", "JSValueReaderTest") {
  const wchar_t *json =
      LR"JSON({
        "Model": 1,
        "Name": "Bob",
        "Age": 42,
        "Shape": 1,
        "Shape2": 2,
        "Shape3": null,
        "Steps": [1, 2, 3],
        "Dimensions": {"Width": 24, "Height": 78},
        "Badges": [2, "Maverick", true],
        "Tools": [
          {"Name": "Screwdriver", "Weight": 2, "IsEnabled": true},
          {"Name": "Electro-shocker", "Weight": 3, "IsEnabled": false}],
        "Path": [{"X": 5, "Y": 6}, {"X": 45, "Y": 90}, {"X": 15, "Y": 16}],
        "Extra": {"Kind": 1, "MovieSeries" : "Episode 2"}
    })JSON";

  IJSValueReader reader = make<JsonJSValueReader>(json);

  RobotInfo robot = ReadValue<RobotInfo>(reader);
  REQUIRE(robot.Model == RobotModel::R2D2);
  REQUIRE(robot.Name == "Bob");
  REQUIRE(robot.Age == 42);
  REQUIRE(robot.Shape == RobotShape::Trashcan);
  REQUIRE(*robot.Shape2 == RobotShape::Beercan);
  REQUIRE(!robot.Shape3.has_value());
  REQUIRE(robot.Steps.size() == 3);
  REQUIRE(robot.Steps[0] == 1);
  REQUIRE(robot.Steps[1] == 2);
  REQUIRE(robot.Steps[2] == 3);
  REQUIRE(robot.Dimensions.size() == 2);
  REQUIRE(robot.Dimensions["Width"] == 24);
  REQUIRE(robot.Dimensions["Height"] == 78);
  REQUIRE(std::get<0>(robot.Badges) == 2);
  REQUIRE(std::get<1>(robot.Badges) == "Maverick");
  REQUIRE(std::get<2>(robot.Badges) == true);
  REQUIRE(robot.Tools.size() == 2);
  REQUIRE(robot.Tools[0].Name == "Screwdriver");
  REQUIRE(robot.Tools[0].Weight == 2);
  REQUIRE(robot.Tools[0].IsEnabled == true);
  REQUIRE(robot.Tools[1].Name == "Electro-shocker");
  REQUIRE(robot.Tools[1].Weight == 3);
  REQUIRE(robot.Tools[1].IsEnabled == false);
  REQUIRE(robot.Path.size() == 3);
  REQUIRE(robot.Path[0].X == 5);
  REQUIRE(robot.Path[0].Y == 6);
  REQUIRE(robot.Path[1].X == 45);
  REQUIRE(robot.Path[1].Y == 90);
  REQUIRE(robot.Path[2].X == 15);
  REQUIRE(robot.Path[2].Y == 16);
  const R2D2Extra *r2d2Extra = std::get_if<R2D2Extra>(&robot.Extra);
  REQUIRE(r2d2Extra != nullptr);
  REQUIRE(r2d2Extra->MovieSeries == "Episode 2");
}

TEST_CASE("TestWriteCustomType", "JSValueReaderTest") {
  RobotInfo robot{};
  robot.Model = RobotModel::R2D2;
  robot.Name = "Bob";
  robot.Age = 42;
  robot.Shape = RobotShape::Trashcan;
  robot.Shape2 = RobotShape::Beercan;
  robot.Shape3 = std::nullopt;
  robot.Steps = std::vector<int>{1, 2, 3};
  robot.Dimensions = std::map<std::string, int>{{"Width", 24}, {"Height", 78}};
  robot.Badges = std::tuple<int, std::string, bool>{2, "Maverick", true};
  robot.Tools = std::vector<RobotTool>{RobotTool{/*Name =*/"Screwdriver", /*Weight =*/2, /*IsEnabled =*/true},
                                       RobotTool{/*Name =*/"Electro-shocker", /*Weight =*/3, /*IsEnabled =*/false}};
  robot.Path = std::vector<RobotPoint>{
      RobotPoint{/*X =*/5, /*Y =*/6}, RobotPoint{/*X =*/45, /*Y =*/90}, RobotPoint{/*X =*/15, /*Y =*/16}};
  robot.Extra = R2D2Extra{/*MovieSeries =*/"Episode 2"};

  JSValue jsValue;
  auto writer = MakeJSValueTreeWriter(jsValue);
  WriteValue(writer, robot);

  REQUIRE(jsValue["Model"] == (int)RobotModel::R2D2);
  REQUIRE(jsValue["Name"] == "Bob");
  REQUIRE(jsValue["Age"] == 42);
  REQUIRE(jsValue["Shape"] == (int)RobotShape::Trashcan);
  REQUIRE(jsValue["Shape2"] == (int)RobotShape::Beercan);
  REQUIRE(jsValue["Shape3"] == nullptr);
  REQUIRE(jsValue["Steps"].ItemCount() == 3);
  REQUIRE(jsValue["Steps"][0] == 1);
  REQUIRE(jsValue["Steps"][1] == 2);
  REQUIRE(jsValue["Steps"][2] == 3);
  REQUIRE(jsValue["Dimensions"].PropertyCount() == 2);
  REQUIRE(jsValue["Dimensions"]["Width"] == 24);
  REQUIRE(jsValue["Dimensions"]["Height"] == 78);
  REQUIRE(jsValue["Badges"][0] == 2);
  REQUIRE(jsValue["Badges"][1] == "Maverick");
  REQUIRE(jsValue["Badges"][2] == true);
  REQUIRE(jsValue["Tools"].ItemCount() == 2);
  REQUIRE(jsValue["Tools"][0]["Name"] == "Screwdriver");
  REQUIRE(jsValue["Tools"][0]["Weight"] == 2);
  REQUIRE(jsValue["Tools"][0]["IsEnabled"] == true);
  REQUIRE(jsValue["Tools"][1]["Name"] == "Electro-shocker");
  REQUIRE(jsValue["Tools"][1]["Weight"] == 3);
  REQUIRE(jsValue["Tools"][1]["IsEnabled"] == false);
  REQUIRE(jsValue["Path"].ItemCount() == 3);
  REQUIRE(jsValue["Path"][0]["X"] == 5);
  REQUIRE(jsValue["Path"][0]["Y"] == 6);
  REQUIRE(jsValue["Path"][1]["X"] == 45);
  REQUIRE(jsValue["Path"][1]["Y"] == 90);
  REQUIRE(jsValue["Path"][2]["X"] == 15);
  REQUIRE(jsValue["Path"][2]["Y"] == 16);
  REQUIRE(jsValue["Extra"]["MovieSeries"] == "Episode 2");
}

TEST_CASE("TestReadValueDefaultExtensions", "JSValueReaderTest") {
  const wchar_t *json =
      LR"JSON({
      "StringValue1": "",
      "StringValue2": "5",
      "StringValue3": "Hello",
      "BoolValue1": false,
      "BoolValue2": true,
      "IntValue1": 0,
      "IntValue2": 42,
      "FloatValue": 3.14,
      "NullValue": null
    })JSON";

  IJSValueReader reader = make<JsonJSValueReader>(json);

  REQUIRE(reader.ValueType() == JSValueType::Object);
  int properyCount = 0;
  hstring propertyName;
  while (reader.GetNextObjectProperty(/*out*/ propertyName)) {
    if (propertyName == L"StringValue1") {
      REQUIRE(ReadValue<std::string>(reader) == "");
      REQUIRE(ReadValue<std::wstring>(reader) == L"");
      REQUIRE(ReadValue<bool>(reader) == false);
      REQUIRE(ReadValue<int8_t>(reader) == 0);
      REQUIRE(ReadValue<int16_t>(reader) == 0);
      REQUIRE(ReadValue<int32_t>(reader) == 0);
      REQUIRE(ReadValue<int64_t>(reader) == 0);
      REQUIRE(ReadValue<uint8_t>(reader) == 0);
      REQUIRE(ReadValue<uint16_t>(reader) == 0);
      REQUIRE(ReadValue<uint32_t>(reader) == 0);
      REQUIRE(ReadValue<uint64_t>(reader) == 0);
      REQUIRE(ReadValue<float>(reader) == 0);
      REQUIRE(ReadValue<double>(reader) == 0);
      ++properyCount;
    } else if (propertyName == L"StringValue2") {
      REQUIRE(ReadValue<std::string>(reader) == "5");
      REQUIRE(ReadValue<std::wstring>(reader) == L"5");
      REQUIRE(ReadValue<bool>(reader) == true);
      REQUIRE(ReadValue<int8_t>(reader) == 5);
      REQUIRE(ReadValue<int16_t>(reader) == 5);
      REQUIRE(ReadValue<int32_t>(reader) == 5);
      REQUIRE(ReadValue<int64_t>(reader) == 5);
      REQUIRE(ReadValue<uint8_t>(reader) == 5);
      REQUIRE(ReadValue<uint16_t>(reader) == 5);
      REQUIRE(ReadValue<uint32_t>(reader) == 5);
      REQUIRE(ReadValue<uint64_t>(reader) == 5);
      REQUIRE(ReadValue<float>(reader) == 5);
      REQUIRE(ReadValue<double>(reader) == 5);
      ++properyCount;
    } else if (propertyName == L"StringValue3") {
      REQUIRE(ReadValue<std::string>(reader) == "Hello");
      REQUIRE(ReadValue<std::wstring>(reader) == L"Hello");
      REQUIRE(ReadValue<bool>(reader) == true);
      REQUIRE(ReadValue<int8_t>(reader) == 0);
      REQUIRE(ReadValue<int16_t>(reader) == 0);
      REQUIRE(ReadValue<int32_t>(reader) == 0);
      REQUIRE(ReadValue<int64_t>(reader) == 0);
      REQUIRE(ReadValue<uint8_t>(reader) == 0);
      REQUIRE(ReadValue<uint16_t>(reader) == 0);
      REQUIRE(ReadValue<uint32_t>(reader) == 0);
      REQUIRE(ReadValue<uint64_t>(reader) == 0);
      REQUIRE(ReadValue<float>(reader) == 0);
      REQUIRE(ReadValue<double>(reader) == 0);
      ++properyCount;
    } else if (propertyName == L"BoolValue1") {
      REQUIRE(ReadValue<std::string>(reader) == "false");
      REQUIRE(ReadValue<std::wstring>(reader) == L"false");
      REQUIRE(ReadValue<bool>(reader) == false);
      REQUIRE(ReadValue<int8_t>(reader) == 0);
      REQUIRE(ReadValue<int16_t>(reader) == 0);
      REQUIRE(ReadValue<int32_t>(reader) == 0);
      REQUIRE(ReadValue<int64_t>(reader) == 0);
      REQUIRE(ReadValue<uint8_t>(reader) == 0);
      REQUIRE(ReadValue<uint16_t>(reader) == 0);
      REQUIRE(ReadValue<uint32_t>(reader) == 0);
      REQUIRE(ReadValue<uint64_t>(reader) == 0);
      REQUIRE(ReadValue<float>(reader) == 0);
      REQUIRE(ReadValue<double>(reader) == 0);
      ++properyCount;
    } else if (propertyName == L"BoolValue2") {
      REQUIRE(ReadValue<std::string>(reader) == "true");
      REQUIRE(ReadValue<std::wstring>(reader) == L"true");
      REQUIRE(ReadValue<bool>(reader) == true);
      REQUIRE(ReadValue<int8_t>(reader) == 1);
      REQUIRE(ReadValue<int16_t>(reader) == 1);
      REQUIRE(ReadValue<int32_t>(reader) == 1);
      REQUIRE(ReadValue<int64_t>(reader) == 1);
      REQUIRE(ReadValue<uint8_t>(reader) == 1);
      REQUIRE(ReadValue<uint16_t>(reader) == 1);
      REQUIRE(ReadValue<uint32_t>(reader) == 1);
      REQUIRE(ReadValue<uint64_t>(reader) == 1);
      REQUIRE(ReadValue<float>(reader) == 1);
      REQUIRE(ReadValue<double>(reader) == 1);
      ++properyCount;
    } else if (propertyName == L"IntValue1") {
      REQUIRE(ReadValue<std::string>(reader) == "0");
      REQUIRE(ReadValue<std::wstring>(reader) == L"0");
      REQUIRE(ReadValue<bool>(reader) == false);
      REQUIRE(ReadValue<int8_t>(reader) == 0);
      REQUIRE(ReadValue<int16_t>(reader) == 0);
      REQUIRE(ReadValue<int32_t>(reader) == 0);
      REQUIRE(ReadValue<int64_t>(reader) == 0);
      REQUIRE(ReadValue<uint8_t>(reader) == 0);
      REQUIRE(ReadValue<uint16_t>(reader) == 0);
      REQUIRE(ReadValue<uint32_t>(reader) == 0);
      REQUIRE(ReadValue<uint64_t>(reader) == 0);
      REQUIRE(ReadValue<float>(reader) == 0);
      REQUIRE(ReadValue<double>(reader) == 0);
      ++properyCount;
    } else if (propertyName == L"IntValue2") {
      REQUIRE(ReadValue<std::string>(reader) == "42");
      REQUIRE(ReadValue<std::wstring>(reader) == L"42");
      REQUIRE(ReadValue<bool>(reader) == true);
      REQUIRE(ReadValue<int8_t>(reader) == 42);
      REQUIRE(ReadValue<int16_t>(reader) == 42);
      REQUIRE(ReadValue<int32_t>(reader) == 42);
      REQUIRE(ReadValue<int64_t>(reader) == 42);
      REQUIRE(ReadValue<uint8_t>(reader) == 42);
      REQUIRE(ReadValue<uint16_t>(reader) == 42);
      REQUIRE(ReadValue<uint32_t>(reader) == 42);
      REQUIRE(ReadValue<uint64_t>(reader) == 42);
      REQUIRE(ReadValue<float>(reader) == 42);
      REQUIRE(ReadValue<double>(reader) == 42);
      ++properyCount;
    } else if (propertyName == L"FloatValue") {
      REQUIRE(ReadValue<std::string>(reader) == "3.14");
      REQUIRE(ReadValue<std::wstring>(reader) == L"3.14");
      REQUIRE(ReadValue<bool>(reader) == true);
      REQUIRE(ReadValue<int8_t>(reader) == 3);
      REQUIRE(ReadValue<int16_t>(reader) == 3);
      REQUIRE(ReadValue<int32_t>(reader) == 3);
      REQUIRE(ReadValue<int64_t>(reader) == 3);
      REQUIRE(ReadValue<uint8_t>(reader) == 3);
      REQUIRE(ReadValue<uint16_t>(reader) == 3);
      REQUIRE(ReadValue<uint32_t>(reader) == 3);
      REQUIRE(ReadValue<uint64_t>(reader) == 3);
      REQUIRE(ReadValue<float>(reader) == 3.14f);
      REQUIRE(ReadValue<double>(reader) == 3.14);
      ++properyCount;
    } else if (propertyName == L"NullValue") {
      REQUIRE(ReadValue<std::string>(reader) == "");
      REQUIRE(ReadValue<std::wstring>(reader) == L"");
      REQUIRE(ReadValue<bool>(reader) == false);
      REQUIRE(ReadValue<int8_t>(reader) == 0);
      REQUIRE(ReadValue<int16_t>(reader) == 0);
      REQUIRE(ReadValue<int32_t>(reader) == 0);
      REQUIRE(ReadValue<int64_t>(reader) == 0);
      REQUIRE(ReadValue<uint8_t>(reader) == 0);
      REQUIRE(ReadValue<uint16_t>(reader) == 0);
      REQUIRE(ReadValue<uint32_t>(reader) == 0);
      REQUIRE(ReadValue<uint64_t>(reader) == 0);
      REQUIRE(ReadValue<float>(reader) == 0);
      REQUIRE(ReadValue<double>(reader) == 0);

      ++properyCount;
    }
  }
  REQUIRE(properyCount == 9);
}

TEST_CASE("TestWriteValueDefaultExtensions", "JSValueReaderTest") {
  JSValue jsValue;
  auto writer = MakeJSValueTreeWriter(jsValue);
  writer.WriteObjectBegin();
  WriteProperty(writer, L"StringValue1", "");
  WriteProperty(writer, L"StringValue2", "5");
  WriteProperty(writer, L"StringValue3", "Hello");
  WriteProperty(writer, L"BoolValue1", false);
  WriteProperty(writer, L"BoolValue2", true);
  WriteProperty(writer, L"IntValue1", 0);
  WriteProperty(writer, L"IntValue2", 42);
  WriteProperty(writer, L"FloatValue", 3.14);
  WriteProperty(writer, L"NullValue", nullptr);
  writer.WriteObjectEnd();

  REQUIRE(jsValue["StringValue1"] == "");
  REQUIRE(jsValue["StringValue2"] == "5");
  REQUIRE(jsValue["StringValue3"] == "Hello");
  REQUIRE(jsValue["BoolValue1"] == false);
  REQUIRE(jsValue["BoolValue2"] == true);
  REQUIRE(jsValue["IntValue1"] == 0);
  REQUIRE(jsValue["IntValue2"] == 42);
  REQUIRE(jsValue["FloatValue"] == 3.14);
  REQUIRE(jsValue["NullValue"] == nullptr);
  REQUIRE(jsValue["NullValue"] == JSValue::Null);
}

} // namespace winrt::Microsoft::ReactNative
