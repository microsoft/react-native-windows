#include <cstdlib>
#include <sys/types.h>

extern "C" {
  int getrusage(int who, struct rusage *usage) {
    std::abort();
    return 0;
  }

  void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
    std::abort();
    return nullptr;
  }

  int mlock(const void *addr, size_t len) {
    std::abort();
    return 0;
  }

  int mprotect(void *addr, size_t len, int prot) {
    std::abort();
    return 0;
  }
}
