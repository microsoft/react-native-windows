#include <bcrypt.h>
#include <optional>
#include <vector>

namespace Microsoft::ReactNative {
std::optional<std::vector<std::uint8_t>> GetSHA256Hash(const void *pb, size_t cb);

class SHA256Hasher {
 public:
  SHA256Hasher();
  ~SHA256Hasher();

  SHA256Hasher(const SHA256Hasher &) = delete;
  SHA256Hasher &operator=(const SHA256Hasher &) = delete;

  std::vector<std::uint8_t> GetHashValue();
  void HashData(const void *pb, size_t cb);

 private:
  BCRYPT_ALG_HANDLE m_hAlg;
  BCRYPT_HASH_HANDLE m_hHash;
  std::vector<std::uint8_t> m_hashObject;
};
} // namespace Microsoft::ReactNative
