
#include "pch.h"
#include "ReactSupport.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

namespace winrt::Microsoft::ReactNative::Bridge {

folly::dynamic ConvertToDynamic(IInspectable const &object) {
  if (object == nullptr)
    return nullptr;

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
      folly::dynamic d(tmpArray.begin(), tmpArray.end());
      value = d;
      break;
    }
    case PropertyType::Char16Array: {
      com_array<char16_t> tmpArray;
      propValue.GetChar16Array(tmpArray);
      folly::dynamic d(tmpArray.begin(), tmpArray.end());
      value = d;
      break;
    }
    case PropertyType::DoubleArray: {
      com_array<double_t> tmpArray;
      propValue.GetDoubleArray(tmpArray);
      folly::dynamic d(tmpArray.begin(), tmpArray.end());
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
      folly::dynamic d(tmpArray.begin(), tmpArray.end());
      value = d;
      break;
    }
    case PropertyType::Int32Array: {
      com_array<int32_t> tmpArray;
      propValue.GetInt32Array(tmpArray);
      folly::dynamic d(tmpArray.begin(), tmpArray.end());
      value = d;
      break;
    }
    case PropertyType::Int64Array: {
      com_array<int64_t> tmpArray;
      propValue.GetInt64Array(tmpArray);
      folly::dynamic d(tmpArray.begin(), tmpArray.end());
      value = d;
      break;
    }
    case PropertyType::SingleArray: {
      com_array<float> tmpArray;
      propValue.GetSingleArray(tmpArray);
      folly::dynamic d(tmpArray.begin(), tmpArray.end());
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
      folly::dynamic d(tmpArray.begin(), tmpArray.end());
      value = d;
      break;
    }
    case PropertyType::UInt16Array: {
      com_array<uint16_t> tmpArray;
      propValue.GetUInt16Array(tmpArray);
      folly::dynamic d(tmpArray.begin(), tmpArray.end());
      value = d;
      break;
    }
    case PropertyType::UInt32Array: {
      com_array<uint32_t> tmpArray;
      propValue.GetUInt32Array(tmpArray);
      folly::dynamic d(tmpArray.begin(), tmpArray.end());
      value = d;
      break;
    }
    case PropertyType::UInt64Array: {
      com_array<uint64_t> tmpArray;
      propValue.GetUInt64Array(tmpArray);
      folly::dynamic d(tmpArray.begin(), tmpArray.end());
      value = d;
      break;
    }
    default:
      wchar_t buf[512];
      swprintf(
          buf,
          sizeof(buf),
          L"Unrecognized argument value type: %d\n",
          propType);
      throw hresult_invalid_argument(buf);
  }

  return value;
}

IInspectable ConvertToIInspectable(folly::dynamic const &object) {
  if (object == nullptr)
    return nullptr;

  switch (object.type()) {
    case folly::dynamic::NULLT:
      return nullptr;
    case folly::dynamic::ARRAY: {
      auto objs = single_threaded_vector<IInspectable>();
      for (size_t i = 0; i < object.size(); i++) {
        objs.Append(ConvertToIInspectable(object.at(i)));
      }
      return objs.GetView();
    }
    case folly::dynamic::BOOL:
      return box_value(object.asBool());
    case folly::dynamic::DOUBLE:
      return box_value(object.asDouble());
    case folly::dynamic::INT64:
      return box_value(object.asInt());
    case folly::dynamic::OBJECT: {
      auto objs = single_threaded_map<IInspectable, IInspectable>();
      for (auto it : object.items()) {
        objs.Insert(
            ConvertToIInspectable(it.first), ConvertToIInspectable(it.second));
      }
      return objs.GetView();
    }
    case folly::dynamic::STRING:
      return box_value(to_hstring(object.asString()));
    default:
      throw hresult_invalid_argument(L"Unrecognized argument value type.");
  }
}

} // namespace winrt::Microsoft::ReactNative::Bridge
