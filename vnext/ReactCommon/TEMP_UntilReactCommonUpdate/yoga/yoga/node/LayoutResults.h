/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <array>

#include <yoga/bits/NumericBitfield.h>
#include <yoga/enums/Direction.h>
#include <yoga/node/CachedMeasurement.h>
#include <yoga/numeric/FloatOptional.h>

namespace facebook::yoga {

struct LayoutResults {
  // This value was chosen based on empirical data:
  // 98% of analyzed layouts require less than 8 entries.
  static constexpr int32_t MaxCachedMeasurements = 8;

  std::array<float, 4> position = {};
  std::array<float, 2> dimensions = {{YGUndefined, YGUndefined}};
  std::array<float, 4> margin = {};
  std::array<float, 4> border = {};
  std::array<float, 4> padding = {};

 private:
  Direction direction_ = Direction::Inherit;
  bool hadOverflow_ = false;

 public:
  uint32_t computedFlexBasisGeneration = 0;
  FloatOptional computedFlexBasis = {};

  // Instead of recomputing the entire layout every single time, we cache some
  // information to break early when nothing changed
  uint32_t generationCount = 0;
  Direction lastOwnerDirection = Direction::Inherit;

  uint32_t nextCachedMeasurementsIndex = 0;
  std::array<CachedMeasurement, MaxCachedMeasurements> cachedMeasurements = {};
  std::array<float, 2> measuredDimensions = {{YGUndefined, YGUndefined}};

  CachedMeasurement cachedLayout{};

  Direction direction() const {
    return direction_;
  }

  void setDirection(Direction direction) {
    direction_ = direction;
  }

  bool hadOverflow() const {
    return hadOverflow_;
  }
  void setHadOverflow(bool hadOverflow) {
    hadOverflow_ = hadOverflow;
  }

  bool operator==(LayoutResults layout) const;
  bool operator!=(LayoutResults layout) const {
    return !(*this == layout);
  }
};

} // namespace facebook::yoga
