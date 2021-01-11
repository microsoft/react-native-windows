// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "Views/PaperShadowNode.h"

namespace Microsoft::ReactNative {

ShadowNode::~ShadowNode() {}

void ShadowNode::dispatchCommand(
    const std::string &commandId,
    winrt::Microsoft::ReactNative::JSValueArray &&commandArgs) {}

void ShadowNode::onDropViewInstance() {}

void ShadowNode::updateProperties(winrt::Microsoft::ReactNative::JSValueObject &props) {}

} // namespace Microsoft::ReactNative
