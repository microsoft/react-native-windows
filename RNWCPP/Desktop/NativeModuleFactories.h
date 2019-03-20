// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <memory>
#include <cxxreact/CxxModule.h>
#include <cxxreact/MessageQueueThread.h>

namespace facebook { namespace react {

extern std::unique_ptr<facebook::xplat::module::CxxModule> CreateAsyncStorageModule(const WCHAR* storageFileName) noexcept;

extern std::unique_ptr<facebook::xplat::module::CxxModule> CreateTimingModule(
	const std::shared_ptr<facebook::react::MessageQueueThread>& nativeThread) noexcept;

extern std::unique_ptr<facebook::xplat::module::CxxModule> CreateWebSocketModule() noexcept;

}}
