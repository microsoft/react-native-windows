// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "Crypto.h"

#include <Windows.h>
#include <bcrypt.h>

namespace Microsoft::Common::Crypto {

Sha256Hash calculateSha256Hash(const void *data, size_t bufferSize) {
  auto destroyAlgHandle = [](BCRYPT_ALG_HANDLE *handle) { BCryptCloseAlgorithmProvider(handle, 0); };
  using BCryptAlgHandle = std::unique_ptr<BCRYPT_ALG_HANDLE, decltype(destroyAlgHandle)>;
  using BCryptHashHandle = std::unique_ptr<BCRYPT_HASH_HANDLE, decltype(&BCryptDestroyHash)>;

  Sha256Hash sha256ret;

  BCryptAlgHandle algHandle(nullptr, destroyAlgHandle);
  if (!BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(algHandle.get(), BCRYPT_SHA256_ALGORITHM, nullptr, 0))) {
    throw std::exception("Unexpected failure calling BCryptOpenAlgorithmProvider");
  }

  BCryptHashHandle hashHandle(nullptr, &BCryptDestroyHash);
  if (!BCRYPT_SUCCESS(
          BCryptCreateHash(*algHandle, hashHandle.get(), sha256ret.data(), sha256ret.size(), nullptr, 0, 0))) {
    throw std::exception("Unexpected failure calling BCryptCreateHash");
  }

  PUCHAR dataUint = static_cast<PUCHAR>(const_cast<void *>(data));
  if (!BCRYPT_SUCCESS(BCryptHashData(*hashHandle, dataUint, bufferSize, 0))) {
    throw std::exception("Unexpected failure calling BCryptHashData");
  }

  return sha256ret;
}
} // namespace Microsoft::Common::Crypto
