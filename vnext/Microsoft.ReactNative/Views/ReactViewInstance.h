// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <IReactInstance.h>
#include <object/unknownObject.h>
#include "ReactHost/React.h"
#include "ReactRootView.h"

namespace Microsoft::ReactNative {

//! This class ensures that we access ReactRootView from UI thread.
struct ReactViewInstance : public Mso::UnknownObject<Mso::RefCountStrategy::WeakRef, Mso::React::IReactViewInstance> {
  ReactViewInstance(
      winrt::weak_ref<winrt::Microsoft::ReactNative::implementation::ReactRootView> &&weakRootControl,
      Mso::DispatchQueue const &uiQueue) noexcept;

  Mso::Future<void> InitRootView(
      Mso::CntPtr<Mso::React::IReactInstance> &&reactInstance,
      Mso::React::ReactViewOptions &&viewOptions) noexcept override;
  Mso::Future<void> UpdateRootView() noexcept override;
  Mso::Future<void> UninitRootView() noexcept override;

 private:
  template <class TAction>
  Mso::Future<void> PostInUIQueue(TAction &&action) noexcept;

 private:
  winrt::weak_ref<winrt::Microsoft::ReactNative::implementation::ReactRootView> m_weakRootControl;
  Mso::DispatchQueue m_uiQueue;
};

//===========================================================================
// ReactViewInstance inline implementation
//===========================================================================

template <class TAction>
inline Mso::Future<void> ReactViewInstance::PostInUIQueue(TAction &&action) noexcept {
  // ReactViewInstance has shorter lifetime than ReactRootControl. Thus, we capture this WeakPtr.
  return Mso::PostFuture(
      m_uiQueue, [weakThis = Mso::WeakPtr{this}, action{std::forward<TAction>(action)}]() mutable noexcept {
        if (auto strongThis = weakThis.GetStrongPtr()) {
          if (auto rootControl = strongThis->m_weakRootControl.get()) {
            action(rootControl);
            return Mso::Maybe<void>{};
          }
        }

        return Mso::CancellationErrorProvider().MakeMaybe<void>();
      });
}

} // namespace Microsoft::ReactNative
