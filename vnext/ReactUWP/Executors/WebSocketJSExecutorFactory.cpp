// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "WebSocketJSExecutorUwp.h"

namespace facebook { namespace react {

WebSocketJSExecutorFactory::WebSocketJSExecutorFactory(facebook::react::JSECreator&& jseCreator) :
  m_jseCreator(std::move(jseCreator))
{}

std::unique_ptr<facebook::react::JSExecutor> WebSocketJSExecutorFactory::createJSExecutor(
  std::shared_ptr<facebook::react::ExecutorDelegate> delegate, std::shared_ptr<facebook::react::MessageQueueThread> jsQueue) {
  if (m_jseCreator)
    return m_jseCreator(delegate, jsQueue);
  else
    return std::unique_ptr<facebook::react::JSExecutor>(new ::react::uwp::WebSocketJSExecutor(delegate, jsQueue));
}

} }
