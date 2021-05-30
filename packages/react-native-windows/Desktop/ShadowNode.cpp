// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "ShadowNode.h"

namespace facebook {
namespace react {

ShadowNode::~ShadowNode() {}

void ShadowNode::dispatchCommand(const std::string &commandId, const folly::dynamic &commandArgs) {}

void ShadowNode::onDropViewInstance() {}

void ShadowNode::updateProperties(const folly::dynamic &&props) {}

} // namespace react
} // namespace facebook
