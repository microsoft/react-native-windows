#pragma once

#if (_MSVC_LANG >= 202002L)
#define CONSTEVAL consteval
#else
#define CONSTEVAL constexpr
#endif
namespace details {

#ifdef FNV_HASH_WIDTH_64
using hash_t = uint64_t;
inline constexpr size_t FNV_offset_basis = 14695981039346656037ULL;
inline constexpr size_t FNV_prime = 1099511628211ULL;
#else
using hash_t = uint32_t;
inline constexpr hash_t FNV_offset_basis = 2166136261U;
inline constexpr hash_t FNV_prime = 16777619U;
#endif

[[nodiscard]] inline CONSTEVAL hash_t Fnv1a_append_bytes(
    hash_t _Val,
    const char *_First,
    const size_t _Count) noexcept { // accumulate range [_First, _First + _Count) into partial FNV-1a hash _Val
  for (size_t _Idx = 0; _Idx < _Count; ++_Idx) {
    _Val ^= static_cast<hash_t>(_First[_Idx]);
    _Val *= FNV_prime;
  }

  return _Val;
}

_NODISCARD inline constexpr hash_t Fnv1a_append_bytes_constexpr(
    hash_t _Val,
    const char *_First,
    const size_t _Count) noexcept { // accumulate range [_First, _First + _Count) into partial FNV-1a hash _Val
  for (size_t _Idx = 0; _Idx < _Count; ++_Idx) {
    _Val ^= static_cast<hash_t>(_First[_Idx]);
    _Val *= FNV_prime;
  }

  return _Val;
}

inline hash_t hash(const char *str) noexcept {
  return Fnv1a_append_bytes_constexpr(FNV_offset_basis, str, strlen(str));
}

template <size_t N>
struct fixed_string {
  char val[N]{};
  CONSTEVAL fixed_string(const char (&v)[N + 1]) noexcept {
    for (size_t i = 0; i < N; i++) {
      val[i] = v[i];
    }
  }
  CONSTEVAL size_t length() const noexcept {
    return N;
  }

  CONSTEVAL operator const char *() const noexcept {
    return val;
  }
  CONSTEVAL const char &operator[](size_t pos) const {
    return val[pos];
  }
  constexpr bool operator==(const char *v) const noexcept {
    return strcmp(val, v) == 0;
  }

  CONSTEVAL hash_t hash() const noexcept {
    return Fnv1a_append_bytes(FNV_offset_basis, val, N);
  }
};
template <size_t N>
fixed_string(char const (&)[N]) -> fixed_string<N - 1>;

} // namespace details

#define HASH_STRING(STR) details::fixed_string{STR}.hash()
