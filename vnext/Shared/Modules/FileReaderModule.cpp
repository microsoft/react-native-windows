// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "FileReaderModule.h"

using namespace facebook::xplat;

using folly::dynamic;
using std::string;

namespace {
constexpr char moduleName[] = "FileReaderModule";
} //namespace <anonymous>

namespace Microsoft::React {

#pragma region FileReaderModule

FileReaderModule::FileReaderModule() noexcept
{
}

FileReaderModule::~FileReaderModule() noexcept /*override*/
{
}

#pragma region CxxModule

string FileReaderModule::getName()
{
  return moduleName;
}

std::map<string, dynamic> FileReaderModule::getConstants()
{
  return {};
}

std::vector<module::CxxModule::Method> FileReaderModule::getMethods()
{
  return
  {
    {
      "readAsDataURL", [](dynamic args) {}
    },
    {
      "readAsText", [](dynamic args) {}
    }
  };
}

#pragma endregion CxxModule

#pragma endregion FileReaderModule

/*extern*/ const char* GetFileReaderModuleName() noexcept
{
  return moduleName;
}

/*extern*/ std::unique_ptr<module::CxxModule> CreateFileReaderModule() noexcept {
  return std::make_unique<FileReaderModule>();
}

}//namespace
