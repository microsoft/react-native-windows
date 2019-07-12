// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
// TODO: VerifyElseCrash instead of nyi()?

#pragma once

#include <glog/logging.h>
#include <sstream>

static inline void nyi() {
  int *a = nullptr;
  *a = 0;
}

namespace double_conversion {

class StringBuilder {
 public:
  StringBuilder(char *buffer, size_t) : m_buffer(buffer) {}

  size_t position() {
    return m_ss.str().length();
  }

  void Finalize() {
    std::string str = m_ss.str();
    memcpy(m_buffer, str.c_str(), str.length());
  }

  char *m_buffer;
  std::stringstream m_ss;
};

class DoubleToStringConverter {
 public:
  enum DtoaMode {
    // Produce the shortest correct representation.
    // For example the output of 0.299999999999999988897 is (the less accurate
    // but correct) 0.3.
    SHORTEST,
    // Same as SHORTEST, but for single-precision floats.
    SHORTEST_SINGLE,
    // Produce a fixed number of digits after the decimal point.
    // For instance fixed(0.1, 4) becomes 0.1000
    // If the input number is big, the output will be big.
    FIXED,
    // Fixed number of digits (independent of the decimal point).
    PRECISION
  };

  // When calling ToFixed with a double > 10^kMaxFixedDigitsBeforePoint
  // or a requested_digits parameter > kMaxFixedDigitsAfterPoint then the
  // function returns false.
  static const int kMaxFixedDigitsBeforePoint = 60;
  static const int kMaxFixedDigitsAfterPoint = 60;

  // When calling ToExponential with a requested_digits
  // parameter > kMaxExponentialDigits then the function returns false.
  static const int kMaxExponentialDigits = 120;

  // When calling ToPrecision with a requested_digits
  // parameter < kMinPrecisionDigits or requested_digits > kMaxPrecisionDigits
  // then the function returns false.
  static const int kMinPrecisionDigits = 1;
  static const int kMaxPrecisionDigits = 120;

  static const int kBase10MaximalLength = 17;

  enum Flags {
    NO_FLAGS = 0,
    EMIT_POSITIVE_EXPONENT_SIGN = 1,
    EMIT_TRAILING_DECIMAL_POINT = 2,
    EMIT_TRAILING_ZERO_AFTER_POINT = 4,
    UNIQUE_ZERO = 8
  };

  DoubleToStringConverter(
      int,
      const char *,
      const char *,
      char,
      int,
      int,
      int,
      int) {
    // nyi();
  }

  bool ToShortest(double d, StringBuilder *s) {
    s->m_ss << d;
    if (s->m_ss.fail()) {
      s->m_ss.clear();
      return false;
    }
    return true;
  }

  void ToShortestSingle(float value, StringBuilder *s) {
    s->m_ss << value;
  }

  bool ToFixed(double d, unsigned int, StringBuilder *s) {
    s->m_ss << d;
    if (s->m_ss.fail()) {
      s->m_ss.clear();
      return false;
    }
    return true;
  }

  void ToPrecision(double d, unsigned int, StringBuilder *s) {
    s->m_ss << d;
  }

  bool ToExponential(double value, int requested_digits, StringBuilder *s)
      const {
    s->m_ss << value;
    if (s->m_ss.fail()) {
      s->m_ss.clear();
      return false;
    }
    return true;
  }
};

class StringToDoubleConverter {
 public:
  enum AtodMode {
    ALLOW_TRAILING_JUNK,
    ALLOW_LEADING_SPACES,
  };

  StringToDoubleConverter(int, double, double, void *, void *) {
    // nyi();
  }

  double StringToDouble(const char *s, int l, int *consumed) {
    size_t idx = 0;
    std::string str(s, l);
    double d = std::stod(str.c_str(), &idx);
    *consumed = static_cast<int>(idx);
    return d;
  }
};

} // namespace double_conversion