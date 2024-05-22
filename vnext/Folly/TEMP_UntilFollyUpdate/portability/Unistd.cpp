// This is a reduced version of unistd.h from folly.
// The version in folly does not work in UWP.
// https://github.com/facebook/folly/issues/1452 tracks some of the work to get folly's unistd to work
// Until the whole of unistd is implemented in folly, we can implement the limited part we need in
// react-native-windows.

// We need to prevent winnt.h from defining the core STATUS codes,
// otherwise they will conflict with what we're getting from ntstatus.h
#define UMDF_USING_NTSTATUS

#include <folly/portability/Unistd.h>
#include <folly/portability/Windows.h>

namespace folly::portability::unistd {

long sysconf(int tp) {
  switch (tp) {
    case _SC_PAGESIZE: {
      SYSTEM_INFO inf;
      GetSystemInfo(&inf);
      return (long)inf.dwPageSize;
    }
    case _SC_NPROCESSORS_ONLN: {
      SYSTEM_INFO inf;
      GetSystemInfo(&inf);
      return (long)inf.dwNumberOfProcessors;
    }
    default:
      return -1L;
  }
}

} // namespace folly::portability::unistd