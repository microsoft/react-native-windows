// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <AsyncStorage/StorageFileIO.h>
#include <Chakra/Utf8DebugExtensions.h>

#ifdef WINRT
#include <winrt/Windows.Storage.h>
#else
#include <ShlObj.h>
#include <Shlwapi.h>
#endif

#include <fcntl.h>
#include <io.h>

#ifdef LIBLET_BUILD
#include <oacr.h>
#endif

namespace facebook {
namespace react {
StorageFileIO::StorageFileIO(const WCHAR *storageFileName) {
  if (storageFileName == nullptr || storageFileName[0] == 0)
    throw std::exception("Storage File name is empty.");

#ifdef WINRT
  const std::wstring localFolder =
      std::wstring(winrt::Windows::Storage::ApplicationData::Current().LocalFolder().Path());
  const std::wstring strStorageFolderFullPath = localFolder + L"\\react-native";
  const std::wstring strStorageFileFullPath = strStorageFolderFullPath + L"\\" + storageFileName + L".txt";
#else
  WCHAR wzMyAppDataDirPathArr[MAX_PATH];
  HRESULT hr = SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA, nullptr, SHGFP_TYPE_CURRENT, wzMyAppDataDirPathArr);
  if (hr != S_OK)
    throwLastErrorMessage();

  const std::wstring strLocalAppDataPath{wzMyAppDataDirPathArr};
  const std::wstring strMicrosoftFullPath = strLocalAppDataPath + L"\\Microsoft";
  const std::wstring strOfficeFullPath = strMicrosoftFullPath + L"\\Office";
  const std::wstring strStorageFolderFullPath = strOfficeFullPath + L"\\SDXStorage";
  const std::wstring strStorageFileExtension = L".txt";
  const std::wstring strStorageFileFullPath =
      strStorageFolderFullPath + L"\\" + storageFileName + strStorageFileExtension;
  // full path should be like -
  // C:\Users\<username>\AppData\Local\Microsoft\Office\SDXStorage\ReactNativeAsyncStorage.txt

  if (!CreateDirectoryW(strMicrosoftFullPath.c_str(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS)
    throwLastErrorMessage();
  if (!CreateDirectoryW(strOfficeFullPath.c_str(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS)
    throwLastErrorMessage();
#endif
  if (!CreateDirectoryW(strStorageFolderFullPath.c_str(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS)
    throwLastErrorMessage();

    // The FILE_FLAG_WRITE_THROUGH can be specified to ensure any writes are
    // written to the disk right away but it causes IO to be much slower (~10x).
#ifdef WINRT
  CREATEFILE2_EXTENDED_PARAMETERS extendedParams = {};
  extendedParams.dwSize = sizeof(CREATEFILE2_EXTENDED_PARAMETERS);
  extendedParams.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
  m_storageFileHandle = CreateFile2(
      strStorageFileFullPath.c_str(),
      GENERIC_READ | GENERIC_WRITE,
      FILE_SHARE_READ | FILE_SHARE_WRITE,
      OPEN_ALWAYS,
      &extendedParams);
#else
  m_storageFileHandle = CreateFileW(
      strStorageFileFullPath.c_str(),
      GENERIC_READ | GENERIC_WRITE,
      FILE_SHARE_READ | FILE_SHARE_WRITE,
      nullptr,
      OPEN_ALWAYS,
      FILE_ATTRIBUTE_NORMAL,
      nullptr);
#endif
  if (m_storageFileHandle == INVALID_HANDLE_VALUE)
    throwLastErrorMessage();

  int fdFileDescriptor = _open_osfhandle((intptr_t)m_storageFileHandle, _O_RDWR);
  if (fdFileDescriptor == -1)
    throwLastErrorMessage();

  m_storageFile =
      std::unique_ptr<FILE, std::function<void(FILE *)>>(_fdopen(fdFileDescriptor, "r+"), [](FILE *f) { fclose(f); });
  if (m_storageFile == nullptr)
    throwLastErrorMessage();
}

StorageFileIO::~StorageFileIO() {}

bool StorageFileIO::getLine(std::string &line) {
  line = "";
  auto startingIdx = m_fileBufferIdx;
  char c = '\0';
  do {
    if (!m_fileBufferInited || (m_fileBufferIdx == IOHelperBufferSize)) {
      line.append(&m_fileBuffer[startingIdx], m_fileBufferIdx - startingIdx);
      m_fileBufferSize = fread(m_fileBuffer, sizeof(char), IOHelperBufferSize, m_storageFile.get());
      m_fileBufferInited = true;
      m_fileBufferIdx = 0;
      startingIdx = 0;
    }

    if (m_fileBufferIdx == m_fileBufferSize)
      return false;

    c = m_fileBuffer[m_fileBufferIdx];
    m_fileBufferIdx++;
  } while (c != '\n');

  // Note that the -1 is to exclude the \n character.
  line.append(&m_fileBuffer[startingIdx], m_fileBufferIdx - startingIdx - 1);
  return true;
}

void StorageFileIO::resetLine() {
  if (fseek(m_storageFile.get(), 0, SEEK_SET))
    throwLastErrorMessage();

  m_fileBufferInited = false;
  memset(m_fileBuffer, 0, IOHelperBufferSize * sizeof(char));
  m_fileBufferIdx = 0;
  m_fileBufferSize = IOHelperBufferSize;
}

void StorageFileIO::clear() {
  resetLine();

  bool success = SetEndOfFile(m_storageFileHandle);
  if (!success)
    throwLastErrorMessage();
}

// Append assumes the file seek pointer is at the end of the file.
void StorageFileIO::append(const std::string &fileContent) {
  fwrite(fileContent.c_str(), sizeof(char), fileContent.size(), m_storageFile.get());
}

void StorageFileIO::flush() {
  fflush(m_storageFile.get());
}

void StorageFileIO::throwLastErrorMessage() {
  char errorMessageBuffer[IOHelperBufferSize + 1] = {0};
  FormatMessageA(
      FORMAT_MESSAGE_FROM_SYSTEM,
      nullptr,
      GetLastError(),
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      errorMessageBuffer,
      IOHelperBufferSize,
      nullptr);
  throw std::exception(errorMessageBuffer);
}
} // namespace react
} // namespace facebook
