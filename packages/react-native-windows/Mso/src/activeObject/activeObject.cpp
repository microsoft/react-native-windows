// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "activeObject/activeObject.h"
#include "eventWaitHandle/eventWaitHandle.h"

namespace Mso::Internal {

LIBLET_PUBLICAPI ActiveObjectBase::ActiveObjectBase(DispatchQueue const &queue) noexcept : m_queue{queue} {
  VerifyElseCrashSz(m_queue, "Queue must not be null");
  VerifyElseCrashSz(m_queue.IsSerial(), "Queue must be sequential");
}

LIBLET_PUBLICAPI ActiveObjectBase::~ActiveObjectBase() noexcept {
  VerifyElseCrashSz(!m_onDestructed.load(), "The OnDestructed must be removed in the type Deleter.");
}

LIBLET_PUBLICAPI void ActiveObjectBase::SetOnDestructedOnce(Mso::VoidFunctor &&onDestructed) noexcept {
  Mso::VoidFunctor onDestructedLocal{std::move(onDestructed)};
  const IVoidFunctor *prevOnDestructed = m_onDestructed.exchange(onDestructedLocal.Get());
  VerifyElseCrashSzTag(!prevOnDestructed, "The OnDestructed can be assigned only once.", 0x027463e1 /* tag_c3gp7 */);
  onDestructedLocal.Detach(); // The object is moved to m_onDestructed.
}

LIBLET_PUBLICAPI /*static*/ void ActiveObjectBase::DestructObject(ActiveObjectBase *obj) noexcept {
  Mso::CntPtr<Mso::IVoidFunctor> onDestructed{obj->m_onDestructed.exchange(nullptr), Mso::AttachTag};
  Mso::WeakRefCountPolicy<ActiveObjectDeleter>::Delete(obj);
  if (onDestructed) {
    onDestructed->Invoke();
  }
}

LIBLET_PUBLICAPI void ActiveObjectBase::Initialize() noexcept {
  VerifyElseCrashSzTag(
      false, "This method must not be called. We must only call overridden method.", 0x027463e2 /* tag_c3gp8 */);
}

LIBLET_PUBLICAPI void ActiveObjectBase::Finalize() noexcept {
  VerifyElseCrashSzTag(
      false, "This method must not be called. We must only call overridden method.", 0x027463e3 /* tag_c3gp9 */);
}

LIBLET_PUBLICAPI bool ActiveObjectBase::IsInQueue() noexcept {
  return m_queue.IsCurrentQueue();
}

LIBLET_PUBLICAPI void ActiveObjectBase::VerifyIsInQueueElseCrash() noexcept {
  VerifyElseCrashSzTag(IsInQueue(), "The code must run in object's queue.", 0x02746400 /* tag_c3gqa */);
}

LIBLET_PUBLICAPI /*static*/ void ActiveObjectBase::ReleaseAndWait(Mso::CntPtr<IUnknown> &obj) noexcept {
  Mso::ManualResetEvent waiter;
  ActiveObjectBase &activeObj = query_cast<ActiveObjectBase &>(*obj);
  activeObj.SetOnDestructedOnce([waiter]() noexcept { waiter.Set(); });
  obj = nullptr;
  waiter.Wait();
}

LIBLET_PUBLICAPI void VerifyIsInQueueElseCrash(DispatchQueue const &queue) noexcept {
  VerifyElseCrashSzTag(queue.CurrentQueue(), "The code must run in specified queue.", 0x02746401 /* tag_c3gqb */);
}

} // namespace Mso::Internal
