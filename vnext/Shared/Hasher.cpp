#include "pch.h"
#include "Hasher.h"

#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
#endif // NT_SUCCESS

namespace Microsoft::ReactNative {

  SHA256Hasher::SHA256Hasher()
  {
    NTSTATUS status;
    if (!NT_SUCCESS(status = BCryptOpenAlgorithmProvider(&m_hAlg, BCRYPT_SHA256_ALGORITHM, NULL, 0)))
    {
      // Failed to load and initialize a CNG provider
      throw status;
    }

    DWORD cbObject;
    DWORD cbData;
    if (!NT_SUCCESS(status = BCryptGetProperty(m_hAlg, BCRYPT_OBJECT_LENGTH, (PBYTE)&cbObject, sizeof(DWORD), &cbData, 0)))
    {
      // Failed to retrieve the value for BCRYPT_OBJECT_LENGTH
      throw status;
    }

    m_hashObject.resize(cbObject);
    if (!NT_SUCCESS(status = BCryptCreateHash(m_hAlg, &m_hHash, m_hashObject.data(), static_cast<ULONG>(m_hashObject.size()), NULL, 0, 0)))
    {
      // Failed to create hash
      throw status;
    }
  }

  SHA256Hasher::~SHA256Hasher()
  {
    BCryptCloseAlgorithmProvider(m_hAlg, 0);
    BCryptDestroyHash(m_hHash);
  }

  void SHA256Hasher::HashData(const void* pb, uint32_t cb)
  {
    if (cb > LONG_MAX)
    {
      // Input too large
      throw E_INVALIDARG;
    }

    NTSTATUS status;
    if (!NT_SUCCESS(status = BCryptHashData(m_hHash, (PBYTE)pb, (DWORD)cb, 0)))
    {
      // Failed to perform final one way hash
      throw status;
    }
  }

  std::vector<std::uint8_t> SHA256Hasher::GetHashValue()
  {
    DWORD hashLength;
    DWORD cbData;
    NTSTATUS status;

    if (!NT_SUCCESS(status = BCryptGetProperty(m_hAlg, BCRYPT_HASH_LENGTH, (PBYTE)&hashLength, sizeof(DWORD), &cbData, 0)))
    {
      // Failed to retrieve the value for BCRYPT_HASH_LENGTH
      throw status;
    }

    std::vector<uint8_t> hashValue(hashLength, 0);
    if (!NT_SUCCESS(status = BCryptFinishHash(m_hHash, &hashValue[0], static_cast<ULONG>(hashValue.size()), 0)))
    {
      // Failed to retrieve hash
      throw status;
    }

    return hashValue;
  }
} // namespace Microsoft::ReactNative
