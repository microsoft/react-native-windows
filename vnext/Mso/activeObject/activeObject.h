// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_ACTIVEOBJECT_ACTIVEOBJECT_H
#define MSO_ACTIVEOBJECT_ACTIVEOBJECT_H

//!
//! ActiveObject is a design pattern (https://en.wikipedia.org/wiki/Active_object)
//! where method invocation is decoupled from method execution in a way
//! that method execution is happening in a specified thread or a sequential queue,
//! while the invocation may happen from any thread.
//!
//! In this file we provide ActiveObject class template that can be used
//! as a base class for classes that follow the ActiveObject design pattern.
//! The ActiveObject constructor receives serial DispatchQueue instance, and then
//! helps to implement class members in a way that they are invoked in that queue:
//! - Constructor can be called from any thread.
//! - Override Initialize() method to run initialization code in the associated queue.
//! - Destructor by default is called from the thread where the object ref count became zero.
//! - Override Finalize() method to ensure finalization in the associated queue. Destructor is run
//!   right after the Finalize() method in the same thread.
//! - SetOnDestructedOnce sets a callback that will be called after destructor is completed.
//    It can be useful in unit test or shutdown scenarios when we must know when object destruction is completed.
//! - InvokeInQueue method invokes provided code in the queue, while keeping weak pointer to the object.
//!   Some calls may be dropped if object is deleted because we cannot acquire a strong reference.
//! - InvokeInQueueStrong method invokes provided code in the queue, while keeping strong pointer to the object.
//!   Object is not deleted until the call is completed.
//! - IsInQueue predicate returns true if execution is happening in the queue.
//! - VerifyInQueueElseCrash crashes process if code is not executed in the associated queue.
//!
//! Use ActiveField and ActiveReadableField classes to protect access to fields in classes derived from ActiveObject:
//! - ActiveField helps to ensure that the field value is read/write from the associated queue.
//! - ActiveReadableField allows to read field value from any thread or queue, but ensures that it is written
//!     only from the associated queue. It uses a mutex for synchronization.
//!
//! Other classes in this file to help implementing the ActiveObject:
//! - ActiveObjectMakePolicy calls constructor from the calling thread and
//!   Initialize() method from the associated queue if it is overridden in user code.
//! - ActiveObjectDeleter calls Finalize() method if it is overridden in user code from the associated queue,
//!   then it calls destructor. The destructor is called either in the same thread where the ref count
//!   became zero, or in the associated queue right after the Finalize() method if it is overridden in user code.
//! - ActiveObjectBase is a base non-template class for ActiveObject to reduce size of compiled code.
//!

#include <mutex>
#include <type_traits>
#include "dispatchQueue/dispatchQueue.h"
#include "object/unknownObject.h"

namespace Mso::Internal {

//! Custom make policy that calls constructor from any thread, and then calls
//! Initialize() method without parameters from the associated queue.
struct ActiveObjectMakePolicy {
  static const bool IsNoExcept{true};

  template <typename T, typename TMemoryGuard, typename... TArgs>
  static void Make(TMemoryGuard &memoryGuard, TArgs &&... args) noexcept;
};

//! Custom deleter to call Finalize() or FinalizeSync() from the associated queue.
struct ActiveObjectDeleter {
  template <typename TObject>
  static void Delete(TObject *obj) noexcept;
};

//! A base non-template class for ActiveObject to reduce size of compiled code.
MSO_STRUCT_GUID(ActiveObjectBase, "423023e6-5c16-4efe-b95a-deec043b7ea7")
struct ActiveObjectBase : IUnknown {
  friend ActiveObjectMakePolicy; // To call protected Initialize() method.
  friend ActiveObjectDeleter; // To call protected Finalize() or FinalizeSync() methods.
  friend Mso::WeakRefCountPolicy<ActiveObjectDeleter>; // To call protected destructor.

  //! Sets OnDestructed callback that is to be called after the ActiveObject-derived class destructor is completed.
  //! The onDestructed can be set only once. The next time it causes a crash.
  LIBLET_PUBLICAPI void SetOnDestructedOnce(Mso::VoidFunctor &&onDestructed) noexcept;

