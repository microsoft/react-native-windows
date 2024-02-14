/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// @generated by enums.py
// clang-format off
#pragma once

#include <cstdint>
#include <yoga/YGEnums.h>
#include <yoga/enums/YogaEnums.h>

namespace facebook::yoga {

enum class ExperimentalFeature : uint8_t {
  WebFlexBasis = YGExperimentalFeatureWebFlexBasis,
  AbsolutePercentageAgainstPaddingEdge = YGExperimentalFeatureAbsolutePercentageAgainstPaddingEdge,
  CallMeasureCallbackOnAllNodes = YGExperimentalFeatureCallMeasureCallbackOnAllNodes, // Used just for NetUI - Keep at end of list
};

template <>
constexpr inline int32_t ordinalCount<ExperimentalFeature>() {
  return 3; // This should be +1 from core due to CallMeasureCallbackOnAllNodes
} 

template <>
constexpr inline int32_t bitCount<ExperimentalFeature>() {
  return 2; // Depending on the number of ExperimentalFeature's this will either match core, or be core+1
} 

constexpr inline ExperimentalFeature scopedEnum(YGExperimentalFeature unscoped) {
  return static_cast<ExperimentalFeature>(unscoped);
}

constexpr inline YGExperimentalFeature unscopedEnum(ExperimentalFeature scoped) {
  return static_cast<YGExperimentalFeature>(scoped);
}

inline const char* toString(ExperimentalFeature e) {
  return YGExperimentalFeatureToString(unscopedEnum(e));
}

} // namespace facebook::yoga
