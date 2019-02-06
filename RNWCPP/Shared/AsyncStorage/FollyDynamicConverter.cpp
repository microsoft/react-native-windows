// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <AsyncStorage/FollyDynamicConverter.h>

using namespace std;
using namespace folly;
using namespace facebook::xplat;

namespace facebook {
  namespace react {
    std::vector<string> FollyDynamicConverter::jsArgAsStringVector(const dynamic& args) noexcept
    {
      // args is array[array[]]
      std::vector<string> keys;
      for (const auto& jsKey : args[0])
      {
        keys.emplace_back(jsKey.getString());
      }
      return keys;
    }

    std::vector<tuple<string, string>> FollyDynamicConverter::jsArgAsTupleStringVector(const dynamic& args) noexcept
    {
      // args is array[array[array[]]]
      std::vector<tuple<string, string>> kVVector;
      const int iKeyPos = 0;
      const int iValuePos = 1;
      for (const auto& jsKV : args[0])
      {
        kVVector.emplace_back(jsKV[iKeyPos].getString(), jsKV[iValuePos].getString());
      }
      return kVVector;
    }

    folly::dynamic FollyDynamicConverter::stringVectorAsRetVal(const std::vector<string>& vec) noexcept
    {
      folly::dynamic jsRetVals = folly::dynamic::array;
      for (const auto& retVal : vec) {
        jsRetVals.push_back(retVal);
      }
      return jsRetVals;
    }

    folly::dynamic FollyDynamicConverter::tupleStringVectorAsRetVal(const std::vector<tuple<string, string>>& vec) noexcept
    {
      folly::dynamic jsRetVals = folly::dynamic::array;
      for (const auto& retVal : vec) {
        jsRetVals.push_back(dynamic::array(std::get<0>(retVal), std::get<1>(retVal)));
      }
      return jsRetVals;
    }
  }
}
