// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <stdint.h>
#include <winrt/Windows.UI.Xaml.h>
#include <IReactInstance.h>
#include <folly/dynamic.h>

namespace winrt {
  using namespace Windows::UI::Xaml;
}

namespace react { namespace uwp {

  using namespace std;

  struct ReactId {
    int64_t tag{ 0 };
    bool isValid{ false };
  };

  template<typename T>
  inline typename T as(folly::dynamic const& obj)
  {
    return (T)(obj.asInt());
  }

  template<>
  inline typename std::string as<std::string>(folly::dynamic const& obj)
  {
    return obj.asString();
  }

  template<>
  inline typename bool as<bool>(folly::dynamic const& obj)
  {
    return obj.asBool();
  }

  template<>
  inline typename double as<double>(folly::dynamic const& obj)
  {
    return obj.asDouble();
  }

   ReactId getReactId(IReactInstance *instance, winrt::FrameworkElement const& fe);
}}
