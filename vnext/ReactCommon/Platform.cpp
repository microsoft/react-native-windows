// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "Platform.h"

namespace facebook {
namespace react {

namespace Logging {
static LoggingLevel s_loggingLevel = INFO;

LoggingLevel getLevel() {
  return s_loggingLevel;
}

LoggingLevel forValue(int loggingLevel) {
  switch (loggingLevel) {
    case 10:
      return ERROR;
      break;
    case 15:
      return WARNING;
      break;
    case 50:
      return INFO;
      break;
    case 100:
    case 200:
      return VERBOSE;
      break;
  }
  return INFO;
}

void setLevel(LoggingLevel level) {
  s_loggingLevel = level;
}
} // namespace Logging

} // namespace react
} // namespace facebook