// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "JSValueReader.h"
#include <variant>
#include "JSValueWriter.h"
#include "JsonJSValueReader.h"

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

TEST_CLASS (JSValueReaderTest) {
  TEST_METHOD(TestReadCustomType) {
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
    TestCheck(robot.Model == RobotModel::R2D2);
    TestCheck(robot.Name == "Bob");
    TestCheck(robot.Age == 42);
    TestCheck(robot.Shape == RobotShape::Trashcan);
    TestCheck(*robot.Shape2 == RobotShape::Beercan);
    TestCheck(!robot.Shape3.has_value());
    TestCheck(robot.Steps.size() == 3);
    TestCheck(robot.Steps[0] == 1);
    TestCheck(robot.Steps[1] == 2);
    TestCheck(robot.Steps[2] == 3);
    TestCheck(robot.Dimensions.size() == 2);
    TestCheck(robot.Dimensions["Width"] == 24);
    TestCheck(robot.Dimensions["Height"] == 78);
    TestCheck(std::get<0>(robot.Badges) == 2);
    TestCheck(std::get<1>(robot.Badges) == "Maverick");
    TestCheck(std::get<2>(robot.Badges) == true);
    TestCheck(robot.Tools.size() == 2);
    TestCheck(robot.Tools[0].Name == "Screwdriver");
    TestCheck(robot.Tools[0].Weight == 2);
    TestCheck(robot.Tools[0].IsEnabled == true);
    TestCheck(robot.Tools[1].Name == "Electro-shocker");
    TestCheck(robot.Tools[1].Weight == 3);
    TestCheck(robot.Tools[1].IsEnabled == false);
    TestCheck(robot.Path.size() == 3);
    TestCheck(robot.Path[0].X == 5);
    TestCheck(robot.Path[0].Y == 6);
    TestCheck(robot.Path[1].X == 45);
    TestCheck(robot.Path[1].Y == 90);
    TestCheck(robot.Path[2].X == 15);
    TestCheck(robot.Path[2].Y == 16);
    const R2D2Extra *r2d2Extra = std::get_if<R2D2Extra>(&robot.Extra);
    TestCheck(r2d2Extra != nullptr);
    TestCheck(r2d2Extra->MovieSeries == "Episode 2");
  }

  TEST_METHOD(TestWriteCustomType) {
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

    auto writer = MakeJSValueTreeWriter();
    WriteValue(writer, robot);
    auto jsValue = TakeJSValue(writer);

    TestCheck(jsValue["Model"] == (int)RobotModel::R2D2);
    TestCheck(jsValue["Name"] == "Bob");
    TestCheck(jsValue["Age"] == 42);
    TestCheck(jsValue["Shape"] == (int)RobotShape::Trashcan);
    TestCheck(jsValue["Shape2"] == (int)RobotShape::Beercan);
    TestCheck(jsValue["Shape3"] == nullptr);
    TestCheck(jsValue["Steps"].ItemCount() == 3);
    TestCheck(jsValue["Steps"][0] == 1);
    TestCheck(jsValue["Steps"][1] == 2);
    TestCheck(jsValue["Steps"][2] == 3);
    TestCheck(jsValue["Dimensions"].PropertyCount() == 2);
    TestCheck(jsValue["Dimensions"]["Width"] == 24);
    TestCheck(jsValue["Dimensions"]["Height"] == 78);
    TestCheck(jsValue["Badges"][0] == 2);
    TestCheck(jsValue["Badges"][1] == "Maverick");
    TestCheck(jsValue["Badges"][2] == true);
    TestCheck(jsValue["Tools"].ItemCount() == 2);
    TestCheck(jsValue["Tools"][0]["Name"] == "Screwdriver");
    TestCheck(jsValue["Tools"][0]["Weight"] == 2);
    TestCheck(jsValue["Tools"][0]["IsEnabled"] == true);
    TestCheck(jsValue["Tools"][1]["Name"] == "Electro-shocker");
    TestCheck(jsValue["Tools"][1]["Weight"] == 3);
    TestCheck(jsValue["Tools"][1]["IsEnabled"] == false);
    TestCheck(jsValue["Path"].ItemCount() == 3);
    TestCheck(jsValue["Path"][0]["X"] == 5);
    TestCheck(jsValue["Path"][0]["Y"] == 6);
    TestCheck(jsValue["Path"][1]["X"] == 45);
    TestCheck(jsValue["Path"][1]["Y"] == 90);
    TestCheck(jsValue["Path"][2]["X"] == 15);
    TestCheck(jsValue["Path"][2]["Y"] == 16);
    TestCheck(jsValue["Extra"]["MovieSeries"] == "Episode 2");
  }

  TEST_METHOD(TestReadValueDefaultExtensions) {
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

    TestCheck(reader.ValueType() == JSValueType::Object);
    int properyCount = 0;
    hstring propertyName;
    while (reader.GetNextObjectProperty(/*out*/ propertyName)) {
      if (propertyName == L"StringValue1") {
        TestCheck(ReadValue<std::string>(reader) == "");
        TestCheck(ReadValue<std::wstring>(reader) == L"");
        TestCheck(ReadValue<bool>(reader) == false);
        TestCheck(ReadValue<int8_t>(reader) == 0);
        TestCheck(ReadValue<int16_t>(reader) == 0);
        TestCheck(ReadValue<int32_t>(reader) == 0);
        TestCheck(ReadValue<int64_t>(reader) == 0);
        TestCheck(ReadValue<uint8_t>(reader) == 0);
        TestCheck(ReadValue<uint16_t>(reader) == 0);
        TestCheck(ReadValue<uint32_t>(reader) == 0);
        TestCheck(ReadValue<uint64_t>(reader) == 0);
        TestCheck(ReadValue<float>(reader) == 0);
        TestCheck(ReadValue<double>(reader) == 0);
        ++properyCount;
      } else if (propertyName == L"StringValue2") {
        TestCheck(ReadValue<std::string>(reader) == "5");
        TestCheck(ReadValue<std::wstring>(reader) == L"5");
        TestCheck(ReadValue<bool>(reader) == true);
        TestCheck(ReadValue<int8_t>(reader) == 5);
        TestCheck(ReadValue<int16_t>(reader) == 5);
        TestCheck(ReadValue<int32_t>(reader) == 5);
        TestCheck(ReadValue<int64_t>(reader) == 5);
        TestCheck(ReadValue<uint8_t>(reader) == 5);
        TestCheck(ReadValue<uint16_t>(reader) == 5);
        TestCheck(ReadValue<uint32_t>(reader) == 5);
        TestCheck(ReadValue<uint64_t>(reader) == 5);
        TestCheck(ReadValue<float>(reader) == 5);
        TestCheck(ReadValue<double>(reader) == 5);
        ++properyCount;
      } else if (propertyName == L"StringValue3") {
        TestCheck(ReadValue<std::string>(reader) == "Hello");
        TestCheck(ReadValue<std::wstring>(reader) == L"Hello");
        TestCheck(ReadValue<bool>(reader) == true);
        TestCheck(ReadValue<int8_t>(reader) == 0);
        TestCheck(ReadValue<int16_t>(reader) == 0);
        TestCheck(ReadValue<int32_t>(reader) == 0);
        TestCheck(ReadValue<int64_t>(reader) == 0);
        TestCheck(ReadValue<uint8_t>(reader) == 0);
        TestCheck(ReadValue<uint16_t>(reader) == 0);
        TestCheck(ReadValue<uint32_t>(reader) == 0);
        TestCheck(ReadValue<uint64_t>(reader) == 0);
        TestCheck(ReadValue<float>(reader) == 0);
        TestCheck(ReadValue<double>(reader) == 0);
        ++properyCount;
      } else if (propertyName == L"BoolValue1") {
        TestCheck(ReadValue<std::string>(reader) == "false");
        TestCheck(ReadValue<std::wstring>(reader) == L"false");
        TestCheck(ReadValue<bool>(reader) == false);
        TestCheck(ReadValue<int8_t>(reader) == 0);
        TestCheck(ReadValue<int16_t>(reader) == 0);
        TestCheck(ReadValue<int32_t>(reader) == 0);
        TestCheck(ReadValue<int64_t>(reader) == 0);
        TestCheck(ReadValue<uint8_t>(reader) == 0);
        TestCheck(ReadValue<uint16_t>(reader) == 0);
        TestCheck(ReadValue<uint32_t>(reader) == 0);
        TestCheck(ReadValue<uint64_t>(reader) == 0);
        TestCheck(ReadValue<float>(reader) == 0);
        TestCheck(ReadValue<double>(reader) == 0);
        ++properyCount;
      } else if (propertyName == L"BoolValue2") {
        TestCheck(ReadValue<std::string>(reader) == "true");
        TestCheck(ReadValue<std::wstring>(reader) == L"true");
        TestCheck(ReadValue<bool>(reader) == true);
        TestCheck(ReadValue<int8_t>(reader) == 1);
        TestCheck(ReadValue<int16_t>(reader) == 1);
        TestCheck(ReadValue<int32_t>(reader) == 1);
        TestCheck(ReadValue<int64_t>(reader) == 1);
        TestCheck(ReadValue<uint8_t>(reader) == 1);
        TestCheck(ReadValue<uint16_t>(reader) == 1);
        TestCheck(ReadValue<uint32_t>(reader) == 1);
        TestCheck(ReadValue<uint64_t>(reader) == 1);
        TestCheck(ReadValue<float>(reader) == 1);
        TestCheck(ReadValue<double>(reader) == 1);
        ++properyCount;
      } else if (propertyName == L"IntValue1") {
        TestCheck(ReadValue<std::string>(reader) == "0");
        TestCheck(ReadValue<std::wstring>(reader) == L"0");
        TestCheck(ReadValue<bool>(reader) == false);
        TestCheck(ReadValue<int8_t>(reader) == 0);
        TestCheck(ReadValue<int16_t>(reader) == 0);
        TestCheck(ReadValue<int32_t>(reader) == 0);
        TestCheck(ReadValue<int64_t>(reader) == 0);
        TestCheck(ReadValue<uint8_t>(reader) == 0);
        TestCheck(ReadValue<uint16_t>(reader) == 0);
        TestCheck(ReadValue<uint32_t>(reader) == 0);
        TestCheck(ReadValue<uint64_t>(reader) == 0);
        TestCheck(ReadValue<float>(reader) == 0);
        TestCheck(ReadValue<double>(reader) == 0);
        ++properyCount;
      } else if (propertyName == L"IntValue2") {
        TestCheck(ReadValue<std::string>(reader) == "42");
        TestCheck(ReadValue<std::wstring>(reader) == L"42");
        TestCheck(ReadValue<bool>(reader) == true);
        TestCheck(ReadValue<int8_t>(reader) == 42);
        TestCheck(ReadValue<int16_t>(reader) == 42);
        TestCheck(ReadValue<int32_t>(reader) == 42);
        TestCheck(ReadValue<int64_t>(reader) == 42);
        TestCheck(ReadValue<uint8_t>(reader) == 42);
        TestCheck(ReadValue<uint16_t>(reader) == 42);
        TestCheck(ReadValue<uint32_t>(reader) == 42);
        TestCheck(ReadValue<uint64_t>(reader) == 42);
        TestCheck(ReadValue<float>(reader) == 42);
        TestCheck(ReadValue<double>(reader) == 42);
        ++properyCount;
      } else if (propertyName == L"FloatValue") {
        TestCheck(ReadValue<std::string>(reader) == "3.14");
        TestCheck(ReadValue<std::wstring>(reader) == L"3.14");
        TestCheck(ReadValue<bool>(reader) == true);
        TestCheck(ReadValue<int8_t>(reader) == 3);
        TestCheck(ReadValue<int16_t>(reader) == 3);
        TestCheck(ReadValue<int32_t>(reader) == 3);
        TestCheck(ReadValue<int64_t>(reader) == 3);
        TestCheck(ReadValue<uint8_t>(reader) == 3);
        TestCheck(ReadValue<uint16_t>(reader) == 3);
        TestCheck(ReadValue<uint32_t>(reader) == 3);
        TestCheck(ReadValue<uint64_t>(reader) == 3);
        TestCheck(ReadValue<float>(reader) == 3.14f);
        TestCheck(ReadValue<double>(reader) == 3.14);
        ++properyCount;
      } else if (propertyName == L"NullValue") {
        TestCheck(ReadValue<std::string>(reader) == "");
        TestCheck(ReadValue<std::wstring>(reader) == L"");
        TestCheck(ReadValue<bool>(reader) == false);
        TestCheck(ReadValue<int8_t>(reader) == 0);
        TestCheck(ReadValue<int16_t>(reader) == 0);
        TestCheck(ReadValue<int32_t>(reader) == 0);
        TestCheck(ReadValue<int64_t>(reader) == 0);
        TestCheck(ReadValue<uint8_t>(reader) == 0);
        TestCheck(ReadValue<uint16_t>(reader) == 0);
        TestCheck(ReadValue<uint32_t>(reader) == 0);
        TestCheck(ReadValue<uint64_t>(reader) == 0);
        TestCheck(ReadValue<float>(reader) == 0);
        TestCheck(ReadValue<double>(reader) == 0);

        ++properyCount;
      }
    }
    TestCheck(properyCount == 9);
  }

  TEST_METHOD(TestWriteValueDefaultExtensions) {
    auto writer = MakeJSValueTreeWriter();
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

    auto jsValue = TakeJSValue(writer);
    TestCheck(jsValue["StringValue1"] == "");
    TestCheck(jsValue["StringValue2"] == "5");
    TestCheck(jsValue["StringValue3"] == "Hello");
    TestCheck(jsValue["BoolValue1"] == false);
    TestCheck(jsValue["BoolValue2"] == true);
    TestCheck(jsValue["IntValue1"] == 0);
    TestCheck(jsValue["IntValue2"] == 42);
    TestCheck(jsValue["FloatValue"] == 3.14);
    TestCheck(jsValue["NullValue"] == nullptr);
    TestCheck(jsValue["NullValue"] == JSValue::Null);
  }
};

} // namespace winrt::Microsoft::ReactNative
