#include <folly/portability/PThread.h>

#include <folly/portability/Sched.h>
#include <folly/portability/Time.h>
#include <folly/portability/Windows.h>

namespace folly {
namespace portability {
namespace pthread {
int pthread_key_create(pthread_key_t *key, void (*destructor)(void *)) {
  DWORD newKey = FlsAlloc(nullptr);

#pragma warning(push)
#pragma warning(disable : 4312)
  *key = reinterpret_cast<pthread_key_t>(newKey);
#pragma warning(pop)

  return 0;
}

int pthread_key_delete(pthread_key_t key) {
#pragma warning(push)
#pragma warning(disable : 4311)
#pragma warning(disable : 4302)
  DWORD index = reinterpret_cast<DWORD>(key);
#pragma warning(pop)

  FlsFree(index);
  return 0;
}

void *pthread_getspecific(pthread_key_t key) {
#pragma warning(push)
#pragma warning(disable : 4311)
#pragma warning(disable : 4302)
  DWORD index = reinterpret_cast<DWORD>(key);
#pragma warning(pop)

  return FlsGetValue(index);
}

int pthread_setspecific(pthread_key_t key, const void *value) {
#pragma warning(push)
#pragma warning(disable : 4311)
#pragma warning(disable : 4302)
  DWORD index = reinterpret_cast<DWORD>(key);
#pragma warning(pop)

  int ret = FlsSetValue(index, const_cast<void *>(value));
  if (ret != 0)
    return 0;
  else
    return 1;
  return 0;
}
} // namespace pthread
} // namespace portability
} // namespace folly



#define _SC_NPROCESSORS_CONF 2
namespace folly {
namespace portability {
namespace unistd {
long sysconf(int tp) {
  if (tp == _SC_NPROCESSORS_CONF)
    return 0;

  std::abort();
  return 0;
}
} // namespace unistd
} // namespace portability
} // namespace folly

int nanosleep(const struct timespec *request, struct timespec *remain) {
  Sleep((DWORD)((request->tv_sec * 1000) + (request->tv_nsec / 1000000)));
  if (remain != nullptr) {
    remain->tv_nsec = 0;
    remain->tv_sec = 0;
  }
  return 0;
}
