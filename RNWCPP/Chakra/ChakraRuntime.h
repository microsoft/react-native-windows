// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <jsi/jsi.h>

namespace facebook {
namespace react {
namespace chakra {

using Logger = std::function<void(const std::string& message, unsigned int logLevel)>;

std::unique_ptr<jsi::Runtime> makeChakraRuntime(ChakraInstanceArgs&& args, std::shared_ptr<MessageQueueThread> jsQueue, Logger logger);

} // namespace chakra
} // namespace react
} // namespace facebook
