// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_CPPEXTENSIONS_AUTORESTORE_H
#define MSO_CPPEXTENSIONS_AUTORESTORE_H

/**
  Helper classes to automatically restore/cleanup/undo an operation.
*/

#include "compilerAdapters/cppMacros.h"
#include "typeTraits/typeTraits.h"

#ifdef __cplusplus
// 4091: extern __declspec(dllimport)' : ignored on left of 'double' when no variable is declared
// 4472: 'pointer_safety' is a native enum: add an access specifier (private/public) to declare a managed enum
// 4996: 'wmemcpy': This function or variable may be unsafe. Consider using wmemcpy_s instead
#pragma warning(push)
#pragma warning(disable : 4091 4472 4996)
#include <memory>
#pragma warning(pop)

// 4996: 'wmemcpy': This function or variable may be unsafe. Consider using wmemcpy_s instead
#pragma warning(push)
#pragma warning(disable : 4996)
#include <utility>
#pragma warning(pop)

namespace Mso {

/**
  Restores a variable to its original value in an exception-safe way.
  For example:
  {
    Mso::TRestorer<bool> restoreFlag(m_fFlag, true);
    ... // do something that might throw an exception
  } // m_fFlag is automatically restored to its original value
*/
template <typename T>
class TRestorer {
 public:
  TRestorer(T &value) noexcept : m_pValue(std::addressof(value)), m_previousValue(value) {}
  TRestorer(T &value, T newValue) noexcept : m_pValue(std::addressof(value)), m_previousValue(value) {
    *m_pValue = newValue;
  }
  TRestorer(TRestorer &&other) noexcept : m_pValue(other.m_pValue), m_previousValue(other.m_previousValue) {
    other.Disable();
  }
  ~TRestorer() noexcept {
    *m_pValue = m_previousValue;
  }

  MSO_NO_COPY_CTOR_AND_ASSIGNMENT(TRestorer);
  TRestorer &operator=(TRestorer &&other) noexcept = delete;

  bool IsEnabled() noexcept {
    return (m_pValue != std::addressof(m_previousValue));
  }
  void Disable() noexcept {
    m_pValue = std::addressof(m_previousValue);
  }

 private:
  T *m_pValue;
  T m_previousValue;
};

/**
  AutoRestorer() can be used to avoid being explicit with TRestorer

  Instead of:
    Mso::TRestorer<bool> restorer(m_inLayout);
  Do this:
    auto restorer = Mso::AutoRestorer(m_inLayout);
*/
template <typename T>
Mso::TRestorer<T> AutoRestorer(T &value) noexcept {
  return Mso::TRestorer<T>(value);
}

template <typename T>
Mso::TRestorer<T> AutoRestorer(T &value, T newValue) noexcept {
  return Mso::TRestorer<T>(value, newValue);
}

/**
  Restores a value to its original value (using a getter/setter) in an exception-safe way. For example:

    Foo* GetGlobalFoo()
    {
      return s_pFooGlobal;
    }

    void SetGlobalFoo(Foo* pFoo)
    {
      s_pFooGlobal = pFoo;
    }

    using GlobalFooRestorer = Mso::TPropertyRestorer<Foo*, GetGlobalFoo, SetGlobalFoo>;

    void MyMethod(Foo* pFoo)
    {
      GlobalFooRestorer rstGlobalFoo(pFoo);
      ... // do something that might throw an exception
    } // s_pFooGlobal is automatically restored to its original value
*/
template <typename T, T PfnGetter(), void PfnSetter(T value)>
struct TPropertyRestorer final {
 public:
  TPropertyRestorer() = default;
  TPropertyRestorer(T newValue) noexcept(
      noexcept(PfnSetter(PfnGetter()))) // noexcept(any expression with both Get and Set)
  {
    PfnSetter(std::forward<T>(newValue));
  }
  TPropertyRestorer(TPropertyRestorer &&other) noexcept
      : m_isDisabled(other.m_isDisabled), m_valueToRestore(std::forward<T>(other.m_valueToRestore)) {
    other.Disable();
  }

  ~TPropertyRestorer() {
    if (!m_isDisabled) {
      PfnSetter(std::forward<T>(m_valueToRestore));
    }
  }

  MSO_NO_COPY_CTOR_AND_ASSIGNMENT(TPropertyRestorer); // No copying!

  template <
      typename TSelf = TPropertyRestorer,
      typename =
          typename std::enable_if_t<std::is_move_assignable<typename std::remove_reference<T>::type>::value, TSelf>>
  TSelf &operator=(TSelf &&other) noexcept {
    if (this != &other) {
      m_isDisabled = other.m_isDisabled;
      m_valueToRestore = std::move(other.m_valueToRestore);

      other.Disable();
    }

    return *this;
  }

  bool IsEnabled() const noexcept {
    return !m_isDisabled;
  }
  void Disable() noexcept {
    m_isDisabled = true;
  }

  T &ValueToRestore() noexcept {
    return m_valueToRestore;
  }

 private:
  bool m_isDisabled = false;
  T m_valueToRestore{PfnGetter()};
};

namespace TCleanup {
/**
  TCleanup
  The last resort.  If none of the other helper classes work for you,
  or there is only a single occurrence that you feel doesn't warrant its own
  helper class, the cleanup lambda option is available.

  Runs the passed cleanup function on destruction.
  This is a handy utility to use previous "goto LError" style cleanup code
  with the early return pattern.

  EXAMPLE:
    // Specify the code to run on return in a capture-by-reference lambda
    auto cleanupCloseOLDoc = TCleanup::Make([&m_pOrigFile, &m_pNewFile]
    {
      // Close out the local file if appropriate
      if (m_pOrigFile != m_pNewFile && m_pNewFile->GetOpenCount())
      {
        m_pNewFile->BeginCmd(msoiolcmdClose);
        m_pNewFile->RecordEvent(msoiolevtCmdCompleted);
      }
    });

    hr = /some code that uses and OLDocument/;
    if (FAILED(hr))
      return hr;
      // Runs the cleanup code above before returning

   If you want to be able to disable the TCleanup from running on exit,
   you can do so by:

    if (/some condition/)
      cleanupCloseOLDoc.disable();  // The TCleanup will now not run on exit
*/
template <typename Func>
struct TCleanup {
 public:
  explicit TCleanup(const Func &fnCleanup) noexcept : m_fnCleanup(fnCleanup) {}

  explicit TCleanup(Func &&fnCleanup) noexcept : m_fnCleanup(std::move(fnCleanup)) {}

  TCleanup(TCleanup &&rVal) noexcept : m_fnCleanup(rVal.m_fnCleanup), m_isEnabled(rVal.m_isEnabled) {
    // We don't want to run the cleanup twice, so disable the original
    rVal.disable();
  }

  ~TCleanup() noexcept {
    if (m_isEnabled)
      m_fnCleanup();
  }

  MSO_NO_COPY_CTOR_AND_ASSIGNMENT(TCleanup);

  void enable() noexcept {
    m_isEnabled = true;
  }
  void disable() noexcept {
    m_isEnabled = false;
  }

 private:
  const Func m_fnCleanup;
  bool m_isEnabled = true;
};

// Constructs a TCleanup from the passed function or lambda
template <typename Func>
inline TCleanup<Func> Make(const Func &pfnCleanup) noexcept {
  return TCleanup<Func>(pfnCleanup);
}

} // namespace TCleanup

} // namespace Mso

#endif // __cplusplus

#endif // MSO_CPPEXTENSIONS_AUTORESTORE_H
