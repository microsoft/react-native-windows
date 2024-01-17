#include "pch.h"
#include "Hasher.h"

#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
#endif // NT_SUCCESS

#define CheckNTSuccess(s)      \
  do {                         \
    NTSTATUS status = (s);     \
    if (!NT_SUCCESS(status)) { \
      throw status;            \
    }                          \
  } while (false)

namespace Microsoft::ReactNative {

std::optional<std::vector<std::uint8_t>> GetSHA256Hash(const void *pb, size_t cb) {
  try {
    SHA256Hasher hasher;
    hasher.HashData(pb, cb);
    return hasher.GetHashValue();
  } catch (...) {
    return std::nullopt;
  }
}

SHA256Hasher::SHA256Hasher() {
  CheckNTSuccess(BCryptOpenAlgorithmProvider(&m_hAlg, BCRYPT_SHA256_ALGORITHM, NULL, 0));

  DWORD cbObject;
  DWORD cbData;
  CheckNTSuccess(BCryptGetProperty(m_hAlg, BCRYPT_OBJECT_LENGTH, (PBYTE)&cbObject, sizeof(DWORD), &cbData, 0));

  m_hashObject.resize(cbObject);
  CheckNTSuccess(
      BCryptCreateHash(m_hAlg, &m_hHash, m_hashObject.data(), static_cast<ULONG>(m_hashObject.size()), NULL, 0, 0));
}

SHA256Hasher::~SHA256Hasher() {
  BCryptDestroyHash(m_hHash);
  BCryptCloseAlgorithmProvider(m_hAlg, 0);
}

void SHA256Hasher::HashData(const void *pb, size_t cb) {
  if (cb > LONG_MAX) {
    // Input too large
    throw E_INVALIDARG;
  }

  CheckNTSuccess(BCryptHashData(m_hHash, (PBYTE)pb, (DWORD)cb, 0));
}

std::vector<std::uint8_t> SHA256Hasher::GetHashValue() {
  DWORD hashLength;
  DWORD cbData;
  CheckNTSuccess(BCryptGetProperty(m_hAlg, BCRYPT_HASH_LENGTH, (PBYTE)&hashLength, sizeof(DWORD), &cbData, 0));

  std::vector<uint8_t> hashValue(hashLength, 0);
  CheckNTSuccess(BCryptFinishHash(m_hHash, &hashValue[0], static_cast<ULONG>(hashValue.size()), 0));

  return hashValue;
}
} // namespace Microsoft::ReactNative
