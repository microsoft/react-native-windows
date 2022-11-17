// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

static constexpr std::string_view ExtrapolateTypeIdentity = "identity";
static constexpr std::string_view ExtrapolateTypeClamp = "clamp";
static constexpr std::string_view ExtrapolateTypeExtend = "extend";

static double Interpolate(
    double value,
    double inputMin,
    double inputMax,
    double outputMin,
    double outputMax,
    std::string_view const &extrapolateLeft,
    std::string_view const &extrapolateRight) {
  auto result = value;

  // Extrapolate
  if (result < inputMin) {
    if (extrapolateLeft == ExtrapolateTypeIdentity) {
      return result;
    } else if (extrapolateLeft == ExtrapolateTypeClamp) {
      result = inputMin;
    }
  }

  if (result > inputMax) {
    if (extrapolateRight == ExtrapolateTypeIdentity) {
      return result;
    } else if (extrapolateRight == ExtrapolateTypeClamp) {
      result = inputMax;
    }
  }

  if (inputMin == inputMax) {
    if (value <= inputMin) {
      return outputMin;
    }
    return outputMax;
  }

  return outputMin + (outputMax - outputMin) * (result - inputMin) / (inputMax - inputMin);
}
