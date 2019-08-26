// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "SampleModule.h"
#include "SampleModule.g.cpp"
#include <winrt\Windows.Data.Json.h>

using namespace winrt;
using namespace Microsoft::ReactNative;
using namespace Microsoft::ReactNative::Bridge;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

namespace winrt::SampleApp::implementation {

void SampleModule::Initialize() {
  // TODO: Add support on ReactContext to register as an
  // IBackgroundEventListener and/or ILifeCycleEventListener
  OutputDebugStringW(L"Initializing\n");
}

hstring SampleModule::Name() {
  return L"SampleModule";
}

IMapView<hstring, IInspectable> SampleModule::Constants() {
  auto constants = single_threaded_map<hstring, IInspectable>();
  constants.Insert(L"a", box_value(L"\"b\""));
  return constants.GetView();
}

IVectorView<MethodInfo> SampleModule::Methods() {
  auto delegates = single_threaded_vector<MethodInfo>(
      {{L"method1",
        ReturnType::Void,
        [](IVectorView<IInspectable> const & /*ignored*/,
           Callback const & /*ignored*/,
           Callback const & /*ignored*/) {
          // no op
          OutputDebugStringW(L"SampleModule.method1()\n");
        }},
       {L"method2",
        ReturnType::Callback,
        [](IVectorView<IInspectable> const & /*ignored*/,
           Callback const &callback,
           Callback const & /*ignored*/) {
          OutputDebugStringW(L"SampleModule.method2()\n");

          // Build up a complex object to pass back as arguments when invoking
          // the callback.  They'll automatically be converted to an
          // equivalent type in JavaScript when the JS function is called.
          auto integerArray = PropertyValue::CreateInt32Array(
              std::array<int32_t, 5>{0, 1, 2, 3, 4});

          auto floatArray = PropertyValue::CreateSingleArray(
              std::array<float_t, 3>{0.02f, 1.0f, 0.42f});

          auto stringArray = PropertyValue::CreateStringArray({L"a", L"bb"});

          auto mixedArray =
              PropertyValue::CreateInspectableArray(std::array<IInspectable, 2>{
                  box_value(L"First"), box_value(0x48)});

          auto nestedArray = PropertyValue::CreateInspectableArray(
              std::array<IInspectable, 2>{mixedArray, box_value(L"Last")});

          auto callbackArgs =
              single_threaded_vector<IInspectable>({box_value(84),
                                                    integerArray,
                                                    floatArray,
                                                    stringArray,
                                                    nestedArray});

          callback(callbackArgs.GetView());
        }},
       {L"method3", ReturnType::Callback, Method(this, &SampleModule::method3)},
       {L"method4",
        ReturnType::Promise,
        [](IVectorView<IInspectable> const & /*ignored*/,
           Callback const &resolve,
           Callback const &reject /*Promise const &promise*/) {
          //
          try {
            resolve({box_value(true)});
          } catch (std::exception e) {
            hstring what = to_hstring(e.what());
            reject({box_value(L"0x80000000"),
                    box_value(what),
                    box_value(L"stack trace goes here")});
          }
        }}});

  return delegates.GetView();
}

void SampleModule::method3(
    IVectorView<IInspectable> const &args,
    Callback const &callback,
    Callback const & /*ignored*/) {
  if (args.Size() < 1)
    throw hresult_invalid_argument();

  // At the moment the input args are packed into a single JSON-formatted
  // string.
  auto input = args.GetAt(0).as<IPropertyValue>();
  assert(input.Type() == PropertyType::String);
  auto jsonIn = input.GetString();
  OutputDebugStringW(L"SampleModule.method3(");
  OutputDebugStringW(jsonIn.c_str());
  OutputDebugStringW(L")\n");

  // Invoke the callback with some arguments. Using JSON isn't required.  We
  // could have used other basic types instead (e.g. int, double, float, bool,
  // etc.)
  auto text = L"{ \"result\" : true }";
  auto jsonOut = Windows::Data::Json::JsonObject::Parse(text);
  auto callbackArgs = single_threaded_vector<IInspectable>({jsonOut}).GetView();

  callback(callbackArgs);
}

} // namespace winrt::SampleApp::implementation
