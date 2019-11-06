// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "DebugHelpers.h"

namespace SampleLibraryCPP {

void DebugWriteLine(const std::string &name, const std::string &methodName, const std::string &arg) {
  std::string output = name;
  output.append("::");
  output.append(methodName);
  output.append("(");
  output.append(arg);
  output.append(")\n");
  OutputDebugStringA(output.c_str());
}

void DebugWriteLine(const std::string &name, const std::string &methodName, double arg) {
  DebugWriteLine(name, methodName, std::to_string(arg));
}

void DebugWriteLine(const std::string &name, const std::string &methodName) {
  DebugWriteLine(name, methodName, "");
}

} // namespace SampleLibraryCPP
