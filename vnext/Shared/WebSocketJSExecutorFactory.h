// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/JSExecutor.h>
#include "DevServerHelper.h"
#include "IDevSupportManager.h"

namespace facebook {
namespace react {

class WebSocketJSExecutorFactory : public JSExecutorFactory {
 public:
  WebSocketJSExecutorFactory() = default;
  WebSocketJSExecutorFactory(JSECreator &&jsExecutorFactory);

  virtual std::unique_ptr<JSExecutor> createJSExecutor(
      std::shared_ptr<ExecutorDelegate> delegate,
      std::shared_ptr<MessageQueueThread> jsQueue) override;

 private:
  JSECreator m_jseCreator;
};

} // namespace react
} // namespace facebook
