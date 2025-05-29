
#include "pch.h"
#include "ReactSupport.h"
#include "Base/FollyIncludes.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

namespace winrt::Microsoft::ReactNative {

folly::dynamic ConvertToDynamic(IInspectable const &object) {
  if (object == nullptr)
    return nullptr;

  if (auto const &map = object.try_as<IMapView<hstring, IInspectable>>()) {
    folly::dynamic obj = folly::dynamic::object;
    for (auto const &kvp : map) {
      obj[to_string(kvp.Key())] = ConvertToDynamic(kvp.Value());
    }
    return obj;
  }

  auto propValue = object.try_as<IPropertyValue>();
  if (!propValue) {
    auto stringable = object.try_as<IStringable>();
    if (stringable) {
      // This handles types such as the Newtonsoft.Json.Linq.JObject and
      // the Windows.Data.Json.JsonObject.
      auto stringified = to_string(stringable.ToString());
      auto json = folly::parseJson(stringified);
      return json;
    } else {
      throw hresult_invalid_argument(L"Unrecognized argument value type.");
    }
  }

  auto propType = propValue.Type();
  folly::dynamic value;

  switch (propType) {
    case PropertyType::Boolean:
      value = propValue.GetBoolean();
      break;
    case PropertyType::Char16:
      value = propValue.GetChar16();
      break;
    case PropertyType::Double:
      value = propValue.GetDouble();
      break;
    case PropertyType::Int16:
      value = propValue.GetInt16();
      break;
    case PropertyType::Int32:
      value = propValue.GetInt32();
      break;
    case PropertyType::Int64:
      value = propValue.GetInt64();
      break;
    case PropertyType::Single:
      value = propValue.GetSingle();
      break;
    case PropertyType::String: {
      hstring stringValue = propValue.GetString();
      value = to_string(stringValue);
      break;
    }
    case PropertyType::UInt8:
      value = propValue.GetUInt8();
      break;
    case PropertyType::UInt16:
      value = propValue.GetUInt16();
      break;
    case PropertyType::UInt32:
      value = propValue.GetUInt32();
      break;
    case PropertyType::UInt64:
      value = propValue.GetUInt64();
      break;

    // ARRAYS
    case PropertyType::BooleanArray: {
      com_array<bool> tmpArray;
      propValue.GetBooleanArray(tmpArray);
      folly::dynamic d = folly::dynamic::array();
      for (bool b : tmpArray) {
        d.push_back(folly::dynamic(b));
      }
      value = d;
      break;
    }
    case PropertyType::Char16Array: {
      com_array<char16_t> tmpArray;
      propValue.GetChar16Array(tmpArray);
      folly::dynamic d = folly::dynamic::array();
      for (char16_t b : tmpArray) {
        d.push_back(folly::dynamic(b));
      }
      value = d;
      break;
    }
    case PropertyType::DoubleArray: {
      com_array<double_t> tmpArray;
      propValue.GetDoubleArray(tmpArray);
      folly::dynamic d = folly::dynamic::array();
      for (double_t b : tmpArray) {
        d.push_back(folly::dynamic(b));
      }
      value = d;
      break;
    }
    case PropertyType::InspectableArray: {
      com_array<IInspectable> tmpArray;
      propValue.GetInspectableArray(tmpArray);
      folly::dynamic d = folly::dynamic::array;
      for (auto inspectable : tmpArray) {
        d.push_back(ConvertToDynamic(inspectable));
      }
      value = d;
      break;
    }
    case PropertyType::Int16Array: {
      com_array<int16_t> tmpArray;
      propValue.GetInt16Array(tmpArray);
      folly::dynamic d = folly::dynamic::array();
      for (int16_t b : tmpArray) {
        d.push_back(folly::dynamic(b));
      }
      value = d;
      break;
    }
    case PropertyType::Int32Array: {
      com_array<int32_t> tmpArray;
      propValue.GetInt32Array(tmpArray);
      folly::dynamic d = folly::dynamic::array();
      for (int32_t b : tmpArray) {
        d.push_back(folly::dynamic(b));
      }
      value = d;
      break;
    }
    case PropertyType::Int64Array: {
      com_array<int64_t> tmpArray;
      propValue.GetInt64Array(tmpArray);
      folly::dynamic d = folly::dynamic::array();
      for (int64_t b : tmpArray) {
        d.push_back(folly::dynamic(b));
      }
      value = d;
      break;
    }
    case PropertyType::SingleArray: {
      com_array<float> tmpArray;
      propValue.GetSingleArray(tmpArray);
      folly::dynamic d = folly::dynamic::array();
      for (float b : tmpArray) {
        d.push_back(folly::dynamic(b));
      }
      value = d;
      break;
    }
    case PropertyType::StringArray: {
      com_array<hstring> tmpArray;
      propValue.GetStringArray(tmpArray);
      folly::dynamic d = folly::dynamic::array;
      for (auto strItem : tmpArray) {
        d.push_back(to_string(strItem));
      }
      value = d;
      break;
    }
    case PropertyType::UInt8Array: {
      com_array<uint8_t> tmpArray;
      propValue.GetUInt8Array(tmpArray);
      folly::dynamic d = folly::dynamic::array();
      for (uint8_t b : tmpArray) {
        d.push_back(folly::dynamic(b));
      }
      value = d;
      break;
    }
    case PropertyType::UInt16Array: {
      com_array<uint16_t> tmpArray;
      propValue.GetUInt16Array(tmpArray);
      folly::dynamic d = folly::dynamic::array();
      for (uint16_t b : tmpArray) {
        d.push_back(folly::dynamic(b));
      }
      value = d;
      break;
    }
    case PropertyType::UInt32Array: {
      com_array<uint32_t> tmpArray;
      propValue.GetUInt32Array(tmpArray);
      folly::dynamic d = folly::dynamic::array();
      for (uint32_t b : tmpArray) {
        d.push_back(folly::dynamic(b));
      }
      value = d;
      break;
    }
    case PropertyType::UInt64Array: {
      com_array<uint64_t> tmpArray;
      propValue.GetUInt64Array(tmpArray);
      folly::dynamic d = folly::dynamic::array();
      for (uint64_t b : tmpArray) {
        d.push_back(folly::dynamic(b));
      }
      value = d;
      break;
    }
    default:
      wchar_t buf[512];
      swprintf(buf, ARRAYSIZE(buf), L"Unrecognized argument value type: %d\n", propType);
      throw hresult_invalid_argument(buf);
  }

  return value;
}

} // namespace winrt::Microsoft::ReactNative
