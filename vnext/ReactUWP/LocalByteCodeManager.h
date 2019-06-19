// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <cxxreact/JSBigString.h>
#include <string>
#include <future>
#include <winrt/Windows.Storage.h>

using namespace winrt;

namespace react { namespace uwp {

class LocalByteCodeManager
{
public:
  // Get byte code file created date to compare with app bundle file created date
  static std::future<winrt::Windows::Foundation::DateTime> LoadCreatedDateTimeAsync();

  // Create byte code file to re-use when app bundle did not change
  static std::future<void> CreateFileAsync(size_t size, const uint8_t* buffer);

  // Get buffer from byte code file
  static std::future<Windows::Storage::Streams::IBuffer> LoadBufferAsync();
};

}}
