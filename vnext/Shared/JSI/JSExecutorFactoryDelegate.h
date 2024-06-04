// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <functional>
#include <memory>

namespace facebook::react {
class CallInvoker;
class JSExecutorFactory;
using JSExecutorFactoryDelegate =
    std::function<std::shared_ptr<JSExecutorFactory>(std::shared_ptr<CallInvoker> const &callInvoker)>;
} // namespace facebook::react