  //! Casts provided obj to ActiveObjectBase, calls SetOnDestructedOnce with a ManualResetEvent,
  //! sets obj to nullptr, and waits for event when object is destructed.
  //! The method can be useful for tests and shutdown scenarios.
  //! It crashes if object cannot be casted to ActiveObjectBase.
  //! It may deadlock if provided obj has more than one reference.
  LIBLET_PUBLICAPI static void ReleaseAndWait(Mso::CntPtr<IUnknown> &obj) noexcept;

 protected:
  //! Constructor that takes a queue to associated with the object being created.
  LIBLET_PUBLICAPI ActiveObjectBase(DispatchQueue const &queue) noexcept;

  //! Declare virtual destructor to correctly destroy derived instances.
  LIBLET_PUBLICAPI virtual ~ActiveObjectBase() noexcept;

  //! Can be overridden in a derived class to do initialization in the specified queue.
  LIBLET_PUBLICAPI virtual void Initialize() noexcept;

  //! Can be overridden in a derived class to do finalization in the specified queue.
  LIBLET_PUBLICAPI virtual void Finalize() noexcept;

  //! The queue associated with the object.
  DispatchQueue const &Queue() const noexcept {
    return m_queue;
  }

  //! Invokes callable object in the associated queue.
  //! Since it captures a weak reference to the object, it may do nothing if the object is already deleted.
  template <class TCallback>
  void InvokeInQueue(TCallback &&callback) noexcept {
    using Callback = std::decay_t<TCallback>;
    m_queue.InvokeElsePost([weakThis = Mso::WeakPtr<ActiveObjectBase>{this},
                            callback = Callback{std::forward<TCallback>(callback)}]() mutable noexcept {
      if (auto strongThis = weakThis.GetStrongPtr()) {
        callback();
      }
    });
  }

  //! Invokes callable object in the associated queue and keeps strong reference to it.
  //! The object is not deleted until the callback is completed.
  template <class TCallback>
  void InvokeInQueueStrong(TCallback &&callback) noexcept {
    using Callback = std::decay_t<TCallback>;
    m_queue.InvokeElsePost([strongThis = Mso::CntPtr<ActiveObjectBase>{this},
                            callback = Callback{std::forward<TCallback>(callback)}]() mutable noexcept { callback(); });
  }

  template <class TCallback>
  void InvokeInQueueStrong(TCallback &&callback) const noexcept {
    using Callback = std::decay_t<TCallback>;
    m_queue.InvokeElsePost([strongThis = Mso::CntPtr<const ActiveObjectBase>{this},
                            callback = Callback{std::forward<TCallback>(callback)}]() mutable noexcept { callback(); });
  }

  //! Returns true if method is called from the associated queue.
  LIBLET_PUBLICAPI bool IsInQueue() noexcept;

  //! Crashes if not called from the associated queue.
  LIBLET_PUBLICAPI void VerifyIsInQueueElseCrash() noexcept;

  //! Returns true if object is initialized.
  //! Initialization may fail if queue is already shutdown.
  bool IsInitialized() const noexcept {
    return m_isInitialized;
  }

 private:
  // Used internally to call object destructor and then the OnDestructed callback.
  LIBLET_PUBLICAPI static void DestructObject(ActiveObjectBase *obj) noexcept;

