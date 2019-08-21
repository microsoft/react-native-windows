#include "fbsystrace.h"

#include <fstream>

void fbsystrace_trace_raw(const char *buffer, size_t n) {
  std::ofstream stream;
  stream.open(
      "C:\\Users\\anandrag.REDMOND\\AppData\\Local\\Packages\\59b7e31b-a8fe-4fc7-85d6-a0a2475ad32e_kc2bncckyf4ap\\LocalState\\systrace.txt",
      std::ios_base::app);

  stream << "Raw"
         << " ###### " << buffer << " ####### " << n << std::endl;
  stream.close();
}

void fbsystrace_end_section(uint64_t tag) {
  std::ofstream stream;
  stream.open(
      "C:\\Users\\anandrag.REDMOND\\AppData\\Local\\Packages\\59b7e31b-a8fe-4fc7-85d6-a0a2475ad32e_kc2bncckyf4ap\\LocalState\\systrace.txt",
      std::ios_base::app);

  stream << "End"
         << " ###### " << tag << std::endl;
  stream.close();
}

void fbsystrace_end_async_flow(uint64_t tag, const char *name, int callId) {
  std::ofstream stream;
  stream.open(
      "C:\\Users\\anandrag.REDMOND\\AppData\\Local\\Packages\\59b7e31b-a8fe-4fc7-85d6-a0a2475ad32e_kc2bncckyf4ap\\LocalState\\async.txt",
      std::ios_base::app);

  stream << "End Async"
         << " ###### " << tag << " ###### " << name << " ###### " << callId  << " ###### " << std::endl;
  stream.close();
}

namespace fbsystrace {

/*static*/ void
FbSystraceAsyncFlow::begin(uint64_t tag, const char *name, int cookie) {
  std::ofstream stream;
  stream.open(
      "C:\\Users\\anandrag.REDMOND\\AppData\\Local\\Packages\\59b7e31b-a8fe-4fc7-85d6-a0a2475ad32e_kc2bncckyf4ap\\LocalState\\async.txt",
      std::ios_base::app);

  stream << "FbSystraceAsyncFlow::begin"
         << " ###### " << tag << " ###### " << name << " ###### " << cookie
         << " ###### " << std::endl;
  stream.close();
}

/*static */ void
FbSystraceAsyncFlow::end(uint64_t tag, const char *name, int cookie) {
  std::ofstream stream;
  stream.open(
      "C:\\Users\\anandrag.REDMOND\\AppData\\Local\\Packages\\59b7e31b-a8fe-4fc7-85d6-a0a2475ad32e_kc2bncckyf4ap\\LocalState\\async.txt",
      std::ios_base::app);

  stream << "End Async"
         << " ###### " << tag << " ###### " << name << " ###### " << cookie
         << " ###### " << std::endl;
  stream.close();
}

} // namespace fbsystrace
