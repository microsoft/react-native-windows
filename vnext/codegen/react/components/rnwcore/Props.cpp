
/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <react/components/rnwcore/Props.h>
#include <react/core/propsConversions.h>

namespace facebook {
namespace react {

ActivityIndicatorViewProps::ActivityIndicatorViewProps(
    const ActivityIndicatorViewProps &sourceProps,
    const RawProps &rawProps): ViewProps(sourceProps, rawProps),

    hidesWhenStopped(convertRawProp(rawProps, "hidesWhenStopped", sourceProps.hidesWhenStopped, {false})),
    animating(convertRawProp(rawProps, "animating", sourceProps.animating, {false})),
    color(convertRawProp(rawProps, "color", sourceProps.color, {})),
    size(convertRawProp(rawProps, "size", sourceProps.size, {ActivityIndicatorViewSize::Small}))
      {}

} // namespace react
} // namespace facebook
