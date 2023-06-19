#include "pch.h"
#include "Hasher.h"

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

  m_pbHashObject = new BYTE[cbObject];
  if (!NT_SUCCESS(status = BCryptCreateHash(m_hAlg, &m_hHash, m_pbHashObject, cbObject, NULL, 0, 0)))
  {
    // Failed to create hash
    throw status;
  }
}

SHA256Hasher::~SHA256Hasher()
{
  BCryptCloseAlgorithmProvider(m_hAlg, 0);
  BCryptDestroyHash(m_hHash);
  delete[] m_pbHashObject;
}

void SHA256Hasher::HashData(const void* pb, size_t cb, const char* szDesc)
{
  NTSTATUS status;

  while (cb > LONG_MAX)
  {
    if (!NT_SUCCESS(status = BCryptHashData(m_hHash, (PBYTE)pb, LONG_MAX, 0)))
    {
      // Failed to perform one way hash
      throw status;
    }
    pb = (BYTE*)pb + LONG_MAX;
    cb -= LONG_MAX;
  }
  if (!NT_SUCCESS(status = BCryptHashData(m_hHash, (PBYTE)pb, (DWORD)cb, 0)))
  {
    // Failed to perform final one way hash
    throw status;
  }
}

wstring SHA256Hasher::GetHashValue()
{
  PBYTE pbHashValue;
  DWORD cbHashValue;
  DWORD cbData;
  NTSTATUS status;

  if (!NT_SUCCESS(status = BCryptGetProperty(m_hAlg, BCRYPT_HASH_LENGTH, (PBYTE)&cbHashValue, sizeof(DWORD), &cbData, 0)))
  {
    // Failed to retrieve the value for BCRYPT_HASH_LENGTH
    throw status;
  }

  pbHashValue = new BYTE[cbHashValue];
  if (!NT_SUCCESS(status = BCryptFinishHash(m_hHash, pbHashValue, cbHashValue, 0)))
  {
    // Failed to retrieve hash
    throw status;
  }

  wstring result;
  result.reserve(cbHashValue * 2);
  for (DWORD i = 0; i < cbHashValue; i++)
  {
    result.push_back(tohex(pbHashValue[i] >> 4));
    result.push_back(tohex(pbHashValue[i] & 15));
  }

  delete[] pbHashValue;
  return result;
}
