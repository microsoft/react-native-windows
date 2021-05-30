// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "object/objectRefCount.h"
#include "object/objectWithWeakRef.h"

//=============================================================================
// BadMakeAllocator helps testing allocators that may return
// null from Allocate method.
//=============================================================================

// Special test allocator that cannot allocate memory and returns nullptr.
struct BadMakeAllocator {
  static void *Allocate(size_t /*size*/) noexcept {
    return nullptr;
  }

  static void Deallocate(void * /*buffer*/) noexcept {}
};

using BadAllocSimpleRefCount = Mso::SimpleRefCountPolicy<Mso::DefaultRefCountedDeleter, BadMakeAllocator>;
using BadAllocWeakRefCount = Mso::WeakRefCountPolicy<Mso::DefaultRefCountedDeleter, BadMakeAllocator>;

//=============================================================================
// StatefulMakeAllocator helps testing stateful allocators
//=============================================================================

struct IMyMemHeap {
  virtual void *Alloc(size_t size) noexcept = 0;
  virtual void Free(void *ptr) noexcept = 0;
};

struct MyMemHeap : public IMyMemHeap {
  MyMemHeap(bool &allocCalled, bool &freeCalled) noexcept : m_allocCalled(allocCalled), m_freeCalled(freeCalled) {}

  MSO_NO_COPY_CTOR_AND_ASSIGNMENT(MyMemHeap);

  virtual void *Alloc(size_t size) noexcept override {
    m_allocCalled = true;
    return Mso::Memory::AllocateEx(size, Mso::Memory::AllocFlags::ShutdownLeak);
  }

  virtual void Free(void *ptr) noexcept override {
    m_freeCalled = true;
    Mso::Memory::Free(ptr);
  }

 private:
  bool &m_allocCalled;
  bool &m_freeCalled;
};

struct StatefulMakeAllocator {
  static const size_t HeaderSize =
      8; // To make sure that we can store a pointer and to have 8 byte alignment for object.

  static void *Allocate(size_t size, IMyMemHeap *memHeap) noexcept {
    size_t bufferSize = size + HeaderSize;
    void *buffer = memHeap->Alloc(bufferSize);
    *reinterpret_cast<IMyMemHeap **>(buffer) = memHeap;
    return static_cast<uint8_t *>(buffer) + HeaderSize;
  }

  static void Deallocate(void *ptr) noexcept {
    void *buffer = static_cast<uint8_t *>(ptr) - HeaderSize;
    IMyMemHeap *memHeap = *reinterpret_cast<IMyMemHeap **>(buffer);
    memHeap->Free(buffer);
  }
};

using StatefulAllocSimpleRefCount = Mso::SimpleRefCountPolicy<Mso::DefaultRefCountedDeleter, StatefulMakeAllocator>;
using StatefulAllocWeakRefCount = Mso::WeakRefCountPolicy<Mso::DefaultRefCountedDeleter, StatefulMakeAllocator>;

//=============================================================================
// BadStatefulAllocator helps testing stateful allocators that may return
// null from Allocate method.
//=============================================================================

struct BadStatefulAllocator {
  static void *Allocate(size_t /*size*/, IMyMemHeap * /*memHeap*/) noexcept {
    return nullptr;
  }

  static void Deallocate(void * /*ptr*/) noexcept {}
};

using BadStatefulAllocSimpleRefCount = Mso::SimpleRefCountPolicy<Mso::DefaultRefCountedDeleter, BadStatefulAllocator>;
using BadStatefulAllocWeakRefCount = Mso::WeakRefCountPolicy<Mso::DefaultRefCountedDeleter, BadStatefulAllocator>;
