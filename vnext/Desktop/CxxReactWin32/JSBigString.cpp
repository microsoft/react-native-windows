// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <cxxreact/JSBigString.h>

namespace facebook {
namespace react {

// Win 32 code should not use JsBigFileString anywhere, so all member
// functions of JsBigFileString crashes for now.
//
// TODO (VSTS 2129144) We should merge JsBigFileString with
// JsBigFileStringWithExternalBuffer

JSBigFileString::JSBigFileString(int fd, size_t size, off_t offset /*= 0*/) {
  std::terminate();
}

JSBigFileString::~JSBigFileString() {
  std::terminate();
}

const char *JSBigFileString::c_str() const {
  std::terminate();
}

size_t JSBigFileString::size() const {
  std::terminate();
}

int JSBigFileString::fd() const {
  std::terminate();
}

std::unique_ptr<const JSBigFileString> JSBigFileString::fromPath(
    const std::string &sourceURL) {
  std::terminate();
}

} // namespace react
} // namespace facebook
