// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "errorCode/maybe.h"
#include "errorCode/errorProvider.h"
#include "errorCode/exceptionErrorProvider.h"
#include "errorCode/hresultErrorProvider.h"
#include "motifCpp/testCheck.h"

MSO_STRUCT_GUID(MyDataGuid, "ff59ec8c-f3e8-4ab6-975c-0b725aa92ccf")

struct MyData {
  MyData() = default;
  MyData(const MyData &) = default;
  MyData(MyData &&other) noexcept : Val1(other.Val1), Val2(other.Val2) {
    other.Val1 = 0;
    other.Val2 = 0;
  }

  MyData(int32_t v1, int32_t v2) noexcept : Val1(v1), Val2(v2) {}

  int32_t Val1{1};
  int32_t Val2{2};
};

Mso::ErrorProvider<MyData, MyDataGuid> s_myErrorProvider{};

const Mso::ErrorProvider<MyData, MyDataGuid> &MyErrorProvider() noexcept {
  return s_myErrorProvider;
}

namespace Mso {

// Override OnUnhandledError to avoid ShipAssert
template <>
void ErrorProvider<MyData, MyDataGuid>::OnUnhandledError(const ErrorCodeState & /*state*/) const noexcept {
  UNREFERENCED_OACR(this);
  // Do nothing
}

} // namespace Mso

