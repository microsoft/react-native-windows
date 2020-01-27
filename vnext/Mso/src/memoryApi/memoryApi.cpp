// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "memoryApi/memoryApi.h"
#include <cstdlib>
#include <memory>

#if !__clang__ && !__GNUC__
#pragma detect_mismatch("Allocator", "Crt")
#endif

__declspec(noreturn) void ThrowOOM() {
  throw std::bad_alloc();
}

namespace Mso {
namespace Memory {

_Use_decl_annotations_ void *AllocateEx(size_t cb, uint32_t /*allocFlags*/) noexcept {
  return ::malloc(cb);
}

_Use_decl_annotations_ void *Reallocate(void **ppv, size_t cb) noexcept {
  if (ppv == nullptr)
    return Mso::Memory::Allocate(cb);

  if (*ppv == nullptr) {
    *ppv = Mso::Memory::Allocate(cb);
    return *ppv;
  }

  void *pv = ::realloc(*ppv, cb);
  if (pv != nullptr) {
    *ppv = pv;
  } else if (cb == 0) {
    // HeapReAlloc with 0 size returns valid pointer and we want all implementations do the same
    // realloc(ptr, 0) on Windows or Mac/iOS with ASAN frees the original pointer and returns null
    // std lib on Mac/iOS returns a valid 0-sized pointer
    // We want to standardize to have only one behavior in shared code
    // so let's allocate a new 0-sized block if resize(ptr, 0) returns nullptr
    pv = ::malloc(0);
    *ppv = pv;
  }
  // else pv = nullptr, cb != 0: if realloc truly failed, the original ptr is untouched

  return pv;
}

_Use_decl_annotations_ void Free(void *pv) noexcept {
  ::free(pv);
}

//#ifdef DEBUG
// void RegisterCallback(Mso::LibletAPI::ILibletMemoryMarking&) noexcept {}
//
// void UnregisterCallback(Mso::LibletAPI::ILibletMemoryMarking&) noexcept {}
//#endif

} // namespace Memory
} // namespace Mso

#ifdef DEBUG
void MsoSetLazyLeakDetection(const void *) noexcept {}

void MsoSetShutdownLeakDetection(const void *) noexcept {}

BOOL FMemHeapMsoSaveBeHost(
    void * /*pinst*/,
    LPARAM /*lParam*/,
    const void * /*pvBlock*/,
    LONG_PTR /*cb*/,
    struct IMsoMemHeap * /*pmmh*/) noexcept {
  return true;
}

// MSOAPI_(VOID) MsoDebugRegisterLazyObject(IMsoDebugLazyObject* /*pidlo*/) noexcept {}
// MSOAPI_(VOID) MsoDebugUnregisterLazyObjectThreaded(IMsoDebugLazyObject* /*pidlo*/, bool /*fMainThread*/) noexcept {}

#endif // DEBUG
