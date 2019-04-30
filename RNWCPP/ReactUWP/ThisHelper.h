// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

/*
Helpers to convert a none ref counted class 'this' to a a WeakThis, then try to get a strongThis in callback function.
Like ReactControl, the class itself is not controlled by ref count, but it received event from other thread,
and then be queued by DefaultNativeMessageQueueThread()->runOnQueue to UI thread.
When ReactControl and message queue has difference life time, so
After ReactControl is destroyed, it's possible that ReactControl is still been used.
Usage:
 1. add WeakThis to the class
    WeakThis<ReactControl> m_weakThis{ this };
 2. use weakThis in queue callback
   auto weakThis = m_weakThis.This();
   m_reactInstance->DefaultNativeMessageQueueThread()->runOnQueue([weakThis]()
   {
     if (auto This = StrongThis(weakThis))
     {
       This->HandleInstanceErrorOnUIThread();
     }
   });
*/

namespace react {
  namespace uwp {
    template <typename _Tp>
    struct This
    {
      This(_Tp* This) : m_ptr(This)
      {
        m_owningThreadId = ::GetCurrentThreadId();
      }

      void clear()
      {
        if (m_owningThreadId != ::GetCurrentThreadId())
        {
          throw std::runtime_error("Expect in the same thread");
        }

        m_ptr = nullptr;
      }

      _Tp* get() const noexcept
      {
        return m_ptr;
      }
    private:
      _Tp* m_ptr{};
      DWORD m_owningThreadId{};
    };

    template <typename _Tp>
    class WeakThis
    {
      using ThisType = typename This<typename _Tp>;
    public:
      WeakThis(_Tp* This)
      {
        m_sharedThis = std::make_shared<ThisType>(This);
      }

      ~WeakThis()
      {
        m_sharedThis->clear();
      }

      std::weak_ptr<ThisType> This()
      {
        return m_sharedThis;
      }

    private:
      std::shared_ptr<ThisType> m_sharedThis;
    };

    template <typename _Tp>
    static _Tp* StrongThis(std::weak_ptr<This<_Tp>> weakThis)
    {
      if (auto strongThis = weakThis.lock())
      {
        return strongThis->get();
      }
      else
      {
        return nullptr;
      }
    }
  }
}
