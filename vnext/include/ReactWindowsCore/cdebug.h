#pragma once
#include <windows.h>
#include <ostream>
#include <sstream>

template <typename CharT = char>
struct debugbuffer_t : public std::basic_stringbuf<CharT, std::char_traits<CharT>> {
  virtual ~debugbuffer_t() {
    sync();
  }

  virtual int sync() override {
    _OutputDebugString(this->str().c_str());
    this->str(std::basic_string<CharT>());
    return 0;
  }

 private:
  void _OutputDebugString(PCSTR str) {
    ::OutputDebugStringA(str);
  }
  void _OutputDebugString(PCWSTR str) {
    ::OutputDebugStringW(str);
  }
};

template <class CharT>
class basic_dostream : public std::basic_ostream<CharT> {
 public:
  basic_dostream() : std::basic_ostream<CharT>(new debugbuffer_t<CharT>()) {}
  ~basic_dostream() {
    delete this->rdbuf();
  }
};

extern basic_dostream<char> cdebug;
extern basic_dostream<wchar_t> cwdebug;
