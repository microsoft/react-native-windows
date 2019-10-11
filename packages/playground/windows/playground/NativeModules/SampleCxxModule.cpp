// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//

#include "pch.h"
#include "SampleCxxModule.h"

const char *SampleCxxModule::Name = "Sample";

std::map<std::string, folly::dynamic> SampleCxxModule::getConstants() {
  return {
      {"one", 1},
      {"two", 2},
      {"animal", "fox"},
  };
}

std::vector<facebook::xplat::module::CxxModule::Method>
SampleCxxModule::getMethods() {
  return {
      facebook::xplat::module::CxxModule::Method(
          "GetStuff",
          [this](folly::dynamic args, Callback cb) {
            folly::dynamic d =
                folly::dynamic::object("Message", "Hello World!")(
                    "Data",
                    folly::dynamic::array(
                        folly::dynamic::object("name", "item1")("prop1", "foo")(
                            "anotherprop1", "bar"),
                        folly::dynamic::object("name", "item2")("prop2", "bar")(
                            "anotherprop2", "yyyyy"),
                        folly::dynamic::object("name", "item3")("prop3", "baz")(
                            "anotherprop1", "zzz")));

            cb({d});
          }),
  };
}
