// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "LocalByteCodeManager.h"
#include "unicode.h"
#include <winrt/Windows.Storage.h>

#if _MSC_VER <= 1913
// VC 19 (2015-2017.6) cannot optimize co_await/cppwinrt usage
#pragma optimize( "", off )
#endif

using namespace winrt;

namespace react {
  namespace uwp {
    std::future<winrt::Windows::Foundation::DateTime> LocalByteCodeManager::LoadCreatedDateTimeAsync()
    {
      auto folder = Windows::Storage::ApplicationData::Current().LocalCacheFolder();
      co_await winrt::resume_background();

      try
      {
        auto file = co_await folder.GetFileAsync(L"app.bytecode");
        return file.DateCreated();
      }
      catch (winrt::hresult_error const& ex)
      {
        winrt::Windows::Foundation::DateTime date;
        return date;
      }
    }

    winrt::Windows::Foundation::DateTime LocalByteCodeManager::LoadCreatedDateTime()
    {
      return LoadCreatedDateTimeAsync().get();
    }

    std::future<void> LocalByteCodeManager::CreateFileAsync(const uint8_t* buffer)
    {
      auto folder = Windows::Storage::ApplicationData::Current().LocalCacheFolder();
      auto file = co_await folder.CreateFileAsync(L"app.bytecode", Windows::Storage::CreationCollisionOption::ReplaceExisting);
      co_await winrt::Windows::Storage::FileIO::WriteBytesAsync(file, winrt::array_view<const uint8_t> { *buffer });
    }

    void LocalByteCodeManager::CreateFile(const uint8_t* buffer)
    {
      CreateFileAsync(buffer).get();
    }

    std::future<Windows::Storage::Streams::IBuffer> LocalByteCodeManager::LoadBufferAsync()
    {
      auto folder = Windows::Storage::ApplicationData::Current().LocalCacheFolder();
      auto file = co_await folder.GetFileAsync(L"app.bytecode");
      auto buffer = co_await Windows::Storage::FileIO::ReadBufferAsync(file);
      return buffer;
    }

    Windows::Storage::Streams::IBuffer LocalByteCodeManager::LoadBuffer()
    {
      return LoadBufferAsync().get();
    }
  }
}
