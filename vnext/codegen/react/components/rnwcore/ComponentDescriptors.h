
/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <react/components/rnwcore/ShadowNodes.h>
#include <react/core/ConcreteComponentDescriptor.h>

namespace facebook {
namespace react {

using ActivityIndicatorViewComponentDescriptor = ConcreteComponentDescriptor<ActivityIndicatorViewShadowNode>;
using DatePickerComponentDescriptor = ConcreteComponentDescriptor<DatePickerShadowNode>;
using AndroidDrawerLayoutComponentDescriptor = ConcreteComponentDescriptor<AndroidDrawerLayoutShadowNode>;
using RCTMaskedViewComponentDescriptor = ConcreteComponentDescriptor<RCTMaskedViewShadowNode>;
using RCTProgressViewComponentDescriptor = ConcreteComponentDescriptor<RCTProgressViewShadowNode>;
using AndroidSwipeRefreshLayoutComponentDescriptor = ConcreteComponentDescriptor<AndroidSwipeRefreshLayoutShadowNode>;
using PullToRefreshViewComponentDescriptor = ConcreteComponentDescriptor<PullToRefreshViewShadowNode>;
using AndroidHorizontalScrollContentViewComponentDescriptor = ConcreteComponentDescriptor<AndroidHorizontalScrollContentViewShadowNode>;
using RCTSegmentedControlComponentDescriptor = ConcreteComponentDescriptor<RCTSegmentedControlShadowNode>;
using SwitchComponentDescriptor = ConcreteComponentDescriptor<SwitchShadowNode>;
using UnimplementedNativeViewComponentDescriptor = ConcreteComponentDescriptor<UnimplementedNativeViewShadowNode>;

} // namespace react
} // namespace facebook
