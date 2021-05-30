#pragma once
#include <windows.h>
#include <ostream>
#include <sstream>
#include <string>

struct basic_dostream : public std::streambuf {};

extern basic_dostream cdebug;

inline basic_dostream &operator<<(basic_dostream &o, const char *str) {
  OutputDebugStringA(str);
  return o;
}
inline basic_dostream &operator<<(basic_dostream &o, const wchar_t *str) {
  OutputDebugStringW(str);
  return o;
}
template <typename T>
basic_dostream &operator<<(basic_dostream &o, const T &t) {
  auto str = std::to_string(t);
  return o << str;
}

inline basic_dostream &operator<<(basic_dostream &o, const std::string_view &sv) {
  return o << sv.data();
}

inline basic_dostream &operator<<(basic_dostream &o, const std::string &sv) {
  return o << sv.data();
}

inline basic_dostream &operator<<(basic_dostream &o, const std::wstring_view &sv) {
  return o << sv.data();
}

inline basic_dostream &operator<<(basic_dostream &o, const std::wstring &sv) {
  return o << sv.data();
}

namespace std {
inline string to_string(int32_t n, int base) {
  char buf[32]{};
  _itoa_s(n, buf, base);
  return buf;
}
} // namespace std

#define DEBUG_HRESULT_ERROR(e)                    \
  cdebug << __FILE__ << " (" << __LINE__ << ") "; \
  cdebug << e.message().c_str() << "\n";
