// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "object/weakPtr.h"
#include "motifCpp/testCheck.h"
#include "object/refCountedObject.h"

struct DECLSPEC_NOVTABLE IWeakPtrSample1 : Mso::IRefCounted {
  virtual int GetValue1() = 0;
};

struct DECLSPEC_NOVTABLE IWeakPtrSample2 : public Mso::IRefCounted {
  virtual int GetValue2() = 0;
};

struct DECLSPEC_NOVTABLE IWeakPtrSample3 : public Mso::IWeakRefCounted {
  virtual int GetValue3() = 0;
};

class WeakPtrSample1 final
    : public Mso::RefCountedObject<Mso::RefCountStrategy::WeakRef, IWeakPtrSample1, IWeakPtrSample2> {
 public:
  WeakPtrSample1(bool &deleted) noexcept : m_deleted(deleted) {}

  virtual int GetValue1() noexcept override {
    return 1;
  }

  virtual int GetValue2() noexcept override {
    return 2;
  }

  std::function<void()> PassThisToLambda() {
    Mso::WeakPtr<WeakPtrSample1> weakThis(this);
    auto lambda = [weakThis]() {
      auto strongThis = weakThis.GetStrongPtr();
      TestAssert::IsNotNull(strongThis.Get(), L"Cannot get StrongPtr");
    };

    return lambda; // causes an extra copy of lambda.
  }

 protected:
  virtual ~WeakPtrSample1() noexcept {
    m_deleted = true;
  }

 private:
  bool &m_deleted;
};

class WeakPtrSample3 final : public Mso::RefCountedObject<Mso::RefCountStrategy::WeakRef, IWeakPtrSample3> {
 public:
  WeakPtrSample3(bool &deleted) noexcept : m_deleted(deleted) {}

  virtual int GetValue3() noexcept override {
    return 3;
  }

 private:
  virtual ~WeakPtrSample3() noexcept {
    m_deleted = true;
  }

  bool &m_deleted;
};