TEST_CLASS (MaybeTest) {
  TEST_METHOD(Maybe_ctor_Default) {
    Mso::Maybe<MyData> m1;
    TestCheck(m1.IsValue());
    TestCheckEqual(1, m1.GetValue().Val1);
    TestCheckEqual(2, m1.GetValue().Val2);
  }

  TEST_METHOD(Maybe_ctor_ValueCopy) {
    MyData d1{3, 4};
    Mso::Maybe<MyData> m1{d1};
    TestCheck(m1.IsValue());
    TestCheckEqual(3, m1.GetValue().Val1);
    TestCheckEqual(4, m1.GetValue().Val2);
    TestCheckEqual(3, d1.Val1);
    TestCheckEqual(4, d1.Val2);
  }

  TEST_METHOD(Maybe_ctor_ValueMove) {
    MyData d1{3, 4};
    Mso::Maybe<MyData> m1{std::move(d1)};
    TestCheck(m1.IsValue());
    TestCheckEqual(3, m1.GetValue().Val1);
    TestCheckEqual(4, m1.GetValue().Val2);
    TestCheckEqual(0, d1.Val1);
    TestCheckEqual(0, d1.Val2);
  }

  template <class T>
  static Mso::Maybe<T> CreateImplicitly(Mso::Maybe<T> && m) noexcept {
    Mso::Maybe<T> result = std::move(m);
    return result;
  }

  TEST_METHOD(Maybe_ctor_ValueCopy_implicit) {
    MyData d1{3, 4};
    Mso::Maybe<MyData> m1 = CreateImplicitly<MyData>(d1);
    TestCheck(m1.IsValue());
    TestCheckEqual(3, m1.GetValue().Val1);
    TestCheckEqual(4, m1.GetValue().Val2);
    TestCheckEqual(3, d1.Val1);
    TestCheckEqual(4, d1.Val2);
  }

  TEST_METHOD(Maybe_ctor_ValueMove_implicit) {
    MyData d1{3, 4};
    Mso::Maybe<MyData> m1 = CreateImplicitly<MyData>(std::move(d1));
    TestCheck(m1.IsValue());
    TestCheckEqual(3, m1.GetValue().Val1);
    TestCheckEqual(4, m1.GetValue().Val2);
    TestCheckEqual(0, d1.Val1);
    TestCheckEqual(0, d1.Val2);
  }

  TEST_METHOD(Maybe_ctor_ValueInPlace) {
    Mso::Maybe<MyData> m1{Mso::InPlaceTag(), 3, 4};
    TestCheck(m1.IsValue());
    TestCheckEqual(3, m1.GetValue().Val1);
    TestCheckEqual(4, m1.GetValue().Val2);
  }

  TEST_METHOD(Maybe_ctor_ValueInPlaceInitList) {
    Mso::Maybe<std::vector<int>> m1{Mso::InPlaceTag(), {1, 2, 3, 4}};
    TestCheck(m1.IsValue());
    TestCheckEqual(4u, m1.GetValue().size());
    TestCheckEqual(3, m1.GetValue()[2]);
  }

  TEST_METHOD(Maybe_ctor_ErrorCopy) {
    Mso::ErrorCode error = MyErrorProvider().MakeErrorCode(MyData(2, 3));
    Mso::Maybe<int> m1(error);
    TestCheck(m1.IsError());

    TestCheckEqual(2, m1.GetError().As(MyErrorProvider())->Val1);
    TestCheckEqual(3, m1.GetError().As(MyErrorProvider())->Val2);

    TestCheckEqual(2, error.As(MyErrorProvider())->Val1);
    TestCheckEqual(3, error.As(MyErrorProvider())->Val2);
  }

  TEST_METHOD(Maybe_ctor_ErrorMove) {
    Mso::ErrorCode error = MyErrorProvider().MakeErrorCode(MyData(2, 3));
    Mso::Maybe<int> m1(std::move(error));
    TestCheck(m1.IsError());

    TestCheckEqual(2, m1.GetError().As(MyErrorProvider())->Val1);
    TestCheckEqual(3, m1.GetError().As(MyErrorProvider())->Val2);

    TestCheck(!error.As(MyErrorProvider()));
  }

  TEST_METHOD(Maybe_ctor_ErrorCopy_implicit) {
    Mso::ErrorCode error = MyErrorProvider().MakeErrorCode(MyData(2, 3));
    Mso::Maybe<int> m1 = CreateImplicitly<int>(error);
    TestCheck(m1.IsError());

    TestCheckEqual(2, m1.GetError().As(MyErrorProvider())->Val1);
    TestCheckEqual(3, m1.GetError().As(MyErrorProvider())->Val2);

    TestCheckEqual(2, error.As(MyErrorProvider())->Val1);
    TestCheckEqual(3, error.As(MyErrorProvider())->Val2);
  }

  TEST_METHOD(Maybe_ctor_ErrorMove_implicit) {
    Mso::ErrorCode error = MyErrorProvider().MakeErrorCode(MyData(2, 3));
    Mso::Maybe<int> m1 = CreateImplicitly<int>(std::move(error));
    TestCheck(m1.IsError());

    TestCheckEqual(2, m1.GetError().As(MyErrorProvider())->Val1);
    TestCheckEqual(3, m1.GetError().As(MyErrorProvider())->Val2);

    TestCheck(!error.As(MyErrorProvider()));
  }

  TEST_METHOD(Maybe_ctor_CopyValue) {
    Mso::Maybe<MyData> m1{Mso::InPlaceTag(), 3, 4};
    Mso::Maybe<MyData> m2(m1);

    TestCheck(m1.IsValue());
    TestCheckEqual(3, m1.GetValue().Val1);
    TestCheckEqual(4, m1.GetValue().Val2);

    TestCheck(m2.IsValue());
    TestCheckEqual(3, m2.GetValue().Val1);
    TestCheckEqual(4, m2.GetValue().Val2);
  }

  TEST_METHOD(Maybe_ctor_CopyError) {
    Mso::Maybe<int> m1 = MyErrorProvider().MakeErrorCode(MyData(2, 3));
    Mso::Maybe<int> m2(m1);

    TestCheck(m1.IsError());
    TestCheckEqual(2, m1.GetError().As(MyErrorProvider())->Val1);
    TestCheckEqual(3, m1.GetError().As(MyErrorProvider())->Val2);

    TestCheck(m2.IsError());
    TestCheckEqual(2, m2.GetError().As(MyErrorProvider())->Val1);
    TestCheckEqual(3, m2.GetError().As(MyErrorProvider())->Val2);
  }

  TEST_METHOD(Maybe_ctor_MoveValue) {
    Mso::Maybe<MyData> m1{Mso::InPlaceTag(), 3, 4};
    Mso::Maybe<MyData> m2(std::move(m1));

    TestCheck(m1.IsValue());
    TestCheckEqual(0, m1.GetValue().Val1);
    TestCheckEqual(0, m1.GetValue().Val2);

    TestCheck(m2.IsValue());
    TestCheckEqual(3, m2.GetValue().Val1);
    TestCheckEqual(4, m2.GetValue().Val2);
  }

  TEST_METHOD(Maybe_ctor_MoveError) {
    Mso::Maybe<int> m1 = MyErrorProvider().MakeErrorCode(MyData(2, 3));
    Mso::Maybe<int> m2(std::move(m1));

    TestCheck(m1.IsError());
    TestCheck(!m1.GetError().As(MyErrorProvider()));

    TestCheck(m2.IsError());
    TestCheckEqual(2, m2.GetError().As(MyErrorProvider())->Val1);
    TestCheckEqual(3, m2.GetError().As(MyErrorProvider())->Val2);
  }

  TEST_METHOD(Maybe_Assign_CopyValue) {
    Mso::Maybe<MyData> m1{Mso::InPlaceTag(), 1, 2};
    Mso::Maybe<MyData> m2{Mso::InPlaceTag(), 3, 4};

    m1 = m2;

    TestCheck(m1.IsValue());
    TestCheckEqual(3, m1.GetValue().Val1);
    TestCheckEqual(4, m1.GetValue().Val2);

    TestCheck(m2.IsValue());
    TestCheckEqual(3, m2.GetValue().Val1);
    TestCheckEqual(4, m2.GetValue().Val2);
  }

  TEST_METHOD(Maybe_Assign_CopyError) {
    Mso::Maybe<int> m1(MyErrorProvider().MakeErrorCode(MyData(1, 4)));
    Mso::Maybe<int> m2(MyErrorProvider().MakeErrorCode(MyData(2, 3)));

    m1 = m2;

    TestCheck(m1.IsError());
    TestCheckEqual(2, m1.GetError().As(MyErrorProvider())->Val1);
    TestCheckEqual(3, m1.GetError().As(MyErrorProvider())->Val2);

    TestCheck(m2.IsError());
    TestCheckEqual(2, m2.GetError().As(MyErrorProvider())->Val1);
    TestCheckEqual(3, m2.GetError().As(MyErrorProvider())->Val2);
  }

  TEST_METHOD(Maybe_Assign_MoveValue) {
    Mso::Maybe<MyData> m1{Mso::InPlaceTag(), 1, 2};
    Mso::Maybe<MyData> m2{Mso::InPlaceTag(), 3, 4};

    m1 = std::move(m2);

    TestCheck(m1.IsValue());
    TestCheckEqual(3, m1.GetValue().Val1);
    TestCheckEqual(4, m1.GetValue().Val2);

    TestCheck(m2.IsValue());
    TestCheckEqual(0, m2.GetValue().Val1);
    TestCheckEqual(0, m2.GetValue().Val2);
  }

  TEST_METHOD(Maybe_Assign_MoveError) {
    Mso::Maybe<int> m1(MyErrorProvider().MakeErrorCode(MyData(1, 4)));
    Mso::Maybe<int> m2(MyErrorProvider().MakeErrorCode(MyData(2, 3)));

    m1 = std::move(m2);

    TestCheck(m1.IsError());
    TestCheckEqual(2, m1.GetError().As(MyErrorProvider())->Val1);
    TestCheckEqual(3, m1.GetError().As(MyErrorProvider())->Val2);

    TestCheck(m2.IsError());
    TestCheck(!m2.GetError());
  }

  TEST_METHOD(Maybe_IsValue) {
    Mso::Maybe<MyData> m1{Mso::InPlaceTag(), 1, 2};
    Mso::Maybe<int> m2(MyErrorProvider().MakeErrorCode(MyData(2, 3)));

    TestCheck(m1.IsValue());
    TestCheck(!m2.IsValue());
  }

  TEST_METHOD(Maybe_IsError) {
    Mso::Maybe<MyData> m1{Mso::InPlaceTag(), 1, 2};
    Mso::Maybe<int> m2(MyErrorProvider().MakeErrorCode(MyData(2, 3)));

    TestCheck(!m1.IsError());
    TestCheck(m2.IsError());
  }

  TEST_METHOD(Maybe_GetValue) {
    Mso::Maybe<MyData> m1{Mso::InPlaceTag(), 1, 2};

    const MyData &d1 = m1.GetValue();

    TestCheck(m1.IsValue());
    TestCheckEqual(1, m1.GetValue().Val1);
    TestCheckEqual(2, m1.GetValue().Val2);

    TestCheckEqual(1, d1.Val1);
    TestCheckEqual(2, d1.Val2);
  }

  TESTMETHOD_REQUIRES_SEH(Maybe_GetValueCrash) {
    Mso::Maybe<int> m1(MyErrorProvider().MakeErrorCode(MyData(1, 4)));

    TestCheckCrash(m1.GetValue());
  }

  TEST_METHOD(Maybe_GetValueElseThrow) {
    Mso::Maybe<MyData> m1{Mso::InPlaceTag(), 1, 2};

    const MyData &d1 = m1.GetValueElseThrow();

    TestCheck(m1.IsValue());
    TestCheckEqual(1, m1.GetValue().Val1);
    TestCheckEqual(2, m1.GetValue().Val2);

    TestCheckEqual(1, d1.Val1);
    TestCheckEqual(2, d1.Val2);
  }

  TEST_METHOD(Maybe_GetValueElseThrow_Error) {
    Mso::Maybe<int> m1(MyErrorProvider().MakeErrorCode(MyData(1, 4)));

    TestCheckException(Mso::ErrorCodeException, m1.GetValueElseThrow());
  }

  TEST_METHOD(Maybe_TakeValue) {
    Mso::Maybe<MyData> m1{Mso::InPlaceTag(), 1, 2};

    MyData d1 = m1.TakeValue();

    TestCheck(m1.IsValue());
    TestCheckEqual(0, m1.GetValue().Val1);
    TestCheckEqual(0, m1.GetValue().Val2);

    TestCheckEqual(1, d1.Val1);
    TestCheckEqual(2, d1.Val2);
  }

  TESTMETHOD_REQUIRES_SEH(Maybe_TakeValueCrash) {
    Mso::Maybe<int> m1(MyErrorProvider().MakeErrorCode(MyData(1, 4)));

    TestCheckCrash(m1.TakeValue());
  }

  TEST_METHOD(Maybe_TakeValueElseThrow) {
    Mso::Maybe<MyData> m1{Mso::InPlaceTag(), 1, 2};

    MyData d1 = m1.TakeValueElseThrow();

    TestCheck(m1.IsValue());
    TestCheckEqual(0, m1.GetValue().Val1);
    TestCheckEqual(0, m1.GetValue().Val2);

    TestCheckEqual(1, d1.Val1);
    TestCheckEqual(2, d1.Val2);
  }

  TEST_METHOD(Maybe_TakeValueElseThrow_Error) {
    Mso::Maybe<int> m1(MyErrorProvider().MakeErrorCode(MyData(1, 4)));

    TestCheckException(Mso::ErrorCodeException, m1.TakeValueElseThrow());
  }

  TEST_METHOD(Maybe_GetError) {
    Mso::Maybe<int> m1(MyErrorProvider().MakeErrorCode(MyData(2, 3)));

    const Mso::ErrorCode &e1 = m1.GetError();

    TestCheck(m1.IsError());
    TestCheckEqual(2, m1.GetError().As(MyErrorProvider())->Val1);
    TestCheckEqual(3, m1.GetError().As(MyErrorProvider())->Val2);

    TestCheckEqual(2, e1.As(MyErrorProvider())->Val1);
    TestCheckEqual(3, e1.As(MyErrorProvider())->Val2);
  }

  TESTMETHOD_REQUIRES_SEH(Maybe_GetErrorCrash) {
    Mso::Maybe<MyData> m1{Mso::InPlaceTag(), 1, 2};

    TestCheckCrash(m1.GetError());
  }

  TEST_METHOD(Maybe_TakeError) {
    Mso::Maybe<int> m1(MyErrorProvider().MakeErrorCode(MyData(2, 3)));

    Mso::ErrorCode e1 = m1.TakeError();

    TestCheck(m1.IsError());
    TestCheck(!m1.GetError());

    TestCheckEqual(2, e1.As(MyErrorProvider())->Val1);
    TestCheckEqual(3, e1.As(MyErrorProvider())->Val2);
  }

  TESTMETHOD_REQUIRES_SEH(Maybe_GetTakeCrash) {
    Mso::Maybe<MyData> m1{Mso::InPlaceTag(), 1, 2};

    TestCheckCrash(m1.TakeError());
  }

  TEST_METHOD(MaybeVoid_ctor_Default) {
    Mso::Maybe<void> m1;
    TestCheck(m1.IsValue());
  }

  TEST_METHOD(MaybeVoid_ctor_ErrorCopy) {
    Mso::ErrorCode error = MyErrorProvider().MakeErrorCode(MyData(2, 3));
    Mso::Maybe<void> m1(error);
    TestCheck(m1.IsError());

    TestCheckEqual(2, m1.GetError().As(MyErrorProvider())->Val1);
    TestCheckEqual(3, m1.GetError().As(MyErrorProvider())->Val2);

    TestCheckEqual(2, error.As(MyErrorProvider())->Val1);
    TestCheckEqual(3, error.As(MyErrorProvider())->Val2);
  }

  TEST_METHOD(MaybeVoid_ctor_ErrorMove) {
    Mso::ErrorCode error = MyErrorProvider().MakeErrorCode(MyData(2, 3));
    Mso::Maybe<void> m1(std::move(error));
    TestCheck(m1.IsError());

    TestCheckEqual(2, m1.GetError().As(MyErrorProvider())->Val1);
    TestCheckEqual(3, m1.GetError().As(MyErrorProvider())->Val2);

    TestCheck(!error);
  }

  TEST_METHOD(MaybeVoid_ctor_ErrorCopy_implicit) {
    Mso::ErrorCode error = MyErrorProvider().MakeErrorCode(MyData(2, 3));
    Mso::Maybe<void> m1 = CreateImplicitly<void>(error);
    TestCheck(m1.IsError());

    TestCheckEqual(2, m1.GetError().As(MyErrorProvider())->Val1);
    TestCheckEqual(3, m1.GetError().As(MyErrorProvider())->Val2);

    TestCheckEqual(2, error.As(MyErrorProvider())->Val1);
    TestCheckEqual(3, error.As(MyErrorProvider())->Val2);
  }

  TEST_METHOD(MaybeVoid_ctor_ErrorMove_implicit) {
    Mso::ErrorCode error = MyErrorProvider().MakeErrorCode(MyData(2, 3));
    Mso::Maybe<void> m1 = CreateImplicitly<void>(std::move(error));
    TestCheck(m1.IsError());

    TestCheckEqual(2, m1.GetError().As(MyErrorProvider())->Val1);
    TestCheckEqual(3, m1.GetError().As(MyErrorProvider())->Val2);

    TestCheck(!error);
  }

  TEST_METHOD(MaybeVoid_ctor_CopyValue) {
    Mso::Maybe<void> m1;
    Mso::Maybe<void> m2(m1);

    TestCheck(m1.IsValue());
    TestCheck(m2.IsValue());
  }

  TEST_METHOD(MaybeVoid_ctor_CopyError) {
    Mso::Maybe<void> m1(MyErrorProvider().MakeErrorCode(MyData(2, 3)));
    Mso::Maybe<void> m2(m1);

    TestCheck(m1.IsError());
    TestCheckEqual(2, m1.GetError().As(MyErrorProvider())->Val1);
    TestCheckEqual(3, m1.GetError().As(MyErrorProvider())->Val2);

    TestCheck(m2.IsError());
    TestCheckEqual(2, m2.GetError().As(MyErrorProvider())->Val1);
    TestCheckEqual(3, m2.GetError().As(MyErrorProvider())->Val2);
  }

  TEST_METHOD(MaybeVoid_ctor_MoveValue) {
    Mso::Maybe<void> m1;
    Mso::Maybe<void> m2(std::move(m1));

    TestCheck(m1.IsValue());
    TestCheck(m2.IsValue());
  }

  TEST_METHOD(MaybeVoid_ctor_MoveError) {
    Mso::Maybe<void> m1(MyErrorProvider().MakeErrorCode(MyData(2, 3)));
    Mso::Maybe<void> m2(std::move(m1));

    TestCheck(m1.IsError());
    TestCheck(!m1.GetError());

    TestCheck(m2.IsError());
    TestCheckEqual(2, m2.GetError().As(MyErrorProvider())->Val1);
    TestCheckEqual(3, m2.GetError().As(MyErrorProvider())->Val2);
  }

  TEST_METHOD(MaybeVoid_Assign_CopyValue) {
    Mso::Maybe<void> m1(MyErrorProvider().MakeErrorCode(MyData(2, 3)));
    Mso::Maybe<void> m2;

    m1 = m2;

    TestCheck(m1.IsValue());
    TestCheck(m2.IsValue());
  }

  TEST_METHOD(MaybeVoid_Assign_CopyError) {
    Mso::Maybe<void> m1;
    Mso::Maybe<void> m2(MyErrorProvider().MakeErrorCode(MyData(2, 3)));

    m1 = m2;

    TestCheck(m1.IsError());
    TestCheckEqual(2, m1.GetError().As(MyErrorProvider())->Val1);
    TestCheckEqual(3, m1.GetError().As(MyErrorProvider())->Val2);

    TestCheck(m2.IsError());
    TestCheckEqual(2, m2.GetError().As(MyErrorProvider())->Val1);
    TestCheckEqual(3, m2.GetError().As(MyErrorProvider())->Val2);
  }

  TEST_METHOD(MaybeVoid_Assign_MoveValue) {
    Mso::Maybe<void> m1(MyErrorProvider().MakeErrorCode(MyData(2, 3)));
    Mso::Maybe<void> m2;

    m1 = std::move(m2);

    TestCheck(m1.IsValue());
    TestCheck(m2.IsValue());
  }

  TEST_METHOD(MaybeVoid_Assign_MoveError) {
    Mso::Maybe<void> m1;
    Mso::Maybe<void> m2(MyErrorProvider().MakeErrorCode(MyData(2, 3)));

    m1 = std::move(m2);

    TestCheck(m1.IsError());
    TestCheckEqual(2, m1.GetError().As(MyErrorProvider())->Val1);
    TestCheckEqual(3, m1.GetError().As(MyErrorProvider())->Val2);

    TestCheck(m2.IsError());
    TestCheck(!m2.GetError());
  }

  TEST_METHOD(MaybeVoid_IsValue) {
    Mso::Maybe<void> m1;
    Mso::Maybe<void> m2(MyErrorProvider().MakeErrorCode(MyData(2, 3)));

    TestCheck(m1.IsValue());
    TestCheck(!m2.IsValue());
  }

  TEST_METHOD(MaybeVoid_IsError) {
    Mso::Maybe<void> m1;
    Mso::Maybe<void> m2(MyErrorProvider().MakeErrorCode(MyData(2, 3)));

    TestCheck(!m1.IsError());
    TestCheck(m2.IsError());
  }

  TEST_METHOD(MaybeVoid_GetError) {
    Mso::Maybe<void> m1(MyErrorProvider().MakeErrorCode(MyData(2, 3)));

    const Mso::ErrorCode &e1 = m1.GetError();

    TestCheck(m1.IsError());
    TestCheckEqual(2, m1.GetError().As(MyErrorProvider())->Val1);
    TestCheckEqual(3, m1.GetError().As(MyErrorProvider())->Val2);

    TestCheckEqual(2, e1.As(MyErrorProvider())->Val1);
    TestCheckEqual(3, e1.As(MyErrorProvider())->Val2);
  }

  TESTMETHOD_REQUIRES_SEH(MaybeVoid_GetErrorCrash) {
    Mso::Maybe<void> m1;

    TestCheckCrash(m1.GetError());
  }

  TEST_METHOD(MaybeVoid_TakeError) {
    Mso::Maybe<void> m1(MyErrorProvider().MakeErrorCode(MyData(2, 3)));

    Mso::ErrorCode e1 = m1.TakeError();

    TestCheck(m1.IsError());
    TestCheck(!m1.GetError());

    TestCheckEqual(2, e1.As(MyErrorProvider())->Val1);
    TestCheckEqual(3, e1.As(MyErrorProvider())->Val2);
  }

  TESTMETHOD_REQUIRES_SEH(MaybeVoid_GetTakeCrash) {
    Mso::Maybe<void> m1;

    TestCheckCrash(m1.TakeError());
  }

  TEST_METHOD(Error) {
    auto err = MyErrorProvider().MakeErrorCode(MyData(1, 2));
    TestCheck(err.As(MyErrorProvider()));
    TestCheckEqual(1, err.As(MyErrorProvider())->Val1);
    TestCheckEqual(2, err.As(MyErrorProvider())->Val2);

    auto err2 = err;
    TestCheck(err2.As(MyErrorProvider()));
    TestCheckEqual(1, err2.As(MyErrorProvider())->Val1);
    TestCheckEqual(2, err2.As(MyErrorProvider())->Val2);
    TestCheckEqual(1, err.As(MyErrorProvider())->Val1);
    TestCheckEqual(2, err.As(MyErrorProvider())->Val2);

    auto err3 = std::move(err);
    TestCheck(err3.As(MyErrorProvider()));
    TestCheckEqual(1, err3.As(MyErrorProvider())->Val1);
    TestCheckEqual(2, err3.As(MyErrorProvider())->Val2);
    TestCheck(!err);
  }

  TEST_METHOD(ExceptionProviderThrowError) {
    Mso::Maybe<int> x;
    try {
      throw std::runtime_error("Test");
    } catch (std::runtime_error & /*e*/) {
      x = Mso::ExceptionErrorProvider().MakeErrorCode(std::current_exception());
    }

    TestCheckException(std::runtime_error, x.ThrowOnError());
  }

  TEST_METHOD(HResultProviderThrowError) {
    Mso::Maybe<int> x = Mso::HResultErrorProvider().MakeErrorCode(E_FAIL);
    TestCheckException(Mso::ErrorCodeException, x.ThrowOnError());
  }

  TEST_METHOD(ErrorCode_HandleSpecific) {
    Mso::ErrorCode error = MyErrorProvider().MakeErrorCode(MyData(2, 3));
    TestCheck(!error.IsHandled());
    TestCheck(error.HandleAs(MyErrorProvider()));
    TestCheck(error.IsHandled());
  }

  TEST_METHOD(ErrorCode_HandleSpecificChain) {
    Mso::ErrorCode error = MyErrorProvider().MakeErrorCode(MyData(2, 3));

    if (const std::exception_ptr *exceptionError = error.HandleAs(Mso::ExceptionErrorProvider())) {
      (void)exceptionError; // In real code we would evaluate the error here.
      TestCheck(false);
    } else if (const HRESULT *hResultError = error.HandleAs(Mso::HResultErrorProvider())) {
      (void)hResultError; // In real code we would evaluate the error here.
      TestCheck(false);
    } else if (const MyData *myError = error.HandleAs(MyErrorProvider())) {
      TestCheckEqual(2, myError->Val1);
      TestCheckEqual(3, myError->Val2);
    }

    TestCheck(error.IsHandled());
  }
};
