// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

/*
Some class like ReactControl, it queues message with 'this', and then resolves to 'this' when processing the message.
Because ReactControl and message queue have different lifetime, it's possible that 'this' is destroyed when the message is handled.
If 'this' is directly passed to message, it may crash the application. To workaround the problem, 'this' is wrapped to ThisWrapper/'This'.
'This' is shared_ptr, so a weak_ptr of 'This' is created and passed to the queue. When the message is processed,
weak_ptr of 'This' is resolved to 'This', then 'this'.
To use ThisWrapper, This constructor and This.get should be executed in the same thread.

Initialization:
  this -> ThisWrapper -> This
Queue message:
  ThisWrapper -> weak_ptr of This
~this:
  ~this -> ~ThisWrapper -> This.clear(not point to this but nullptr)
Process the message:
  weak_ptr of This -> resolved to shared_ptr of This or nullptr -> resolved to 'this' or nullptr

Usage:
 1. add ThisWrapper to the class
    ThisWrapper<ReactControl> m_thisWrapper{ this };
 2. pass This in queue callback
   auto weakThis = m_thisWrapper.WeakThis();
   m_reactInstance->DefaultNativeMessageQueueThread()->runOnQueue([weakThis]()
   {
     if (auto This = FromWeakThis(weakThis))
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
        m_ptr = nullptr;
      }

      _Tp* get() const
      {
        if (m_ptr != nullptr && m_owningThreadId != ::GetCurrentThreadId())
        {
          throw std::runtime_error("Because raw pointer 'this' is returned, we expect get is executed in the same thread. Otherwise we may still using 'this' but 'this' is being destroyed in another thread");
        }

        return m_ptr;
      }
    private:
      _Tp* m_ptr{};
      DWORD m_owningThreadId{};
    };

    template <typename _Tp>
    class ThisWrapper
    {
      using ThisType = typename This<typename _Tp>;
    public:
      ThisWrapper(_Tp* This)
      {
        m_sharedThis = std::make_shared<ThisType>(This);
      }

      ~ThisWrapper()
      {
        m_sharedThis->clear();
      }

      std::weak_ptr<ThisType> WeakThis()
      {
        return m_sharedThis;
      }

    private:
      std::shared_ptr<ThisType> m_sharedThis;
    };

    template <typename _Tp>
    static _Tp* FromWeakThis(std::weak_ptr<This<_Tp>> weakThis)
    {
      if (auto This = weakThis.lock())
      {
        return This->get();
      }
      else
      {
        return nullptr;
      }
    }
  }
}
