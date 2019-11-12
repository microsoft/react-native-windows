// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "PreparedJavaScriptStore.h"

#include <cstddef>
#include <memory>
#include <optional>
#include <utility>

namespace facebook::react {
class MemoryTracker;
}

namespace Microsoft::JSI {

struct ChakraRuntimeArgs {
 public:
  /**
   * @enum ExecutablePageAllocationPolicy Indicates whether a ChakraRuntime
   * is allowed to allocate executatble pages.
   */
  enum class ExecutablePageAllocationPolicy {
    /**
     * @var ChakraRuntime will not allocate any executable code pages. This
     * also implies that Native Code generation will be turned off. Note that
     * this will break JavaScript stack decoding in tools like WPA, since
     * they rely on allocation of unique thunks to interpret each function and
     * allocation of those thunks will be disabled as well
     */
    DisableAll,
    /**
     * @var ChakraRuntime will not generate native code.
     */
    DisableNativeCodeGeneration,
    /**
     * @var ChakraRuntime is free to generate executable pages.
     */
    EnableAll
  };

  /**
   * @brief ChakraRuntime's policy for allocating executable pages.
   */
  ExecutablePageAllocationPolicy m_executablePageAllocationPolicy{ExecutablePageAllocationPolicy::EnableAll};

  /**
   * TODO (yicyao)
   */
  bool m_enableDebugging{false};

  /**
   * @brief The maximum amount of memory the JavaScript engine may allocate,
   * in bytes. Its value must be greater than zero.
   */
  std::optional<size_t> m_memoryLimit{};

  /**
   * @brief The memory tracker that monitors ChakraRuntime's memory consumption.
   * This argument is optional and can be nullptr.
   */
  std::shared_ptr<facebook::react::MemoryTracker> m_memoryTracker{nullptr};

  /**
   * TODO (yicyao)
   */
  std::unique_ptr<PreparedJavaScriptStore> m_preparedJsStore{nullptr};
};

} // namespace Microsoft::JSI
