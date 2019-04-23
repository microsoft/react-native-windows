// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
// TODO: VerifyElseCrash instead of nyi()?

#pragma once

#include <sstream>

static inline void nyi() { int *a = nullptr; *a = 0; }

namespace double_conversion {

class StringBuilder {
public:
  StringBuilder(char* buffer, size_t)
    : m_buffer(buffer)
  {
  }

  size_t position() {
    return m_ss.str().length();
  }

  void Finalize() {
    std::string str = m_ss.str();
    memcpy(m_buffer, str.c_str(), str.length());
  }

  char* m_buffer;
  std::stringstream m_ss;
};

class DoubleToStringConverter {
public:
  enum DtoaMode {
    NO_FLAGS,
    SHORTEST,
    SHORTEST_SINGLE,
    FIXED,
    PRECISION,
  };

  static const int kBase10MaximalLength = 17;

  DoubleToStringConverter(DtoaMode, const char*, const char*, char, int, int, int, int) {
    // nyi();
  }

  void ToShortest(double d, StringBuilder* s) {
    s->m_ss << d;
  }

  void ToFixed(double d, unsigned int, StringBuilder* s) {
    s->m_ss << d;
  }

  void ToPrecision(double d, unsigned int, StringBuilder* s) {
    s->m_ss << d;
  }
};

class StringToDoubleConverter {
public:
  enum AtodMode {
    ALLOW_TRAILING_JUNK,
    ALLOW_LEADING_SPACES,
  };

  StringToDoubleConverter(int, double, double, void*, void*) {
    // nyi();
  }

  double StringToDouble(const char* s, int l, int* consumed) {
    size_t idx = 0;
    std::string str(s, l);
    double d = std::stod(str.c_str(), &idx);
    *consumed = static_cast<int>(idx);
    return d;
  }
};

} // namespace double_conversion
