#include <bcrypt.h>
#include <vector>

namespace Microsoft::ReactNative {
  class SHA256Hasher
  {
  public:
    SHA256Hasher();
    ~SHA256Hasher();

    std::vector<std::uint8_t> GetHashValue();
    void HashData(const void* pb, uint32_t cb);

  private:
    BCRYPT_ALG_HANDLE m_hAlg;
    BCRYPT_HASH_HANDLE m_hHash;
    std::vector<std::uint8_t> m_hashObject;

    SHA256Hasher(const SHA256Hasher&);
    SHA256Hasher& operator=(const SHA256Hasher&);
  };
} // namespace Microsoft::ReactNative
