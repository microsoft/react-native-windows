// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

// For now, uwp uses a suboptimal implementation that simply reads file into
// memory.
// TODO: Actually memory map the file.

#include "pch.h"

#include <cxxreact/JSBigString.h>
#include <fstream>

namespace facebook {
namespace react {

JSBigFileString::JSBigFileString(int fd, size_t size, off_t /*offset*/ /*= 0*/)
    : m_fd(fd), m_size(size), m_pageOff(0), m_mapOff(0), m_data(new char[size + 1]) {
  *(const_cast<char *>(&m_data[m_size])) = '\0';
}

JSBigFileString::~JSBigFileString() {
  delete[] m_data;
}

const char *JSBigFileString::c_str() const {
  return m_data;
}

std::unique_ptr<const JSBigFileString> JSBigFileString::fromPath(const std::string &sourceURL) {
  std::unique_ptr<JSBigFileString> buffer;
  {
    std::ifstream file(sourceURL);
    if (file) {
      file.seekg(0, std::ios::end);
      auto fileSize = file.tellg();
      if (0 <= fileSize) {
        buffer = std::make_unique<JSBigFileString>(-1 /*fd*/, static_cast<size_t>(fileSize));
        file.seekg(0, std::ios::beg);
        file.read(const_cast<char *>(buffer->m_data), buffer->m_size);
      }
    }
  }

  return buffer;
}

size_t JSBigFileString::size() const {
  return m_size;
}

} // namespace react
} // namespace facebook