 private:
  DispatchQueue m_queue; //!< The queue associated with the object.
  std::atomic<Mso::IVoidFunctor *> m_onDestructed{nullptr}; //!< The callback to call after object destruction.
  bool m_hasInitialize{false};
  bool m_hasFinalize{false};
  bool m_isInitialized{false};
};

//=============================================================================================
// ActiveObjectMakePolicy implementation
//=============================================================================================

template <typename T, typename TMemoryGuard, typename... TArgs>
/*static*/ void ActiveObjectMakePolicy::Make(TMemoryGuard &memoryGuard, TArgs &&... args) noexcept {
  memoryGuard.Obj = ::new (memoryGuard.ObjMemory) T{std::forward<TArgs>(args)...};
  memoryGuard.ObjMemory = nullptr; // Memory is now controlled by the object. Set to null to avoid memory destruction.

  ActiveObjectBase *activeObj = static_cast<ActiveObjectBase *>(memoryGuard.Obj);
  activeObj->m_hasInitialize = typeid(&ActiveObjectBase::Initialize) != typeid(&T::Initialize);
  activeObj->m_hasFinalize = typeid(&ActiveObjectBase::Finalize) != typeid(&T::Finalize);

  // If we have user defined Initialize method, then call it in the associated queue.
  if (activeObj->m_hasInitialize) {
    //
    // We must use InvokeElsePost to call Initialize() to make sure that it is called before any object's
    // method in case if we are called from the queue.
    // If we would use the Msp::Async::Post below and an object is created in one Mso::Async::Post, and
    // in the next line we call Mso::Async::Post to call its method, then the method would be called
    // before the Initialize() method because it would be posted much later.
    //
    activeObj->Queue().InvokeElsePost([weakPtr = Mso::WeakPtr<ActiveObjectBase>{activeObj}]() noexcept {
      if (auto strongPtr = weakPtr.GetStrongPtr()) {
        strongPtr->Initialize();
        strongPtr->m_isInitialized = true;
      }
    });
  } else {
    activeObj->m_isInitialized = true;
  }
}

//=============================================================================================
// ActiveObjectDeleter implementation
//=============================================================================================

template <typename TObject>
/*static*/ void ActiveObjectDeleter::Delete(TObject *obj) noexcept {
  ActiveObjectBase *activeObj = static_cast<ActiveObjectBase *>(obj);

  // If we have user defined Finalize method, then call it in the associated queue.
  if (activeObj->m_isInitialized && activeObj->m_hasFinalize) {
    //
    // In case if we already inside of our queue, we delete synchronously.
    // Otherwise post to the target queue.
    //
    // Q: What if there are other pending items in the queue that need our object?
    //    Would it possible that InvokeElsePost executes destructor out-of-order?
    // A: We are here because the ref count to our object is zero.
    //    It means that pending items do not have a strong reference. They may only
    //    have a weak reference. The weak reference cannot be resolved to the strong one
    //    because the ref count is zero, and thus such pending items will do nothing.
    //    On the other hand, if we do not do the synchronous call when we can, then we
    //    may get a memory leak in case if it was the last item that the queue executes
    //    before being shutdown.
    //
    activeObj->m_queue.InvokeElsePost(Mso::MakeDispatchTask(
        [activeObj]() noexcept {
          activeObj->Finalize();
          ActiveObjectBase::DestructObject(activeObj);
        },
        /*onCancel:*/
        [activeObj]() noexcept {
          AssertSz(false, "Cannot finalize object because the queue is already shutdown.");
          ActiveObjectBase::DestructObject(activeObj);
        }));
  } else {
    // Otherwise destroy the object immediately.
    ActiveObjectBase::DestructObject(activeObj);
  }
}

//=============================================================================================
// Helper functions
//=============================================================================================

//! Crashes if called from a queue different from the provided one.
LIBLET_PUBLICAPI void VerifyIsInQueueElseCrash(DispatchQueue const &queue) noexcept;

} // namespace Mso::Internal

namespace Mso {

//! A base class to implement object that follows the ActiveObject design pattern.
//! It executes Initialize() virtual method and destructor in the associated queue.
//! For other methods we can use InvokeInQueue, IsInQueue, and VerifyIsInQueueElseCrash methods.
//! Consider to use ActiveField and ActivaeReadableField class to protect access to fields.
template <typename... TBaseTypes>
class ActiveObject : public Mso::UnknownObject<
                         Mso::WeakRefCountPolicy<Internal::ActiveObjectDeleter>,
                         Internal::ActiveObjectBase,
                         TBaseTypes...> {
  using Super = Mso::
      UnknownObject<Mso::WeakRefCountPolicy<Internal::ActiveObjectDeleter>, Internal::ActiveObjectBase, TBaseTypes...>;

 public:
  using MakePolicy = Internal::ActiveObjectMakePolicy;
  using ActiveObjectType = ActiveObject;

 protected:
  ActiveObject(DispatchQueue const &queue) noexcept : Super{queue} {}
};

//! ActiveField verifies that field is read and written from the specified queue
template <typename T>
struct ActiveField {
  ActiveField(T &&value, DispatchQueue const &queue) noexcept : m_value{std::move(value)}, m_queue{queue} {}

