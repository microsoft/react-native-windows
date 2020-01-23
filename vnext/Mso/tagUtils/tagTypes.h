// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_TAGUTILS_TAGTYPES_H
#define MSO_TAGUTILS_TAGTYPES_H

#if __cplusplus && !defined(MS_TARGET_POSIX)
/**
Default tag to use, will be replaced later by tagging script.
ie: AssertTag(FALSE, UNTAGGED);
*/
constexpr unsigned int UNTAGGED = static_cast<unsigned int>(0x0000);

class MsoReserveTag final {
 public:
  /**
   * Creates a tag from underlying value.
   * @param v the value of the tag.
   * Note that the expression evaluating to v should be a literal or simple variable expression;
   * more complex expressions may cause the tagging script to fail.
   * See http://aka.ms/officetagging for more details on the tagging script.
   */
  constexpr explicit MsoReserveTag(unsigned int v) noexcept : m_value(v) {}

  // This is an unfortunate work around at the moment to allow conversion back to the underlying
  // type so that existing comparisons and function signatures work
  constexpr operator unsigned int() const noexcept {
    return m_value;
  }

 private:
  const unsigned int m_value;
};

constexpr inline MsoReserveTag MsoReserve() noexcept {
  return MsoReserveTag(UNTAGGED);
}

#else

#if __OBJC__
#define UNTAGGED ('0' << 24 | '0' << 16 | '0' << 8 | '0')
#else
#define UNTAGGED '0000'
#endif

#define MsoReserve() MsoReserveTag(UNTAGGED)
#define MsoReserveTag(x) x

#endif

/**
Used for legacy 4-character tags
AssertTag(FALSE, ASSERTTAG('abcd'));
*/
#define ASSERTTAG(x) x

#endif // MSO_TAGUTILS_TAGTYPES_H
