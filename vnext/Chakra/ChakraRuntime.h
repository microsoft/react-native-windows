// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <jsi/jsi.h>

namespace facebook {
namespace react {
namespace chakra {

#ifndef JSI_CORE

using Logger = std::function<void(const std::string& message, unsigned int logLevel)>;

std::unique_ptr<jsi::Runtime> makeChakraRuntime(ChakraInstanceArgs&& args, std::shared_ptr<MessageQueueThread> jsQueue, Logger logger);

#else // JSI_CORE

struct IPromiseContinuation
{
	virtual void registerPromise(facebook::jsi::Function&& func) = 0;
};

std::unique_ptr<jsi::Runtime> makeChakraRuntime(IPromiseContinuation&);
std::unique_ptr<jsi::Runtime> makeChakraRuntime();

#endif // JSI_CORE

} // namespace chakra
} // namespace react
} // namespace facebook