TestClassComponent(ObjectWeakPtrTest, Mso.ObjectWeakPtr);
TEST_CLASS (ObjectWeakPtrTest) {
  TEST_METHOD(WeakPtr_Default) {
    Mso::WeakPtr<WeakPtrSample1> weakPtr;

    TestAssert::IsTrue(weakPtr.IsExpired());
    TestAssert::IsNull(weakPtr.GetStrongPtr().Get(), L"Expected null");
  }

  TEST_METHOD(WeakPtr_Null) {
    Mso::WeakPtr<WeakPtrSample1> weakPtr = nullptr;

    TestAssert::IsTrue(weakPtr.IsExpired());
    TestAssert::IsNull(weakPtr.GetStrongPtr().Get(), L"Expected null");
  }

  // Pointer to a WeakPtrSample1 assigned to Mso::WeakPtr<WeakPtrSample1>
  TEST_METHOD(WeakPtr_PointerAndWeakRef1) {
    bool deleted = false;
    {
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      Mso::WeakPtr<WeakPtrSample1> weakPtr(ptr.Get(), &ptr->GetWeakRef());

      TestAssert::IsFalse(weakPtr.IsExpired());
      TestAssert::IsNotNull(weakPtr.GetStrongPtr().Get());
      TestAssert::IsTrue(ptr.Get() == weakPtr.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
  }

  // Pointer to a WeakPtrSample1 assigned to Mso::WeakPtr<IWeakPtrSample1>
  TEST_METHOD(WeakPtr_PointerAndWeakRef21) {
    bool deleted = false;
    {
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      Mso::WeakPtr<IWeakPtrSample1> weakPtr(ptr.Get(), &ptr->GetWeakRef());

      TestAssert::IsFalse(weakPtr.IsExpired());
      TestAssert::IsNotNull(weakPtr.GetStrongPtr().Get());
      TestAssert::IsTrue(ptr.Get() == weakPtr.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
  }

  // Pointer to a WeakPtrSample1 assigned to Mso::WeakPtr<IWeakPtrSample2>
  // IWeakPtrSample2 is the second in the inheritance chain and we need to make sure void* casting in Mso::WeakPtr
  // works.
  TEST_METHOD(WeakPtr_PointerAndWeakRef22) {
    bool deleted = false;
    {
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);

      Mso::WeakPtr<IWeakPtrSample2> weakPtr1(ptr.Get());
      TestAssert::IsFalse(weakPtr1.IsExpired());
      TestAssert::IsNotNull(weakPtr1.GetStrongPtr().Get());
      TestAssert::IsTrue(static_cast<IWeakPtrSample2 *>(ptr.Get()) == weakPtr1.GetStrongPtr().Get());

      Mso::WeakPtr<IWeakPtrSample2> weakPtr2(ptr.Get(), &ptr->GetWeakRef());
      TestAssert::IsFalse(weakPtr2.IsExpired());
      TestAssert::IsNotNull(weakPtr2.GetStrongPtr().Get());
      TestAssert::IsTrue(static_cast<IWeakPtrSample2 *>(ptr.Get()) == weakPtr2.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
  }

  // Pointer to a IWeakPtrSample1 assigned to Mso::WeakPtr<IWeakPtrSample1>
  TEST_METHOD(WeakPtr_PointerAndWeakRef3) {
    bool deleted = false;
    {
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      Mso::WeakPtr<IWeakPtrSample1> weakPtr(static_cast<IWeakPtrSample1 *>(ptr.Get()), &ptr->GetWeakRef());

      TestAssert::IsFalse(weakPtr.IsExpired());
      TestAssert::IsNotNull(weakPtr.GetStrongPtr().Get());
      TestAssert::IsTrue(ptr.Get() == weakPtr.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
  }

  // Null pointer to a WeakPtrSample1 assigned to Mso::WeakPtr<WeakPtrSample1>
  TEST_METHOD(WeakPtr_NullPointerAndWeakRef) {
    bool deleted = false;
    {
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      WeakPtrSample1 *ptrNull = nullptr;
      Mso::WeakPtr<WeakPtrSample1> weakPtr(ptrNull, &ptr->GetWeakRef());

      TestAssert::IsTrue(weakPtr.IsExpired());
      TestAssert::IsNull(weakPtr.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
  }

  // Pointer to a WeakPtrSample1 assigned to Mso::WeakPtr<WeakPtrSample1>
  TEST_METHOD(WeakPtr_Pointer1) {
    bool deleted = false;
    {
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      Mso::WeakPtr<WeakPtrSample1> weakPtr(ptr.Get());

      TestAssert::IsFalse(weakPtr.IsExpired());
      TestAssert::IsNotNull(weakPtr.GetStrongPtr().Get());
      TestAssert::IsTrue(ptr.Get() == weakPtr.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
  }

  // Pointer to a WeakPtrSample1 assigned to Mso::WeakPtr<IWeakPtrSample1>
  TEST_METHOD(WeakPtr_Pointer2) {
    bool deleted = false;
    {
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      Mso::WeakPtr<IWeakPtrSample1> weakPtr(ptr.Get());

      TestAssert::IsFalse(weakPtr.IsExpired());
      TestAssert::IsNotNull(weakPtr.GetStrongPtr().Get());
      TestAssert::IsTrue(ptr.Get() == weakPtr.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
  }

  // Null pointer to a WeakPtrSample1 assigned to Mso::WeakPtr<WeakPtrSample1>
  TEST_METHOD(WeakPtr_NullPointer) {
    WeakPtrSample1 *ptrNull = nullptr;
    Mso::WeakPtr<WeakPtrSample1> weakPtr(ptrNull);

    TestAssert::IsTrue(weakPtr.IsExpired());
    TestAssert::IsNull(weakPtr.GetStrongPtr().Get());
  }

  // Pointer to a WeakPtrSample1 assigned to Mso::WeakPtr<WeakPtrSample1>
  TEST_METHOD(WeakPtr_CntPtrAndWeakRef1) {
    bool deleted = false;
    {
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      Mso::WeakPtr<WeakPtrSample1> weakPtr(ptr, &ptr->GetWeakRef());

      TestAssert::IsFalse(weakPtr.IsExpired());
      TestAssert::IsNotNull(weakPtr.GetStrongPtr().Get());
      TestAssert::IsTrue(ptr.Get() == weakPtr.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
  }

  // Mso::CntPtr<WeakPtrSample1> is assigned to Mso::WeakPtr<IWeakPtrSample1>
  TEST_METHOD(WeakPtr_CntPtrAndWeakRef21) {
    bool deleted = false;
    {
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      Mso::WeakPtr<IWeakPtrSample1> weakPtr(ptr, &ptr->GetWeakRef());

      TestAssert::IsFalse(weakPtr.IsExpired());
      TestAssert::IsNotNull(weakPtr.GetStrongPtr().Get());
      TestAssert::IsTrue(ptr.Get() == weakPtr.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
  }

  // Mso::CntPtr<WeakPtrSample1> is assigned to Mso::WeakPtr<IWeakPtrSample1>
  TEST_METHOD(WeakPtr_CntPtrAndWeakRef22) {
    bool deleted = false;
    {
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);

      Mso::WeakPtr<IWeakPtrSample2> weakPtr1(ptr);
      TestAssert::IsFalse(weakPtr1.IsExpired());
      TestAssert::IsNotNull(weakPtr1.GetStrongPtr().Get());
      TestAssert::IsTrue(static_cast<IWeakPtrSample2 *>(ptr.Get()) == weakPtr1.GetStrongPtr().Get());

      Mso::WeakPtr<IWeakPtrSample2> weakPtr2(ptr, &ptr->GetWeakRef());
      TestAssert::IsFalse(weakPtr2.IsExpired());
      TestAssert::IsNotNull(weakPtr2.GetStrongPtr().Get());
      TestAssert::IsTrue(static_cast<IWeakPtrSample2 *>(ptr.Get()) == weakPtr2.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
  }

  // Pointer to a IWeakPtrSample1 assigned to Mso::WeakPtr<IWeakPtrSample1>
  TEST_METHOD(WeakPtr_CntPtrAndWeakRef3) {
    bool deleted = false;
    {
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      Mso::WeakPtr<IWeakPtrSample1> weakPtr(static_cast<IWeakPtrSample1 *>(ptr.Get()), &ptr->GetWeakRef());

      TestAssert::IsFalse(weakPtr.IsExpired());
      TestAssert::IsNotNull(weakPtr.GetStrongPtr().Get());
      TestAssert::IsTrue(ptr.Get() == weakPtr.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
  }

  // Null pointer to a WeakPtrSample1 assigned to Mso::WeakPtr<WeakPtrSample1>
  TEST_METHOD(WeakPtr_NullCntPtrAndWeakRef) {
    bool deleted = false;
    {
      Mso::CntPtr<WeakPtrSample1> ptr{Mso::Make<WeakPtrSample1>(/*ref*/ deleted)};
      Mso::CntPtr<WeakPtrSample1> ptrNull{nullptr};
      Mso::WeakPtr<WeakPtrSample1> weakPtr{ptrNull, &ptr->GetWeakRef()};

      TestAssert::IsTrue(weakPtr.IsExpired());
      TestAssert::IsNull(weakPtr.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
  }

  // Pointer to a WeakPtrSample1 assigned to Mso::WeakPtr<WeakPtrSample1>
  TEST_METHOD(WeakPtr_CntPtr1) {
    bool deleted = false;
    {
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      Mso::WeakPtr<WeakPtrSample1> weakPtr(ptr);

      TestAssert::IsFalse(weakPtr.IsExpired());
      TestAssert::IsNotNull(weakPtr.GetStrongPtr().Get());
      TestAssert::IsTrue(ptr.Get() == weakPtr.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
  }

  // Pointer to a WeakPtrSample1 assigned to Mso::WeakPtr<IWeakPtrSample1>
  TEST_METHOD(WeakPtr_CntPtr2) {
    bool deleted = false;
    {
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      Mso::WeakPtr<IWeakPtrSample1> weakPtr(ptr);

      TestAssert::IsFalse(weakPtr.IsExpired());
      TestAssert::IsNotNull(weakPtr.GetStrongPtr().Get());
      TestAssert::IsTrue(ptr.Get() == weakPtr.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
  }

  // Null pointer to a WeakPtrSample1 assigned to Mso::WeakPtr<WeakPtrSample1>
  TEST_METHOD(WeakPtr_NullCntPtr) {
    Mso::CntPtr<WeakPtrSample1> ptrNull{nullptr};
    Mso::WeakPtr<WeakPtrSample1> weakPtr{ptrNull};

    TestAssert::IsTrue(weakPtr.IsExpired());
    TestAssert::IsNull(weakPtr.GetStrongPtr().Get());
  }

  TEST_METHOD(WeakPtr_CopyConstructor) {
    bool deleted = false;
    {
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      Mso::WeakPtr<WeakPtrSample1> weakPtr1(ptr);
      Mso::WeakPtr<WeakPtrSample1> weakPtr2(weakPtr1);

      TestAssert::IsFalse(weakPtr1.IsExpired());
      TestAssert::IsNotNull(weakPtr1.GetStrongPtr().Get());
      TestAssert::IsTrue(ptr.Get() == weakPtr1.GetStrongPtr().Get());

      TestAssert::IsFalse(weakPtr2.IsExpired());
      TestAssert::IsNotNull(weakPtr2.GetStrongPtr().Get());
      TestAssert::IsTrue(ptr.Get() == weakPtr2.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(WeakPtr_CopyConstructorInterface) {
    bool deleted = false;
    {
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      Mso::WeakPtr<WeakPtrSample1> weakPtr1(ptr);
      Mso::WeakPtr<IWeakPtrSample1> weakPtr2(weakPtr1);

      TestAssert::IsFalse(weakPtr1.IsExpired());
      TestAssert::IsNotNull(weakPtr1.GetStrongPtr().Get());
      TestAssert::IsTrue(ptr.Get() == weakPtr1.GetStrongPtr().Get());

      TestAssert::IsFalse(weakPtr2.IsExpired());
      TestAssert::IsNotNull(weakPtr2.GetStrongPtr().Get());
      TestAssert::IsTrue(ptr.Get() == weakPtr2.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(WeakPtr_CopyConstructorInterface2) {
    bool deleted = false;
    {
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      Mso::WeakPtr<WeakPtrSample1> weakPtr1(ptr);
      Mso::WeakPtr<IWeakPtrSample2> weakPtr2(weakPtr1);

      TestAssert::IsFalse(weakPtr1.IsExpired());
      TestAssert::IsNotNull(weakPtr1.GetStrongPtr().Get());
      TestAssert::IsTrue(ptr.Get() == weakPtr1.GetStrongPtr().Get());

      TestAssert::IsFalse(weakPtr2.IsExpired());
      TestAssert::IsNotNull(weakPtr2.GetStrongPtr().Get());
      TestAssert::IsTrue(static_cast<IWeakPtrSample2 *>(ptr.Get()) == weakPtr2.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(WeakPtr_MoveConstructor) {
    bool deleted = false;
    {
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      Mso::WeakPtr<WeakPtrSample1> weakPtr1(ptr);
      Mso::WeakPtr<WeakPtrSample1> weakPtr2(std::move(weakPtr1));

      TestAssert::IsTrue(weakPtr1.IsExpired());
      TestAssert::IsNull(weakPtr1.GetStrongPtr().Get());

      TestAssert::IsFalse(weakPtr2.IsExpired());
      TestAssert::IsNotNull(weakPtr2.GetStrongPtr().Get());
      TestAssert::IsTrue(ptr.Get() == weakPtr2.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(WeakPtr_MoveConstructorInterface) {
    bool deleted = false;
    {
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      Mso::WeakPtr<WeakPtrSample1> weakPtr1(ptr);
      Mso::WeakPtr<IWeakPtrSample1> weakPtr2(std::move(weakPtr1));

      TestAssert::IsTrue(weakPtr1.IsExpired());
      TestAssert::IsNull(weakPtr1.GetStrongPtr().Get());

      TestAssert::IsFalse(weakPtr2.IsExpired());
      TestAssert::IsNotNull(weakPtr2.GetStrongPtr().Get());
      TestAssert::IsTrue(ptr.Get() == weakPtr2.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(WeakPtr_MoveConstructorInterface2) {
    bool deleted = false;
    {
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      Mso::WeakPtr<WeakPtrSample1> weakPtr1(ptr);
      Mso::WeakPtr<IWeakPtrSample2> weakPtr2(std::move(weakPtr1));

      TestAssert::IsTrue(weakPtr1.IsExpired());
      TestAssert::IsNull(weakPtr1.GetStrongPtr().Get());

      TestAssert::IsFalse(weakPtr2.IsExpired());
      TestAssert::IsNotNull(weakPtr2.GetStrongPtr().Get());
      TestAssert::IsTrue(static_cast<IWeakPtrSample2 *>(ptr.Get()) == weakPtr2.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(WeakPtr_NullAssignment) {
    bool deleted = false;
    {
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      Mso::WeakPtr<WeakPtrSample1> weakPtr1(ptr);

      weakPtr1 = nullptr;
      TestAssert::IsTrue(weakPtr1.IsExpired());
      TestAssert::IsNull(weakPtr1.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
  }

  // Pointer to a WeakPtrSample1 assigned to Mso::WeakPtr<WeakPtrSample1>
  TEST_METHOD(WeakPtr_PointerAssignment1) {
    bool deleted = false;
    {
      Mso::WeakPtr<WeakPtrSample1> weakPtr;
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      weakPtr = ptr.Get();

      TestAssert::IsFalse(weakPtr.IsExpired());
      TestAssert::IsNotNull(weakPtr.GetStrongPtr().Get());
      TestAssert::IsTrue(ptr.Get() == weakPtr.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
  }

  // Pointer to a WeakPtrSample1 assigned to Mso::WeakPtr<IWeakPtrSample1>
  TEST_METHOD(WeakPtr_PointerAssignment21) {
    bool deleted = false;
    {
      Mso::WeakPtr<IWeakPtrSample1> weakPtr;
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      weakPtr = ptr.Get();

      TestAssert::IsFalse(weakPtr.IsExpired());
      TestAssert::IsNotNull(weakPtr.GetStrongPtr().Get());
      TestAssert::IsTrue(ptr.Get() == weakPtr.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
  }

  // Pointer to a WeakPtrSample1 assigned to Mso::WeakPtr<IWeakPtrSample2>
  TEST_METHOD(WeakPtr_PointerAssignment22) {
    bool deleted = false;
    {
      Mso::WeakPtr<IWeakPtrSample2> weakPtr;
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      weakPtr = ptr.Get();

      TestAssert::IsFalse(weakPtr.IsExpired());
      TestAssert::IsNotNull(weakPtr.GetStrongPtr().Get());
      TestAssert::IsTrue(static_cast<IWeakPtrSample2 *>(ptr.Get()) == weakPtr.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
  }

  // Null pointer to a WeakPtrSample1 assigned to Mso::WeakPtr<WeakPtrSample1>
  TEST_METHOD(WeakPtr_NullPointerAssignment) {
    bool deleted = false;
    {
      Mso::WeakPtr<IWeakPtrSample1> weakPtr;
      WeakPtrSample1 *ptrNull = nullptr;
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      weakPtr = ptrNull;

      TestAssert::IsTrue(weakPtr.IsExpired());
      TestAssert::IsNull(weakPtr.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
  }

  // CntPtr with a WeakPtrSample1 assigned to Mso::WeakPtr<WeakPtrSample1>
  TEST_METHOD(WeakPtr_CntPtrAssignment1) {
    bool deleted = false;
    {
      Mso::WeakPtr<WeakPtrSample1> weakPtr;
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      weakPtr = ptr;

      TestAssert::IsFalse(weakPtr.IsExpired());
      TestAssert::IsNotNull(weakPtr.GetStrongPtr().Get());
      TestAssert::IsTrue(ptr.Get() == weakPtr.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
  }

  // CntPtr with a WeakPtrSample1 assigned to Mso::WeakPtr<IWeakPtrSample1>
  TEST_METHOD(WeakPtr_CntPtrAssignment21) {
    bool deleted = false;
    {
      Mso::WeakPtr<IWeakPtrSample1> weakPtr;
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      weakPtr = ptr;

      TestAssert::IsFalse(weakPtr.IsExpired());
      TestAssert::IsNotNull(weakPtr.GetStrongPtr().Get());
      TestAssert::IsTrue(ptr.Get() == weakPtr.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
  }

  // CntPtr with a WeakPtrSample1 assigned to Mso::WeakPtr<IWeakPtrSample2>
  TEST_METHOD(WeakPtr_CntPtrAssignment22) {
    bool deleted = false;
    {
      Mso::WeakPtr<IWeakPtrSample2> weakPtr;
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      weakPtr = ptr;

      TestAssert::IsFalse(weakPtr.IsExpired());
      TestAssert::IsNotNull(weakPtr.GetStrongPtr().Get());
      TestAssert::IsTrue(static_cast<IWeakPtrSample2 *>(ptr.Get()) == weakPtr.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
  }

  // CntPtr with a Null pointer to a WeakPtrSample1 assigned to Mso::WeakPtr<WeakPtrSample1>
  TEST_METHOD(WeakPtr_NullCntPtrAssignment) {
    bool deleted = false;
    {
      Mso::WeakPtr<IWeakPtrSample1> weakPtr;
      Mso::CntPtr<WeakPtrSample1> ptrNull{nullptr};
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      weakPtr = ptrNull;

      TestAssert::IsTrue(weakPtr.IsExpired());
      TestAssert::IsNull(weakPtr.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(WeakPtr_CopyAssignment) {
    Mso::WeakPtr<WeakPtrSample1> weakPtr2;
    bool deleted = false;
    {
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      Mso::WeakPtr<WeakPtrSample1> weakPtr1(ptr);
      weakPtr2 = weakPtr1;

      TestAssert::IsFalse(weakPtr1.IsExpired());
      TestAssert::IsNotNull(weakPtr1.GetStrongPtr().Get());
      TestAssert::IsTrue(ptr.Get() == weakPtr1.GetStrongPtr().Get());

      TestAssert::IsFalse(weakPtr2.IsExpired());
      TestAssert::IsNotNull(weakPtr2.GetStrongPtr().Get());
      TestAssert::IsTrue(ptr.Get() == weakPtr2.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
    TestAssert::IsTrue(weakPtr2.IsExpired());
    TestAssert::IsNull(weakPtr2.GetStrongPtr().Get());
  }

  TEST_METHOD(WeakPtr_CopyAssignmentInterface) {
    Mso::WeakPtr<IWeakPtrSample1> weakPtr2;
    bool deleted = false;
    {
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      Mso::WeakPtr<WeakPtrSample1> weakPtr1(ptr);
      weakPtr2 = weakPtr1;

      TestAssert::IsFalse(weakPtr1.IsExpired());
      TestAssert::IsNotNull(weakPtr1.GetStrongPtr().Get());
      TestAssert::IsTrue(ptr.Get() == weakPtr1.GetStrongPtr().Get());

      TestAssert::IsFalse(weakPtr2.IsExpired());
      TestAssert::IsNotNull(weakPtr2.GetStrongPtr().Get());
      TestAssert::IsTrue(ptr.Get() == weakPtr2.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
    TestAssert::IsTrue(weakPtr2.IsExpired());
    TestAssert::IsNull(weakPtr2.GetStrongPtr().Get());
  }

  TEST_METHOD(WeakPtr_CopyAssignmentInterface2) {
    Mso::WeakPtr<IWeakPtrSample2> weakPtr2;
    bool deleted = false;
    {
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      Mso::WeakPtr<WeakPtrSample1> weakPtr1(ptr);
      weakPtr2 = weakPtr1;

      TestAssert::IsFalse(weakPtr1.IsExpired());
      TestAssert::IsNotNull(weakPtr1.GetStrongPtr().Get());
      TestAssert::IsTrue(ptr.Get() == weakPtr1.GetStrongPtr().Get());

      TestAssert::IsFalse(weakPtr2.IsExpired());
      TestAssert::IsNotNull(weakPtr2.GetStrongPtr().Get());
      TestAssert::IsTrue(static_cast<IWeakPtrSample2 *>(ptr.Get()) == weakPtr2.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
    TestAssert::IsTrue(weakPtr2.IsExpired());
    TestAssert::IsNull(weakPtr2.GetStrongPtr().Get());
  }

  TEST_METHOD(WeakPtr_CopyAssignmentSamePtr) {
    bool deleted = false;
    {
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      Mso::WeakPtr<WeakPtrSample1> weakPtr1(ptr);

      OACR_WARNING_SUPPRESS(IDENTITY_ASSIGNMENT, "We want to test our code that nothing bad happens in this case");
      weakPtr1 = weakPtr1;

      TestAssert::IsFalse(weakPtr1.IsExpired());
      TestAssert::IsNotNull(weakPtr1.GetStrongPtr().Get());
      TestAssert::IsTrue(ptr.Get() == weakPtr1.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(WeakPtr_MoveAssignment) {
    Mso::WeakPtr<WeakPtrSample1> weakPtr2;
    bool deleted = false;
    {
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      Mso::WeakPtr<WeakPtrSample1> weakPtr1(ptr);
      weakPtr2 = std::move(weakPtr1);

      TestAssert::IsTrue(weakPtr1.IsExpired());
      TestAssert::IsNull(weakPtr1.GetStrongPtr().Get());

      TestAssert::IsFalse(weakPtr2.IsExpired());
      TestAssert::IsNotNull(weakPtr2.GetStrongPtr().Get());
      TestAssert::IsTrue(ptr.Get() == weakPtr2.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
    TestAssert::IsTrue(weakPtr2.IsExpired());
    TestAssert::IsNull(weakPtr2.GetStrongPtr().Get());
  }

  TEST_METHOD(WeakPtr_MoveAssignmentInterface) {
    Mso::WeakPtr<IWeakPtrSample1> weakPtr2;
    bool deleted = false;
    {
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      Mso::WeakPtr<WeakPtrSample1> weakPtr1(ptr);
      weakPtr2 = std::move(weakPtr1);

      TestAssert::IsTrue(weakPtr1.IsExpired());
      TestAssert::IsNull(weakPtr1.GetStrongPtr().Get());

      TestAssert::IsFalse(weakPtr2.IsExpired());
      TestAssert::IsNotNull(weakPtr2.GetStrongPtr().Get());
      TestAssert::IsTrue(ptr.Get() == weakPtr2.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
    TestAssert::IsTrue(weakPtr2.IsExpired());
    TestAssert::IsNull(weakPtr2.GetStrongPtr().Get());
  }

  TEST_METHOD(WeakPtr_MoveAssignmentInterface2) {
    Mso::WeakPtr<IWeakPtrSample2> weakPtr2;
    bool deleted = false;
    {
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      Mso::WeakPtr<WeakPtrSample1> weakPtr1(ptr);
      weakPtr2 = std::move(weakPtr1);

      TestAssert::IsTrue(weakPtr1.IsExpired());
      TestAssert::IsNull(weakPtr1.GetStrongPtr().Get());

      TestAssert::IsFalse(weakPtr2.IsExpired());
      TestAssert::IsNotNull(weakPtr2.GetStrongPtr().Get());
      TestAssert::IsTrue(static_cast<IWeakPtrSample2 *>(ptr.Get()) == weakPtr2.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
    TestAssert::IsTrue(weakPtr2.IsExpired());
    TestAssert::IsNull(weakPtr2.GetStrongPtr().Get());
  }

  TEST_METHOD(WeakPtr_MoveAssignmentSamePtr) {
    bool deleted = false;
    {
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      Mso::WeakPtr<WeakPtrSample1> weakPtr1(ptr);

      weakPtr1 = std::move(weakPtr1);

      TestAssert::IsFalse(weakPtr1.IsExpired());
      TestAssert::IsNotNull(weakPtr1.GetStrongPtr().Get());
      TestAssert::IsTrue(ptr.Get() == weakPtr1.GetStrongPtr().Get());
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(WeakPtr_PassToLambda) {
    bool deleted = false;
    {
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      auto func = ptr->PassThisToLambda();
      func();
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(WeakPtr_IsEmptyOnDefaultConstructedObject) {
    Mso::WeakPtr<IWeakPtrSample1> weakPtr;
    TestAssert::IsTrue(weakPtr.IsEmpty());
  }

  TEST_METHOD(WeakPtr_IsEmptyOnObjectConstructedWithNullptrArg) {
    Mso::WeakPtr<IWeakPtrSample1> weakPtr = nullptr;
    TestAssert::IsTrue(weakPtr.IsEmpty());
  }

  TEST_METHOD(WeakPtr_IsEmptyOnObjectConstructedWithNonNullArg) {
    bool deleted = false;
    {
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/* ref */ deleted);
      Mso::WeakPtr<WeakPtrSample1> weakPtr = ptr;
      TestAssert::IsFalse(weakPtr.IsEmpty());
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(WeakPtr_IsEmptyOnExpiredObject) {
    bool deleted = false;
    {
      Mso::CntPtr<WeakPtrSample1> ptr = Mso::Make<WeakPtrSample1>(/* ref */ deleted);
      Mso::WeakPtr<WeakPtrSample1> weakPtr = ptr;
      TestAssert::IsFalse(weakPtr.IsEmpty());
      TestAssert::IsFalse(weakPtr.IsExpired());

      // cause the object to expire
      ptr.Clear();

      TestAssert::IsFalse(weakPtr.IsEmpty());
      TestAssert::IsTrue(weakPtr.IsExpired());
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(WeakPtr_EqualityOperator) {
    bool deleted = false;
    {
      Mso::CntPtr<WeakPtrSample1> ptr1 = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      Mso::WeakPtr<WeakPtrSample1> weakPtr1(ptr1);

      Mso::WeakPtr<WeakPtrSample1> weakPtr2 = weakPtr1;

      Mso::WeakPtr<WeakPtrSample1> weakPtr3 = nullptr;

      TestAssert::IsTrue(weakPtr1 == weakPtr2);
      TestAssert::IsTrue(weakPtr3 == nullptr);
      TestAssert::IsTrue(nullptr == weakPtr3);
      TestAssert::IsTrue(weakPtr1 != weakPtr3);
      TestAssert::IsTrue(weakPtr2 != nullptr);
      TestAssert::IsTrue(nullptr != weakPtr2);
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(WeakPtr_CanConvertFromCntPtrToIWeakRefCounted) {
    bool deleted = false;
    {
      // convert CntPtr to IWeakRefCounted-derived interface to WeakPtr to that interface
      Mso::CntPtr<IWeakPtrSample3> ptr1 = Mso::Make<WeakPtrSample3>(/*ref*/ deleted);
      Mso::WeakPtr<IWeakPtrSample3> weakPtr1(ptr1);
      TestAssert::IsFalse(weakPtr1.IsEmpty());
      TestAssert::IsFalse(weakPtr1.IsExpired());

      // transfer the strong reference from ptr1 to ptr2 using weakPtr1
      Mso::CntPtr<IWeakPtrSample3> ptr2 = weakPtr1.GetStrongPtr();
      ptr1 = nullptr;
      TestAssert::IsFalse(weakPtr1.IsEmpty());
      TestAssert::IsFalse(weakPtr1.IsExpired());

      // nullify the strong reference: weak ptr expires but remains non-empty
      ptr2 = nullptr;
      TestAssert::IsFalse(weakPtr1.IsEmpty());
      TestAssert::IsTrue(weakPtr1.IsExpired());
    }
    TestAssert::IsTrue(deleted);
  }

  TESTMETHOD_REQUIRES_SEH(WeakPtr_CannotConvertFromCntPtrToIRefCounted) {
    bool deleted = false;
    {
      Mso::CntPtr<IWeakPtrSample1> ptr1 = Mso::Make<WeakPtrSample1>(/*ref*/ deleted);
      Mso::WeakPtr<IWeakPtrSample1> weakPtr1;
      TestAssert::ExpectVEC([&]() noexcept {
        weakPtr1 = ptr1; // tag_bad22 is expected here
      });
    }
    TestAssert::IsTrue(deleted);
  }
};
