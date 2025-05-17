#include <folly/string.h>

// Avoid bringing in a bunch of folly threading just for setThreadName
namespace folly {
    bool setThreadName(StringPiece)
    {
      return false;
    }
  }
  