  ActiveField(DispatchQueue const &queue) noexcept : m_queue{queue} {}

  //! Verifies that value is loaded in the specified queue.
  T &Load() const noexcept {
    Internal::VerifyIsInQueueElseCrash(m_queue);
    return const_cast<ActiveField *>(this)->m_value;
  }

  //! Verifies that value is being stored in the specified queue.
  void Store(T &&value) const noexcept {
    Internal::VerifyIsInQueueElseCrash(m_queue);
    const_cast<ActiveField *>(this)->m_value = std::move(value);
  }

  //! Verifies that value is being stored in the specified queue.
  //! Returns previously stored value.
  T Exchange(T &&value) const noexcept {
    Internal::VerifyIsInQueueElseCrash(m_queue);
    T result{std::move(const_cast<ActiveField *>(this)->m_value)};
    const_cast<ActiveField *>(this)->m_value = std::move(value);
    return result;
  }

 private:
  T m_value{};
  DispatchQueue m_queue;
};

//! ActiveReadableField allows to read field from any thread, and it verifies
//! that the field is written only from the specified queue.
//! It uses a mutex to ensure safe field reading from a non-queue thread.
template <typename T>
struct ActiveReadableField {
  ActiveReadableField(T &&value, DispatchQueue const &queue, std::mutex &readMutex) noexcept
      : m_value{std::move(value)}, m_queue{queue}, m_readMutex{readMutex} {}

  ActiveReadableField(DispatchQueue const &queue, std::mutex &readMutex) noexcept
      : m_queue{queue}, m_readMutex{readMutex} {}

  //! Verifies that value is loaded in the specified queue.
  T &Load() const noexcept {
    Internal::VerifyIsInQueueElseCrash(m_queue);
    return const_cast<ActiveReadableField *>(this)->m_value;
  }

  //! Uses lock to load value from any thread.
  T LoadWithLock() const noexcept {
    std::lock_guard<std::mutex> lock{m_readMutex};
    return m_value;
  }

  //! Uses lock to load value from any thread.
  T LoadWithLock(std::unique_lock<std::mutex> &lock) const noexcept {
    VerifyElseCrashSzTag(
        lock.mutex() == &m_readMutex && lock.owns_lock(),
        "The lock must own specified mutex.",
        0x026e3494 /* tag_c19su */);
    return m_value;
  }

  //! Verifies that value is being stored in the specified queue.
  //! It locks specified mutex to store value.
  //! Returns previous value to avoid its destruction under the lock.
  T Exchange(T &&value) const noexcept {
    Internal::VerifyIsInQueueElseCrash(m_queue);
    std::lock_guard<std::mutex> lock{m_readMutex};

    T result{std::move(const_cast<ActiveReadableField *>(this)->m_value)};
    const_cast<ActiveReadableField *>(this)->m_value = std::move(value);
    return result;
  }

  //! Verifies that value is being stored in the specified queue.
  //! Verifies that provided lock owns the stored mutex.
  //! Returns previous value to avoid its destruction under lock.
  T Exchange(T &&value, std::unique_lock<std::mutex> &lock) const noexcept {
    Internal::VerifyIsInQueueElseCrash(m_queue);
    VerifyElseCrashSzTag(
        lock.mutex() == &m_readMutex && lock.owns_lock(),
        "The lock must own specified mutex.",
        0x02691808 /* tag_c0r6i */);

    T result{std::move(const_cast<ActiveReadableField *>(this)->m_value)};
    const_cast<ActiveReadableField *>(this)->m_value = std::move(value);
    return result;
  }

 private:
  T m_value{};
  DispatchQueue m_queue;
  std::mutex &m_readMutex;
};

} // namespace Mso

#endif // MSO_ACTIVEOBJECT_ACTIVEOBJECT_H
