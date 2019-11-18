// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <functional>
#include <memory>
#include <string>

#include <cxxreact/ReactMarker.h>

namespace facebook {
namespace react {

namespace Logging {
enum LoggingLevel { VERBOSE = 200, INFO = 50, WARNING = 15, ERROR = 10 };

LoggingLevel forValue(int level);

LoggingLevel getLevel();

void setLevel(LoggingLevel loggingLevel);
} // namespace Logging

} // namespace react
} // namespace facebook