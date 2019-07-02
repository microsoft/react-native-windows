// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <WebSocketJSExecutorFactory.h>
#include "WebSocketJSExecutor.h"

using Microsoft::React::WebSocketJSExecutor;

namespace facebook {
namespace react {

#pragma region WebSocketJSExecutorFactory members

  WebSocketJSExecutorFactory::WebSocketJSExecutorFactory(JSECreator&& jsExecutorFactory)
  : m_jseCreator(std::move(jsExecutorFactory))
{
}

std::unique_ptr<JSExecutor> WebSocketJSExecutorFactory::createJSExecutor(
  std::shared_ptr<ExecutorDelegate> delegate,
  std::shared_ptr<MessageQueueThread> jsQueue)
{
  if (m_jseCreator)
    return m_jseCreator(delegate, jsQueue);
  else
    return std::unique_ptr<JSExecutor>(new WebSocketJSExecutor(delegate, jsQueue));
}

#pragma endregion WebSocketJSExecutorFactory members

}} // namespace facebook::react
