#include <bcrypt.h>

class SHA256Hasher
{
public:
  SHA256Hasher();
  ~SHA256Hasher();

  std::wstring GetHashValue();
  void HashData(const void* pb, size_t cb, const char* szDesc);

private:
  BCRYPT_ALG_HANDLE  m_hAlg;
  BCRYPT_HASH_HANDLE m_hHash;
  PBYTE              m_pbHashObject;

  SHA256Hasher(const SHA256Hasher&);
  SHA256Hasher& operator=(const SHA256Hasher&);
};
