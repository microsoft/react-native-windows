// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "ShadowNode.h"

namespace facebook { namespace react {

ShadowNode::~ShadowNode() {}

void ShadowNode::dispatchCommand(int64_t commandId, const folly::dynamic& commandArgs) {}

void ShadowNode::onDropViewInstance() {}

void ShadowNode::updateProperties(const folly::dynamic&& props) {}

}} // namespace facebook::react
