// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "functional/functor.h"
#include <stdexcept>
#include "functorTest.h"
#include "motifCpp/testCheck.h"

namespace FunctionalTests {

// To test how we can pass Functor to a method.
struct VoidFunctorExecutor {
  static void Execute(Mso::VoidFunctor &&func) noexcept {
    auto funcField = std::move(func); // Simulate storing functor in a field before execution.
    funcField();
  }
};

// To test how we can pass FunctorThrow to a method.
struct VoidFunctorThrowExecutor {
  static void Execute(Mso::VoidFunctorThrow &&func) {
    auto funcField = std::move(func); // Simulate storing functor in a field before execution.
    funcField();
  }
};

struct TestVoidFunctor : Mso::VoidFunctorImpl {
  virtual void Invoke() noexcept override {
    ++m_callCount;
  }

  int32_t m_callCount{0};
};

struct TestVoidFunctorThrow : Mso::VoidFunctorThrowImpl {
  virtual void Invoke() override {
    ++m_callCount;
  }

  int32_t m_callCount{0};
};

// To test how we can pass std::function to a method.
struct MyFunctorExecutor {
  static void Execute(Mso::Functor<void(StructParam *)> &&func) noexcept {
    auto funcField = std::move(func); // Simulate storing functor in a field before execution.
    StructParam param;
    funcField(&param);
  }
};

// To test how we can pass std::function to a method.
struct MyFunctorThrowExecutor {
  static void Execute(Mso::FunctorThrow<void(StructParam *)> &&func) {
    auto funcField = std::move(func); // Simulate storing functor in a field before execution.
    StructParam param;
    funcField(&param);
  }
};

int g_freeFunctionState = 0;

static void TestFreeFunction_Increment() noexcept {
  g_freeFunctionState++;
}

static void TestFreeFunction_Decrement() noexcept {
  g_freeFunctionState--;
}

static int TestFreeFunction_Add(int i) noexcept {
  g_freeFunctionState += i;
  return g_freeFunctionState;
}

static int TestFreeFunction_Subtract(int i) noexcept {
  g_freeFunctionState -= i;
  return g_freeFunctionState;
}

static void TestFreeFunction_Throw(int i) {
  if (i < 0)
    throw std::runtime_error("test");
}

TEST_CLASS (FunctorTest) {
  TEST_METHOD(Functor_ctor_Default) {
    Mso::Functor<int(int, int)> f1;
    TestCheck(f1.IsEmpty());
  }

  TEST_METHOD(Functor_ctor_nullptr) {
    Mso::Functor<int(int, int)> f1(nullptr);
    TestCheck(f1.IsEmpty());
  }

  TEST_METHOD(Functor_ctor_Copy) {
    Mso::Functor<int(int, int)> f1 = [](int x, int y) noexcept {
      return x + y;
    };
    Mso::Functor<int(int, int)> f2(f1);
    TestCheck(!f1.IsEmpty());
    TestCheck(f1 == f2);
  }

  TEST_METHOD(Functor_ctor_Move) {
    Mso::Functor<int(int, int)> f1 = [](int x, int y) noexcept {
      return x + y;
    };
    Mso::Functor<int(int, int)> f2(f1); // copy of f1
    Mso::Functor<int(int, int)> f3(std::move(f1));
    TestCheck(f1.IsEmpty());
    TestCheck(!f3.IsEmpty());
    TestCheck(f3 == f2);
  }

  TEST_METHOD(Functor_ctor_DoNothingFunctor) {
    auto f1 = Mso::Functor<int(int, int)>::DoNothing();
    auto f2 = Mso::Functor<int(int, int)>::DoNothing();
    TestCheck(!f1.IsEmpty());
    TestCheck(f1.Get() == f2.Get());
  }

  TEST_METHOD(VoidFunctor_ctor_DoNothingFunctor) {
    auto f1 = Mso::VoidFunctor::DoNothing();
    auto f2 = Mso::VoidFunctor::DoNothing();
    TestCheck(!f1.IsEmpty());
    TestCheck(f1.Get() == f2.Get());
  }

  TEST_METHOD(Functor_ctor_IFunctor_Ptr) {
    auto functorImpl = Mso::Make<TestVoidFunctor>();
    Mso::VoidFunctor f1(functorImpl.Get());
    TestCheck(!f1.IsEmpty());
    TestCheck(f1.Get() == functorImpl.Get());
    f1();
    TestCheckEqual(1, functorImpl->m_callCount);
  }

  TEST_METHOD(Functor_ctor_IFunctor_CntPtrCopy) {
    auto functorImpl = Mso::Make<TestVoidFunctor>();
    Mso::VoidFunctor f1(functorImpl);
    TestCheck(!f1.IsEmpty());
    TestCheck(f1.Get() == functorImpl.Get());
    f1();
    TestCheckEqual(1, functorImpl->m_callCount);
  }

  TEST_METHOD(Functor_ctor_IFunctor_ConstCntPtrCopy) {
    const auto functorImpl = Mso::Make<TestVoidFunctor>();
    Mso::VoidFunctor f1(functorImpl);
    TestCheck(!f1.IsEmpty());
    TestCheck(f1.Get() == functorImpl.Get());
    f1();
    TestCheckEqual(1, functorImpl->m_callCount);
  }

  TEST_METHOD(Functor_ctor_IFunctor_CntPtrMove) {
    auto functorImpl = Mso::Make<TestVoidFunctor>();
    Mso::CntPtr<TestVoidFunctor> functorImplCopy = functorImpl; // keep original to check call count
    Mso::VoidFunctor f1(std::move(functorImplCopy));
    TestCheck(!f1.IsEmpty());
    TestCheck(functorImplCopy.IsEmpty());
    TestCheck(f1.Get() == functorImpl.Get());
    f1();
    TestCheckEqual(1, functorImpl->m_callCount);
  }

  TEST_METHOD(Functor_ctor_IFunctor_Ptr_PassToMethod) {
    auto functorImpl = Mso::Make<TestVoidFunctor>();
    VoidFunctorExecutor::Execute(functorImpl.Get());

    TestCheckEqual(1, functorImpl->m_callCount);
  }

  TEST_METHOD(Functor_ctor_IFunctor_CntPtrCopy_PassToMethod) {
    auto functorImpl = Mso::Make<TestVoidFunctor>();
    VoidFunctorExecutor::Execute(functorImpl);

    TestCheckEqual(1, functorImpl->m_callCount);
  }

  TEST_METHOD(Functor_ctor_IFunctor_ConstCntPtrCopy_PassToMethod) {
    const auto functorImpl = Mso::Make<TestVoidFunctor>();
    VoidFunctorExecutor::Execute(functorImpl);

    TestCheckEqual(1, functorImpl->m_callCount);
  }

  TEST_METHOD(Functor_ctor_IFunctor_CntPtrMove_PassToMethod) {
    auto functorImpl = Mso::Make<TestVoidFunctor>();
    Mso::CntPtr<TestVoidFunctor> functorImplCopy = functorImpl; // keep original to check call count
    VoidFunctorExecutor::Execute(std::move(functorImplCopy));

    TestCheckEqual(1, functorImpl->m_callCount);
  }

  TEST_METHOD(Functor_ctor_Lambda) {
    Mso::Functor<int(int, int)> f1 = [](int x, int y) noexcept {
      return x + y;
    };
    TestCheck(!f1.IsEmpty());
    TestCheckEqual(5, f1(2, 3));
  }

  TEST_METHOD(Functor_ctor_Lambda_copy) {
    auto lambda = [](int x, int y) noexcept {
      return x + y;
    };
    Mso::Functor<int(int, int)> f1(lambda);
    TestCheck(!f1.IsEmpty());
    TestCheckEqual(5, f1(2, 3));
  }

  TEST_METHOD(Functor_ctor_Lambda_move) {
    auto lambda = [](int x, int y) noexcept {
      return x + y;
    };
    Mso::Functor<int(int, int)> f1(std::move(lambda));
    TestCheck(!f1.IsEmpty());
    TestCheckEqual(5, f1(2, 3));
  }

  TEST_METHOD(Functor_ctor_Lambda_CaptureCopyCount) {
    int addRefCalls = 0;
    int releaseCalls = 0;
    {
      auto data = Mso::Make<TestData>(addRefCalls, releaseCalls);
      data->Value = 3;
      Mso::Functor<int(int, int)> f1 = ([data](int x, int y) noexcept { return x + y + data->Value; });

      TestCheckEqual(13, f1(4, 6));
    }

    TestCheckEqual(1, addRefCalls);
    TestCheckEqual(2, releaseCalls);
  }

  TEST_METHOD(Functor_ctor_Lambda_MoveCapture) {
    int addRefCalls = 0;
    int releaseCalls = 0;
    {
      auto data = Mso::Make<TestData>(addRefCalls, releaseCalls);
      data->Value = 3;
      Mso::Functor<int(int, int)> f1 = [movedData = std::move(data)](int x, int y) noexcept {
        return x + y + movedData->Value;
      };

      TestCheckEqual(13, f1(4, 6));
    }

    TestCheckEqual(0, addRefCalls);
    TestCheckEqual(1, releaseCalls);
  }

  TEST_METHOD(Functor_ctor_MutableLambda) {
    Mso::Functor<int()> increment([i = 0]() mutable noexcept->int { return i++; });

    TestCheckEqual(0, increment());
    TestCheckEqual(1, increment());
  }

  TEST_METHOD(Functor_ctor_MutableLambda_NoCapture) {
    static int i = 0;
    i = 0;
    Mso::Functor<int()> increment([]() mutable noexcept->int { return i++; });

    TestCheckEqual(0, increment());
    TestCheckEqual(1, increment());
  }

  TEST_METHOD(Functor_ctor_StaticMethod) {
    // noexcept is not part of function pointer until C++17
    Mso::Functor<int(int, int)> f1{&StaticMethod::Add, Mso::TerminateOnException};
    TestCheck(!f1.IsEmpty());
    TestCheckEqual(5, f1(2, 3));
  }

  TEST_METHOD(Functor_ctor_StdFunctionCopy) {
    int callCount = 0;
    int value = 0;
    auto func = std::function<int(int)>([&callCount, &value ](int v) noexcept {
      ++callCount;
      value = v;
      return v;
    });
    Mso::Functor<int(int)> f1(func, Mso::TerminateOnException);
    TestCheckEqual(5, f1(5));
    TestCheckEqual(1, callCount);
    TestCheckEqual(5, value);
  }

  TEST_METHOD(Functor_ctor_ConstStdFunctionCopy) {
    int callCount = 0;
    int value = 0;
    const auto func = std::function<int(int)>([&callCount, &value ](int v) noexcept {
      ++callCount;
      value = v;
      return v;
    });
    Mso::Functor<int(int)> f1(func, Mso::TerminateOnException);
    TestCheckEqual(5, f1(5));
    TestCheckEqual(1, callCount);
    TestCheckEqual(5, value);
  }

  TEST_METHOD(Functor_ctor_StdFunctionMove) {
    int callCount = 0;
    int value = 0;
    auto func = std::function<int(int)>([&callCount, &value ](int v) noexcept {
      ++callCount;
      value = v;
      return v;
    });
    Mso::Functor<int(int)> f1(std::move(func), Mso::TerminateOnException);
    TestCheckEqual(5, f1(5));
    TestCheckEqual(1, callCount);
    TestCheckEqual(5, value);
  }

  TEST_METHOD(Functor_ctor_StdFunctionCopy_PassToMethod) {
    int callCount = 0;
    auto func = std::function<void()>([&callCount]() noexcept { ++callCount; });
    VoidFunctorExecutor::Execute({func, Mso::TerminateOnException});
    TestCheckEqual(1, callCount);
  }

  TEST_METHOD(Functor_ctor_ConstStdFunctionCopy_PassToMethod) {
    int callCount = 0;
    const auto func = std::function<void()>([&callCount]() noexcept { ++callCount; });
    VoidFunctorExecutor::Execute({func, Mso::TerminateOnException});
    TestCheckEqual(1, callCount);
  }

  TEST_METHOD(Functor_ctor_StdFunctionMove_PassToMethod) {
    int callCount = 0;
    auto func = std::function<void()>([&callCount]() noexcept { ++callCount; });
    VoidFunctorExecutor::Execute({std::move(func), Mso::TerminateOnException});
    TestCheckEqual(1, callCount);
  }

  TEST_METHOD(Functor_ctor_StdFunctionCopy_DifferentSignature_PassToMethod) {
    int callCount = 0;
    auto func =
        std::function<void(Mso::CntPtr<StructParam>)>([&callCount](Mso::CntPtr<StructParam>) noexcept { ++callCount; });
    MyFunctorExecutor::Execute({func, Mso::TerminateOnException});
    TestCheckEqual(1, callCount);
  }

  TEST_METHOD(Functor_ctor_ConstStdFunctionCopy_DifferentSignature_PassToMethod) {
    int callCount = 0;
    const auto func =
        std::function<void(Mso::CntPtr<StructParam>)>([&callCount](Mso::CntPtr<StructParam>) noexcept { ++callCount; });
    MyFunctorExecutor::Execute({func, Mso::TerminateOnException});
    TestCheckEqual(1, callCount);
  }

  TEST_METHOD(Functor_ctor_StdFunctionMove_DifferentSignature_PassToMethod) {
    int callCount = 0;
    auto func =
        std::function<void(Mso::CntPtr<StructParam>)>([&callCount](Mso::CntPtr<StructParam>) noexcept { ++callCount; });
    MyFunctorExecutor::Execute({std::move(func), Mso::TerminateOnException});
    TestCheckEqual(1, callCount);
  }

  TEST_METHOD(Functor_Assign_nullptr) {
    Mso::Functor<int(int, int)> f1 = [](int x, int y) noexcept {
      return x + y;
    };
    f1 = nullptr;
    TestCheck(f1.IsEmpty());
  }

  TEST_METHOD(Functor_Assign_Copy) {
    Mso::Functor<int(int, int)> f1 = [](int x, int y) noexcept {
      return x + y;
    };
    Mso::Functor<int(int, int)> f2 = [](int x, int y) noexcept {
      return x - y;
    };
    TestCheck(f1 != f2);
    Mso::Functor<int(int, int)> f3(f1); // copy of f1
    f1 = f2;
    TestCheck(!f2.IsEmpty());
    TestCheck(f1 == f2);
    TestCheck(f1 != f3);
  }

  TEST_METHOD(Functor_Assign_Move) {
    Mso::Functor<int(int, int)> f1 = [](int x, int y) noexcept {
      return x + y;
    };
    Mso::Functor<int(int, int)> f2 = [](int x, int y) noexcept {
      return x - y;
    };
    Mso::Functor<int(int, int)> f3(f1); // copy of f1
    Mso::Functor<int(int, int)> f4(f2); // copy of f2
    f1 = std::move(f2);
    TestCheck(!f1.IsEmpty());
    TestCheck(f2.IsEmpty());
    TestCheck(f1 != f3);
    TestCheck(f1 == f4);
  }

  TESTMETHOD_REQUIRES_SEH(Functor_operator_call_Empty) {
    Mso::Functor<int(int, int)> f1;
    TestCheckCrash(f1(1, 2));
  }

  TEST_METHOD(Functor_operator_call_NonEmpty) {
    Mso::Functor<int(int, int)> f1 = ([](int x, int y) noexcept { return x + y; });
    TestCheckEqual(10, f1(4, 6));
  }

  TESTMETHOD_REQUIRES_SEH(Functor_operator_call_Throws) {
    Mso::Functor<int(int)> f1 = ([](int) noexcept->int {
      OACR_NOEXCEPT_MAYTERMINATE;
#pragma warning(suppress : 4297) // Suppress warning about throwing in noexcept function.
      throw std::runtime_error("Test error");
    });
    TestCheckTerminate(f1(5));
  }

  TESTMETHOD_REQUIRES_SEH(Functor_operator_call_StdFunction_Throws) {
    int callCount = 0;
    auto func = std::function<int(int)>([&callCount](int) -> int {
      ++callCount;
      throw std::runtime_error("Test error");
    });
    Mso::Functor<int(int)> f1({std::move(func), Mso::TerminateOnException});
    TestCheckTerminate(f1(5));
    TestCheckEqual(1, callCount);
  }

  TEST_METHOD(Functor_operator_call_Void) {
    int result = 0;
    Mso::Functor<void(int, int)> f1 = [&result](int x, int y) noexcept {
      result = x + y;
    };
    TestCheck(!f1.IsEmpty());
    f1(4, 6);
    TestCheckEqual(10, result);
  }

  TEST_METHOD(Functor_operator_call_WithCapture) {
    int bias = 5;
    auto add = Mso::Functor<int(int, int)>([bias](int x, int y) noexcept { return x + y + bias; });
    TestCheckEqual(15, add(4, 6));
  }

  TEST_METHOD(Functor_IsEmpty) {
    Mso::Functor<int(int, int)> f1 = [](int x, int y) noexcept {
      return x + y;
    };
    Mso::Functor<int(int, int)> f2;
    TestCheck(!f1.IsEmpty());
    TestCheck(f2.IsEmpty());
  }

  TEST_METHOD(Functor_operator_bool) {
    Mso::Functor<int(int, int)> f1 = [](int x, int y) noexcept {
      return x + y;
    };
    Mso::Functor<int(int, int)> f2;
    TestCheck(f1);
    TestCheck(!f2);
  }

  TEST_METHOD(Functor_Swap) {
    Mso::Functor<int(int, int)> f1 = [](int x, int y) noexcept {
      return x + y;
    };
    Mso::Functor<int(int, int)> f2 = [](int x, int y) noexcept {
      return x - y;
    };
    Mso::Functor<int(int, int)> f3(f1); // copy of f1
    Mso::Functor<int(int, int)> f4(f2); // copy of f2
    f1.Swap(f2);
    TestCheck(f1 == f4);
    TestCheck(f2 == f3);
  }

  TEST_METHOD(Functor_std_swap) {
    using std::swap; // The typical pattern how to call the swap method.
    Mso::Functor<int(int, int)> f1 = [](int x, int y) noexcept {
      return x + y;
    };
    Mso::Functor<int(int, int)> f2 = [](int x, int y) noexcept {
      return x - y;
    };
    Mso::Functor<int(int, int)> f3(f1); // copy of f1
    Mso::Functor<int(int, int)> f4(f2); // copy of f2
    swap(f1, f2); // Never use the std prefix for swap: the swap can be overridden in the namespace of T.
    TestCheck(f1 == f4);
    TestCheck(f2 == f3);
  }

  TEST_METHOD(Functor_Get) {
    Mso::Functor<int(int, int)> f1 = [](int x, int y) noexcept {
      return x + y;
    };
    Mso::Functor<int(int, int)> f2;
    TestCheck(f1.Get() != nullptr);
    TestCheck(f2.Get() == nullptr);
  }

  TEST_METHOD(Functor_Detach) {
    Mso::Functor<int(int, int)> f1 = [](int x, int y) noexcept {
      return x + y;
    };

    Mso::CntPtr<Mso::IFunctor<int, int, int>> impl{f1.Detach(), Mso::AttachTag};
    TestCheck(f1.IsEmpty());
    TestCheck(!impl.IsEmpty());
  }

  TEST_METHOD(Functor_operator_Equal) {
    using std::swap; // The typical pattern how to call the swap method.
    Mso::Functor<int(int, int)> f1 = [](int x, int y) noexcept {
      return x + y;
    };
    Mso::Functor<int(int, int)> f2 = [](int x, int y) noexcept {
      return x - y;
    };
    Mso::Functor<int(int, int)> f3(f1); // copy of f1
    Mso::Functor<int(int, int)> f4;
    TestCheck(f1 == f3);
    TestCheck(f3 == f1);
    TestCheck(f1 != f2);
    TestCheck(f2 != f1);
    TestCheck(f4 != f1);
    TestCheck(f1 != f4);
    TestCheck(f4 == nullptr);
    TestCheck(nullptr == f4);
    TestCheck(f1 != nullptr);
    TestCheck(nullptr != f1);
  }

  TEST_METHOD(Functor_DoNothing) {
    Mso::Functor<int(int)> f1 = Mso::Functor<int(int)>::DoNothing();
    TestCheck(!f1.IsEmpty());
    TestCheckEqual(0, f1(1));

    Mso::Functor<double(double)> f2 = Mso::Functor<double(double)>::DoNothing();
    TestCheck(!f2.IsEmpty());
    TestCheckEqual(0.0, f2(3.14));

    Mso::Functor<int(int, int)> f3 = Mso::Functor<int(int, int)>::DoNothing();
    TestCheck(!f3.IsEmpty());
    TestCheckEqual(0, f3(100, 200));

    Mso::Functor<int *(int, int)> f4 = Mso::Functor<int *(int, int)>::DoNothing();
    TestCheck(!f4.IsEmpty());
    TestCheckEqual(nullptr, f4(1000, 2000));
  }

  TEST_METHOD(Functor_GenericLambda) {
    auto adder = [](const auto &a, const auto &b) noexcept {
      return a + b;
    };

    Mso::Functor<int(int, int)> fnInt = adder;
    Mso::Functor<double(double, double)> fnDouble = adder;
    Mso::Functor<std::string(const std::string &, const std::string &)> fnString = adder;
  }

  TEST_METHOD(FunctorThrow_ctor_Default) {
    Mso::FunctorThrow<int(int, int)> f1;
    TestCheck(f1.IsEmpty());
  }

  TEST_METHOD(FunctorThrow_ctor_nullptr) {
    Mso::FunctorThrow<int(int, int)> f1(nullptr);
    TestCheck(f1.IsEmpty());
  }

  TEST_METHOD(FunctorThrow_ctor_Copy) {
    Mso::FunctorThrow<int(int, int)> f1 = [](int x, int y) noexcept {
      return x + y;
    };
    Mso::FunctorThrow<int(int, int)> f2(f1);
    TestCheck(!f1.IsEmpty());
    TestCheck(f1 == f2);
  }

  TEST_METHOD(FunctorThrow_ctor_Move) {
    Mso::FunctorThrow<int(int, int)> f1 = [](int x, int y) noexcept {
      return x + y;
    };
    Mso::FunctorThrow<int(int, int)> f2(f1); // copy of f1
    Mso::FunctorThrow<int(int, int)> f3(std::move(f1));
    TestCheck(f1.IsEmpty());
    TestCheck(!f3.IsEmpty());
    TestCheck(f3 == f2);
  }

  TEST_METHOD(FunctorThrow_ctor_DoNothingFunctorThrow) {
    auto f1 = Mso::FunctorThrow<int(int, int)>::DoNothing();
    auto f2 = Mso::FunctorThrow<int(int, int)>::DoNothing();
    TestCheck(!f1.IsEmpty());
    TestCheck(f1.Get() == f2.Get());
  }

  TEST_METHOD(FunctorThrow_ctor_IFunctorThrow_Ptr) {
    auto functorImpl = Mso::Make<TestVoidFunctorThrow>();
    Mso::VoidFunctorThrow f1{functorImpl.Get()};
    TestCheck(!f1.IsEmpty());
    TestCheck(f1.Get() == functorImpl.Get());
    f1();
    TestCheckEqual(1, functorImpl->m_callCount);
  }

  TEST_METHOD(FunctorThrow_ctor_IFunctorThrow_CntPtrCopy) {
    auto functorImpl = Mso::Make<TestVoidFunctorThrow>();
    Mso::VoidFunctorThrow f1(functorImpl);
    TestCheck(!f1.IsEmpty());
    TestCheck(f1.Get() == functorImpl.Get());
    f1();
    TestCheckEqual(1, functorImpl->m_callCount);
  }

  TEST_METHOD(FunctorThrow_ctor_IFunctorThrow_ConstCntPtrCopy) {
    const auto functorImpl = Mso::Make<TestVoidFunctorThrow>();
    Mso::VoidFunctorThrow f1(functorImpl);
    TestCheck(!f1.IsEmpty());
    TestCheck(f1.Get() == functorImpl.Get());
    f1();
    TestCheckEqual(1, functorImpl->m_callCount);
  }

  TEST_METHOD(FunctorThrow_ctor_IFunctorThrow_CntPtrMove) {
    auto functorImpl = Mso::Make<TestVoidFunctorThrow>();
    Mso::CntPtr<TestVoidFunctorThrow> functorImplCopy = functorImpl; // keep original to check call count
    Mso::VoidFunctorThrow f1(std::move(functorImplCopy));
    TestCheck(!f1.IsEmpty());
    TestCheck(functorImplCopy.IsEmpty());
    TestCheck(f1.Get() == functorImpl.Get());
    f1();
    TestCheckEqual(1, functorImpl->m_callCount);
  }

  TEST_METHOD(FunctorThrow_ctor_IFunctorThrow_Ptr_PassToMethod) {
    auto functorImpl = Mso::Make<TestVoidFunctorThrow>();
    VoidFunctorThrowExecutor::Execute(functorImpl.Get());

    TestCheckEqual(1, functorImpl->m_callCount);
  }

  TEST_METHOD(FunctorThrow_ctor_IFunctorThrow_CntPtrCopy_PassToMethod) {
    auto functorImpl = Mso::Make<TestVoidFunctorThrow>();
    VoidFunctorThrowExecutor::Execute(functorImpl);

    TestCheckEqual(1, functorImpl->m_callCount);
  }

  TEST_METHOD(FunctorThrow_ctor_IFunctorThrow_ConstCntPtrCopy_PassToMethod) {
    const auto functorImpl = Mso::Make<TestVoidFunctorThrow>();
    VoidFunctorThrowExecutor::Execute(functorImpl);

    TestCheckEqual(1, functorImpl->m_callCount);
  }

  TEST_METHOD(FunctorThrow_ctor_IFunctorThrow_CntPtrMove_PassToMethod) {
    auto functorImpl = Mso::Make<TestVoidFunctorThrow>();
    Mso::CntPtr<TestVoidFunctorThrow> functorImplCopy = functorImpl; // keep original to check call count
    VoidFunctorThrowExecutor::Execute(std::move(functorImplCopy));

    TestCheckEqual(1, functorImpl->m_callCount);
  }

  TEST_METHOD(FunctorThrow_ctor_Lambda) {
    Mso::FunctorThrow<int(int, int)> f1 = [](int x, int y) noexcept {
      return x + y;
    };
    TestCheck(!f1.IsEmpty());
    TestCheckEqual(5, f1(2, 3));
  }

  TEST_METHOD(FunctorThrow_ctor_Lambda_copy) {
    auto lambda = [](int x, int y) noexcept {
      return x + y;
    };
    Mso::FunctorThrow<int(int, int)> f1(lambda);
    TestCheck(!f1.IsEmpty());
    TestCheckEqual(5, f1(2, 3));
  }

  TEST_METHOD(FunctorThrow_ctor_Lambda_move) {
    auto lambda = [](int x, int y) noexcept {
      return x + y;
    };
    Mso::FunctorThrow<int(int, int)> f1(std::move(lambda));
    TestCheck(!f1.IsEmpty());
    TestCheckEqual(5, f1(2, 3));
  }

  TEST_METHOD(FunctorThrow_ctor_Lambda_CaptureCopyCount) {
    int addRefCalls = 0;
    int releaseCalls = 0;
    {
      auto data = Mso::Make<TestData>(addRefCalls, releaseCalls);
      data->Value = 3;
      Mso::FunctorThrow<int(int, int)> f1 = ([data](int x, int y) noexcept { return x + y + data->Value; });

      TestCheckEqual(13, f1(4, 6));
    }

    TestCheckEqual(1, addRefCalls);
    TestCheckEqual(2, releaseCalls);
  }

  TEST_METHOD(FunctorThrow_ctor_Lambda_MoveCapture) {
    int addRefCalls = 0;
    int releaseCalls = 0;
    {
      auto data = Mso::Make<TestData>(addRefCalls, releaseCalls);
      data->Value = 3;
      Mso::FunctorThrow<int(int, int)> f1 = [movedData = std::move(data)](int x, int y) noexcept {
        return x + y + movedData->Value;
      };

      TestCheckEqual(13, f1(4, 6));
    }

    TestCheckEqual(0, addRefCalls);
    TestCheckEqual(1, releaseCalls);
  }

  TEST_METHOD(FunctorThrow_ctor_MutableLambda) {
    Mso::FunctorThrow<int()> increment([i = 0]() mutable noexcept->int { return i++; });

    TestCheckEqual(0, increment());
    TestCheckEqual(1, increment());
  }

  TEST_METHOD(FunctorThrow_ctor_MutableLambda_NoCapture) {
    static int i = 0;
    i = 0;
    Mso::FunctorThrow<int()> increment([]() mutable noexcept->int { return i++; });

    TestCheckEqual(0, increment());
    TestCheckEqual(1, increment());
  }

  TEST_METHOD(FunctorThrow_ctor_StaticMethod) {
    Mso::FunctorThrow<int(int, int)> f1{&StaticMethod::Add};
    TestCheck(!f1.IsEmpty());
    TestCheckEqual(5, f1(2, 3));
  }

  TEST_METHOD(FunctorThrow_ctor_StdFunctionCopy) {
    int callCount = 0;
    int value = 0;
    auto func = std::function<int(int)>([&callCount, &value ](int v) noexcept {
      ++callCount;
      value = v;
      return v;
    });
    Mso::FunctorThrow<int(int)> f1(func);
    TestCheckEqual(5, f1(5));
    TestCheckEqual(1, callCount);
    TestCheckEqual(5, value);
  }

  TEST_METHOD(FunctorThrow_ctor_ConstStdFunctionCopy) {
    int callCount = 0;
    int value = 0;
    const auto func = std::function<int(int)>([&callCount, &value ](int v) noexcept {
      ++callCount;
      value = v;
      return v;
    });
    Mso::FunctorThrow<int(int)> f1(func);
    TestCheckEqual(5, f1(5));
    TestCheckEqual(1, callCount);
    TestCheckEqual(5, value);
  }

  TEST_METHOD(FunctorThrow_ctor_StdFunctionMove) {
    int callCount = 0;
    int value = 0;
    auto func = std::function<int(int)>([&callCount, &value ](int v) noexcept {
      ++callCount;
      value = v;
      return v;
    });
    Mso::FunctorThrow<int(int)> f1(std::move(func));
    TestCheckEqual(5, f1(5));
    TestCheckEqual(1, callCount);
    TestCheckEqual(5, value);
  }

  TEST_METHOD(FunctorThrow_ctor_StdFunctionCopy_PassToMethod) {
    int callCount = 0;
    auto func = std::function<void()>([&callCount]() noexcept { ++callCount; });
    VoidFunctorThrowExecutor::Execute(func);
    TestCheckEqual(1, callCount);
  }

  TEST_METHOD(FunctorThrow_ctor_ConstStdFunctionCopy_PassToMethod) {
    int callCount = 0;
    const auto func = std::function<void()>([&callCount]() noexcept { ++callCount; });
    VoidFunctorThrowExecutor::Execute(func);
    TestCheckEqual(1, callCount);
  }

  TEST_METHOD(FunctorThrow_ctor_StdFunctionMove_PassToMethod) {
    int callCount = 0;
    auto func = std::function<void()>([&callCount]() noexcept { ++callCount; });
    VoidFunctorThrowExecutor::Execute(std::move(func));
    TestCheckEqual(1, callCount);
  }

  TEST_METHOD(FunctorThrow_ctor_StdFunctionCopy_DifferentSignature_PassToMethod) {
    int callCount = 0;
    auto func =
        std::function<void(Mso::CntPtr<StructParam>)>([&callCount](Mso::CntPtr<StructParam>) noexcept { ++callCount; });
    MyFunctorThrowExecutor::Execute(func);
    TestCheckEqual(1, callCount);
  }

  TEST_METHOD(FunctorThrow_ctor_ConstStdFunctionCopy_DifferentSignature_PassToMethod) {
    int callCount = 0;
    const auto func =
        std::function<void(Mso::CntPtr<StructParam>)>([&callCount](Mso::CntPtr<StructParam>) noexcept { ++callCount; });
    MyFunctorThrowExecutor::Execute(func);
    TestCheckEqual(1, callCount);
  }

  TEST_METHOD(FunctorThrow_ctor_StdFunctionMove_DifferentSignature_PassToMethod) {
    int callCount = 0;
    auto func =
        std::function<void(Mso::CntPtr<StructParam>)>([&callCount](Mso::CntPtr<StructParam>) noexcept { ++callCount; });
    MyFunctorThrowExecutor::Execute(std::move(func));
    TestCheckEqual(1, callCount);
  }

  TEST_METHOD(FunctorThrow_Assign_nullptr) {
    Mso::FunctorThrow<int(int, int)> f1 = [](int x, int y) noexcept {
      return x + y;
    };
    f1 = nullptr;
    TestCheck(f1.IsEmpty());
  }

  TEST_METHOD(FunctorThrow_Assign_Copy) {
    Mso::FunctorThrow<int(int, int)> f1 = [](int x, int y) noexcept {
      return x + y;
    };
    Mso::FunctorThrow<int(int, int)> f2 = [](int x, int y) noexcept {
      return x - y;
    };
    TestCheck(f1 != f2);
    Mso::FunctorThrow<int(int, int)> f3(f1); // copy of f1
    f1 = f2;
    TestCheck(!f2.IsEmpty());
    TestCheck(f1 == f2);
    TestCheck(f1 != f3);
  }

  TEST_METHOD(FunctorThrow_Assign_Move) {
    Mso::FunctorThrow<int(int, int)> f1 = [](int x, int y) noexcept {
      return x + y;
    };
    Mso::FunctorThrow<int(int, int)> f2 = [](int x, int y) noexcept {
      return x - y;
    };
    Mso::FunctorThrow<int(int, int)> f3(f1); // copy of f1
    Mso::FunctorThrow<int(int, int)> f4(f2); // copy of f2
    f1 = std::move(f2);
    TestCheck(!f1.IsEmpty());
    TestCheck(f2.IsEmpty());
    TestCheck(f1 != f3);
    TestCheck(f1 == f4);
  }

  TESTMETHOD_REQUIRES_SEH(FunctorThrow_operator_call_Empty) {
    Mso::FunctorThrow<int(int, int)> f1;
    TestCheckCrash(f1(1, 2));
  }

  TEST_METHOD(FunctorThrow_operator_call_NonEmpty) {
    Mso::FunctorThrow<int(int, int)> f1 = ([](int x, int y) noexcept { return x + y; });
    TestCheckEqual(10, f1(4, 6));
  }

  TEST_METHOD(FunctorThrow_operator_call_Throws) {
    Mso::FunctorThrow<int(int)> f1 = ([](int) -> int { throw std::runtime_error("Test error"); });
    TestCheckException(std::runtime_error, f1(5));
  }

  TEST_METHOD(FunctorThrow_operator_call_StdFunction_Throws) {
    int callCount = 0;
    auto func = std::function<int(int)>([&callCount](int) -> int {
      ++callCount;
      throw std::runtime_error("Test error");
    });
    Mso::FunctorThrow<int(int)> f1(std::move(func));
    TestCheckException(std::runtime_error, f1(5));
    TestCheckEqual(1, callCount);
  }

  TEST_METHOD(FunctorThrow_operator_call_Void) {
    int result = 0;
    Mso::FunctorThrow<void(int, int)> f1 = [&result](int x, int y) noexcept {
      result = x + y;
    };
    TestCheck(!f1.IsEmpty());
    f1(4, 6);
    TestCheckEqual(10, result);
  }

  TEST_METHOD(FunctorThrow_operator_call_WithCapture) {
    int bias = 5;
    auto add = Mso::FunctorThrow<int(int, int)>([bias](int x, int y) noexcept { return x + y + bias; });
    TestCheckEqual(15, add(4, 6));
  }

  TEST_METHOD(FunctorThrow_IsEmpty) {
    Mso::FunctorThrow<int(int, int)> f1 = [](int x, int y) noexcept {
      return x + y;
    };
    Mso::FunctorThrow<int(int, int)> f2;
    TestCheck(!f1.IsEmpty());
    TestCheck(f2.IsEmpty());
  }

  TEST_METHOD(FunctorThrow_operator_bool) {
    Mso::FunctorThrow<int(int, int)> f1 = [](int x, int y) noexcept {
      return x + y;
    };
    Mso::FunctorThrow<int(int, int)> f2;
    TestCheck(f1);
    TestCheck(!f2);
  }

  TEST_METHOD(FunctorThrow_Swap) {
    Mso::FunctorThrow<int(int, int)> f1 = [](int x, int y) noexcept {
      return x + y;
    };
    Mso::FunctorThrow<int(int, int)> f2 = [](int x, int y) noexcept {
      return x - y;
    };
    Mso::FunctorThrow<int(int, int)> f3(f1); // copy of f1
    Mso::FunctorThrow<int(int, int)> f4(f2); // copy of f2
    f1.Swap(f2);
    TestCheck(f1 == f4);
    TestCheck(f2 == f3);
  }

  TEST_METHOD(FunctorThrow_std_swap) {
    using std::swap; // The typical pattern how to call the swap method.
    Mso::FunctorThrow<int(int, int)> f1 = [](int x, int y) noexcept {
      return x + y;
    };
    Mso::FunctorThrow<int(int, int)> f2 = [](int x, int y) noexcept {
      return x - y;
    };
    Mso::FunctorThrow<int(int, int)> f3(f1); // copy of f1
    Mso::FunctorThrow<int(int, int)> f4(f2); // copy of f2
    swap(f1, f2); // Never use the std prefix for swap: the swap can be overridden in the namespace of T.
    TestCheck(f1 == f4);
    TestCheck(f2 == f3);
  }

  TEST_METHOD(FunctorThrow_Get) {
    Mso::FunctorThrow<int(int, int)> f1 = [](int x, int y) noexcept {
      return x + y;
    };
    Mso::FunctorThrow<int(int, int)> f2;
    TestCheck(f1.Get() != nullptr);
    TestCheck(f2.Get() == nullptr);
  }

  TEST_METHOD(FunctorThrow_Detach) {
    Mso::FunctorThrow<int(int, int)> f1 = [](int x, int y) noexcept {
      return x + y;
    };

    Mso::CntPtr<Mso::IFunctorThrow<int, int, int>> impl{f1.Detach(), Mso::AttachTag};
    TestCheck(f1.IsEmpty());
    TestCheck(!impl.IsEmpty());
  }

  TEST_METHOD(FunctorThrow_operator_Equal) {
    using std::swap; // The typical pattern how to call the swap method.
    Mso::FunctorThrow<int(int, int)> f1 = [](int x, int y) noexcept {
      return x + y;
    };
    Mso::FunctorThrow<int(int, int)> f2 = [](int x, int y) noexcept {
      return x - y;
    };
    Mso::FunctorThrow<int(int, int)> f3(f1); // copy of f1
    Mso::FunctorThrow<int(int, int)> f4;
    TestCheck(f1 == f3);
    TestCheck(f3 == f1);
    TestCheck(f1 != f2);
    TestCheck(f2 != f1);
    TestCheck(f4 != f1);
    TestCheck(f1 != f4);
    TestCheck(f4 == nullptr);
    TestCheck(nullptr == f4);
    TestCheck(f1 != nullptr);
    TestCheck(nullptr != f1);
  }

  TEST_METHOD(FunctorThrow_DoNothing) {
    Mso::FunctorThrow<int(int)> f1 = Mso::FunctorThrow<int(int)>::DoNothing();
    TestCheck(!f1.IsEmpty());
    TestCheckEqual(0, f1(1));

    Mso::FunctorThrow<double(double)> f2 = Mso::FunctorThrow<double(double)>::DoNothing();
    TestCheck(!f2.IsEmpty());
    TestCheckEqual(0.0, f2(3.14));

    Mso::FunctorThrow<int(int, int)> f3 = Mso::FunctorThrow<int(int, int)>::DoNothing();
    TestCheck(!f3.IsEmpty());
    TestCheckEqual(0, f3(100, 200));

    Mso::FunctorThrow<int *(int, int)> f4 = Mso::FunctorThrow<int *(int, int)>::DoNothing();
    TestCheck(!f4.IsEmpty());
    TestCheckEqual(nullptr, f4(1000, 2000));
  }

  // Make sure that lambda can be matched to a correct Mso::Functor instantiation
  struct CheckFunctorOverload final {
    using FunctorAlias1 = Mso::Functor<void(int &)>;
    using FunctorAlias2 = Mso::Functor<void(int &, bool &)>;

    static int DoCheckOverload(const FunctorAlias1 &) noexcept {
      return 1;
    }
    static int DoCheckOverload(const FunctorAlias2 &) noexcept {
      return 2;
    }

    static int CheckOverload() noexcept {
      return DoCheckOverload([&](auto &) noexcept {}) + DoCheckOverload([&](auto &, auto &) noexcept {});
    }
  };

  // Make sure that lambda can be matched to a correct Mso::FunctorThrow instantiation
  struct CheckFunctorThrowOverload final {
    using FunctorAlias1 = Mso::FunctorThrow<void(int &)>;
    using FunctorAlias2 = Mso::FunctorThrow<void(int &, bool &)>;

    static int DoCheckOverload(const FunctorAlias1 &) noexcept {
      return 1;
    }
    static int DoCheckOverload(const FunctorAlias2 &) noexcept {
      return 2;
    }

    static int CheckOverload() noexcept {
      return DoCheckOverload([&](auto &) noexcept {}) + DoCheckOverload([&](auto &, auto &) noexcept {});
    }
  };

  TEST_METHOD(Functor_CheckOverload) {
    TestCheckEqual(3, CheckFunctorOverload::CheckOverload());
  }

  TEST_METHOD(FunctorThrow_CheckOverload) {
    TestCheckEqual(3, CheckFunctorThrowOverload::CheckOverload());
  }

  TEST_METHOD(Functor_MoveOnlyLambda) {
    std::unique_ptr<int> uniquePtr = std::make_unique<int>(123450);
    auto lambda = [capturedUniquePtr = std::move(uniquePtr)](int x) noexcept {
      return *capturedUniquePtr + x;
    };

    Mso::Functor<int(int)> f1 = std::move(lambda);

    TestCheck(!static_cast<bool>(uniquePtr));
    TestCheckEqual(123456, f1(6));
  }

  TEST_METHOD(Functor_LambdaMoveOnlyParam) {
    std::unique_ptr<int> uniquePtr = std::make_unique<int>(123450);

    Mso::Functor<int(std::unique_ptr<int> &&)> f1 = [](std::unique_ptr<int> && uniquePtrParam) noexcept {
      std::unique_ptr<int> movedPtr = std::move(uniquePtrParam);
      return *movedPtr + 6;
    };

    TestCheckEqual(123456, f1(std::move(uniquePtr)));
    TestCheck(!static_cast<bool>(uniquePtr));
  }

  TEST_METHOD(Functor_LambdaCovarianceAndContravariance) {
    Derived derivedInitial;
    derivedInitial.i = 123456;

    // FunctorTestsCommon::DerivedFromBaseClass returns a Derived& and takes a BaseClass&.
    // Covariance of the return type means our functor can have a more generic return type
    // compared to the function itself. Contravariance of the parameters means that the
    // functor can have more specific parameter types compared to the function itself.

#ifdef JUST_FOR_ILLUSTATION__CPP_17
    // 'noexcept' isn't maintained in the function pointer type until C++17
    Mso::Functor<BaseClass &(Derived &)> f1{&DerivedFromBaseClass};
#else
    Mso::Functor<BaseClass &(Derived &)> f1(&DerivedFromBaseClass, Mso::TerminateOnException);
#endif

    BaseClass &baseClass = f1(derivedInitial);

    TestCheck(static_cast<BaseClass *>(&derivedInitial) != &baseClass); // Make sure we got a different object back
    TestCheckEqual(baseClass.GetInt(), 123456);
  }

  OACR_WARNING_PUSH
  OACR_WARNING_DISABLE(NOEXCEPT_FUNC_THROWS, "std::terminate behavior is okay for unit test")
  TEST_METHOD(Functor_FunctorMakesCopyPassesRefToLambda) {
    std::string str = "Hello, world!";
    bool stringMatched = false;
    bool gotCopy = false;

    Mso::Functor<void(std::string)> f1 = [&](const std::string &strParam) noexcept {
      stringMatched = (strParam == "Hello, world!");
      gotCopy = (static_cast<const void *>(&str) != static_cast<const void *>(&strParam));
    };

    f1(str);
    TestCheck(stringMatched);
    TestCheck(gotCopy);
  }
  OACR_WARNING_POP

  TEST_METHOD(Functor_FromFunctionPtr_Void) {
    g_freeFunctionState = 0;

    Mso::VoidFunctor func = Mso::VoidFunctor::FromFunctionPtr<&TestFreeFunction_Increment>();
    TestCheckEqual(g_freeFunctionState, 0);
    func();
    TestCheckEqual(g_freeFunctionState, 1);
    func();
    TestCheckEqual(g_freeFunctionState, 2);

    func = Mso::VoidFunctor::FromFunctionPtr<&TestFreeFunction_Decrement>();
    func();
    TestCheckEqual(g_freeFunctionState, 1);
    func();
    TestCheckEqual(g_freeFunctionState, 0);
  }

  TEST_METHOD(Functor_FromFunctionPtr) {
    g_freeFunctionState = 0;

    Mso::Functor<int(int)> func = Mso::Functor<int(int)>::FromFunctionPtr<&TestFreeFunction_Add>();
    TestCheckEqual(func(5), 5);
    TestCheckEqual(func(3), 8);

    func = Mso::Functor<int(int)>::FromFunctionPtr<&TestFreeFunction_Subtract>();
    TestCheckEqual(func(2), 6);
    TestCheckEqual(func(6), 0);
  }

  TEST_METHOD(FunctorThrow_FromFunctionPtr_ThrowingFunction) {
    Mso::FunctorThrow<void(int)> func = Mso::FunctorThrow<void(int)>::FromFunctionPtr<&TestFreeFunction_Throw>();

    func(1); // should not throw

    TestCheckException(std::runtime_error, func(-1));
  }
};

TestClassComponent(FunctorNoexceptTest, Mso.Functor.Noexcept) TEST_CLASS (FunctorNoexceptTest) {
  TEST_METHOD(Functor_noexcept_ctor_Default) {
    Mso::Functor<int(int, int) noexcept> f1;
    TestCheck(f1.IsEmpty());
  }

  TEST_METHOD(Functor_noexcept_ctor_nullptr) {
    Mso::Functor<int(int, int) noexcept> f1(nullptr);
    TestCheck(f1.IsEmpty());
  }

  TEST_METHOD(Functor_noexcept_ctor_Copy) {
    Mso::Functor<int(int, int) noexcept> f1 = [](int x, int y) noexcept {
      return x + y;
    };
    Mso::Functor<int(int, int) noexcept> f2(f1);
    TestCheck(!f1.IsEmpty());
    TestCheck(f1 == f2);
  }

  TEST_METHOD(Functor_noexcept_ctor_Move) {
    Mso::Functor<int(int, int) noexcept> f1 = [](int x, int y) noexcept {
      return x + y;
    };
    Mso::Functor<int(int, int) noexcept> f2(f1); // copy of f1
    Mso::Functor<int(int, int) noexcept> f3(std::move(f1));
    TestCheck(f1.IsEmpty());
    TestCheck(!f3.IsEmpty());
    TestCheck(f3 == f2);
  }

  TEST_METHOD(Functor_noexcept_ctor_DoNothingFunctor) {
    auto f1 = Mso::Functor<int(int, int) noexcept>::DoNothing();
    auto f2 = Mso::Functor<int(int, int) noexcept>::DoNothing();
    TestCheck(!f1.IsEmpty());
    TestCheck(f1.Get() == f2.Get());
  }

  TEST_METHOD(Functor_noexcept_ctor_IFunctor_Ptr) {
    auto functorImpl = Mso::Make<TestVoidFunctor>();
    Mso::VoidFunctor f1(functorImpl.Get());
    TestCheck(!f1.IsEmpty());
    TestCheck(f1.Get() == functorImpl.Get());
    f1();
    TestCheckEqual(1, functorImpl->m_callCount);
  }

  TEST_METHOD(Functor_noexcept_ctor_IFunctor_CntPtrCopy) {
    auto functorImpl = Mso::Make<TestVoidFunctor>();
    Mso::VoidFunctor f1(functorImpl);
    TestCheck(!f1.IsEmpty());
    TestCheck(f1.Get() == functorImpl.Get());
    f1();
    TestCheckEqual(1, functorImpl->m_callCount);
  }

  TEST_METHOD(Functor_noexcept_ctor_IFunctor_ConstCntPtrCopy) {
    const auto functorImpl = Mso::Make<TestVoidFunctor>();
    Mso::VoidFunctor f1(functorImpl);
    TestCheck(!f1.IsEmpty());
    TestCheck(f1.Get() == functorImpl.Get());
    f1();
    TestCheckEqual(1, functorImpl->m_callCount);
  }

  TEST_METHOD(Functor_noexcept_ctor_IFunctor_CntPtrMove) {
    auto functorImpl = Mso::Make<TestVoidFunctor>();
    Mso::CntPtr<TestVoidFunctor> functorImplCopy = functorImpl; // keep original to check call count
    Mso::VoidFunctor f1(std::move(functorImplCopy));
    TestCheck(!f1.IsEmpty());
    TestCheck(functorImplCopy.IsEmpty());
    TestCheck(f1.Get() == functorImpl.Get());
    f1();
    TestCheckEqual(1, functorImpl->m_callCount);
  }

  TEST_METHOD(Functor_noexcept_ctor_IFunctor_Ptr_PassToMethod) {
    auto functorImpl = Mso::Make<TestVoidFunctor>();
    VoidFunctorExecutor::Execute(functorImpl.Get());

    TestCheckEqual(1, functorImpl->m_callCount);
  }

  TEST_METHOD(Functor_noexcept_ctor_IFunctor_CntPtrCopy_PassToMethod) {
    auto functorImpl = Mso::Make<TestVoidFunctor>();
    VoidFunctorExecutor::Execute(functorImpl);

    TestCheckEqual(1, functorImpl->m_callCount);
  }

  TEST_METHOD(Functor_noexcept_ctor_IFunctor_ConstCntPtrCopy_PassToMethod) {
    const auto functorImpl = Mso::Make<TestVoidFunctor>();
    VoidFunctorExecutor::Execute(functorImpl);

    TestCheckEqual(1, functorImpl->m_callCount);
  }

  TEST_METHOD(Functor_noexcept_ctor_IFunctor_CntPtrMove_PassToMethod) {
    auto functorImpl = Mso::Make<TestVoidFunctor>();
    Mso::CntPtr<TestVoidFunctor> functorImplCopy = functorImpl; // keep original to check call count
    VoidFunctorExecutor::Execute(std::move(functorImplCopy));

    TestCheckEqual(1, functorImpl->m_callCount);
  }

  TEST_METHOD(Functor_noexcept_ctor_Lambda) {
    Mso::Functor<int(int, int) noexcept> f1 = [](int x, int y) noexcept {
      return x + y;
    };
    TestCheck(!f1.IsEmpty());
    TestCheckEqual(5, f1(2, 3));
  }

  TEST_METHOD(Functor_noexcept_ctor_Lambda_copy) {
    auto lambda = [](int x, int y) noexcept {
      return x + y;
    };
    Mso::Functor<int(int, int) noexcept> f1(lambda);
    TestCheck(!f1.IsEmpty());
    TestCheckEqual(5, f1(2, 3));
  }

  TEST_METHOD(Functor_noexcept_ctor_Lambda_move) {
    auto lambda = [](int x, int y) noexcept {
      return x + y;
    };
    Mso::Functor<int(int, int) noexcept> f1(std::move(lambda));
    TestCheck(!f1.IsEmpty());
    TestCheckEqual(5, f1(2, 3));
  }

  TEST_METHOD(Functor_noexcept_ctor_Lambda_CaptureCopyCount) {
    int addRefCalls = 0;
    int releaseCalls = 0;
    {
      auto data = Mso::Make<TestData>(addRefCalls, releaseCalls);
      data->Value = 3;
      Mso::Functor<int(int, int) noexcept> f1 = ([data](int x, int y) noexcept { return x + y + data->Value; });

      TestCheckEqual(13, f1(4, 6));
    }

    TestCheckEqual(1, addRefCalls);
    TestCheckEqual(2, releaseCalls);
  }

  TEST_METHOD(Functor_noexcept_ctor_Lambda_MoveCapture) {
    int addRefCalls = 0;
    int releaseCalls = 0;
    {
      auto data = Mso::Make<TestData>(addRefCalls, releaseCalls);
      data->Value = 3;
      Mso::Functor<int(int, int) noexcept> f1 = [movedData = std::move(data)](int x, int y) noexcept {
        return x + y + movedData->Value;
      };

      TestCheckEqual(13, f1(4, 6));
    }

    TestCheckEqual(0, addRefCalls);
    TestCheckEqual(1, releaseCalls);
  }

  TEST_METHOD(Functor_noexcept_ctor_StaticMethod) {
    Mso::Functor<int(int, int) noexcept> f1{&StaticMethod::Add, Mso::TerminateOnException};
    TestCheck(!f1.IsEmpty());
    TestCheckEqual(5, f1(2, 3));
  }

  TEST_METHOD(Functor_noexcept_ctor_StdFunctionCopy) {
    int callCount = 0;
    int value = 0;
    auto func = std::function<int(int)>([&callCount, &value ](int v) noexcept {
      ++callCount;
      value = v;
      return v;
    });
    Mso::Functor<int(int) noexcept> f1{func, Mso::TerminateOnException};
    TestCheckEqual(5, f1(5));
    TestCheckEqual(1, callCount);
    TestCheckEqual(5, value);
  }

  TEST_METHOD(Functor_noexcept_ctor_ConstStdFunctionCopy) {
    int callCount = 0;
    int value = 0;
    const auto func = std::function<int(int)>([&callCount, &value ](int v) noexcept {
      ++callCount;
      value = v;
      return v;
    });
    Mso::Functor<int(int) noexcept> f1{func, Mso::TerminateOnException};
    TestCheckEqual(5, f1(5));
    TestCheckEqual(1, callCount);
    TestCheckEqual(5, value);
  }

  TEST_METHOD(Functor_noexcept_ctor_StdFunctionMove) {
    int callCount = 0;
    int value = 0;
    auto func = std::function<int(int)>([&callCount, &value ](int v) noexcept {
      ++callCount;
      value = v;
      return v;
    });
    Mso::Functor<int(int) noexcept> f1{std::move(func), Mso::TerminateOnException};
    TestCheckEqual(5, f1(5));
    TestCheckEqual(1, callCount);
    TestCheckEqual(5, value);
  }

  TEST_METHOD(Functor_noexcept_ctor_StdFunctionCopy_PassToMethod) {
    int callCount = 0;
    auto func = std::function<void()>([&callCount]() noexcept { ++callCount; });
    VoidFunctorExecutor::Execute({func, Mso::TerminateOnException});
    TestCheckEqual(1, callCount);
  }

  TEST_METHOD(Functor_noexcept_ctor_ConstStdFunctionCopy_PassToMethod) {
    int callCount = 0;
    const auto func = std::function<void()>([&callCount]() noexcept { ++callCount; });
    VoidFunctorExecutor::Execute({func, Mso::TerminateOnException});
    TestCheckEqual(1, callCount);
  }

  TEST_METHOD(Functor_noexcept_ctor_StdFunctionMove_PassToMethod) {
    int callCount = 0;
    auto func = std::function<void()>([&callCount]() noexcept { ++callCount; });
    VoidFunctorExecutor::Execute({std::move(func), Mso::TerminateOnException});
    TestCheckEqual(1, callCount);
  }

  TEST_METHOD(Functor_noexcept_ctor_StdFunctionCopy_DifferentSignature_PassToMethod) {
    int callCount = 0;
    auto func =
        std::function<void(Mso::CntPtr<StructParam>)>([&callCount](Mso::CntPtr<StructParam>) noexcept { ++callCount; });
    MyFunctorExecutor::Execute({func, Mso::TerminateOnException});
    TestCheckEqual(1, callCount);
  }

  TEST_METHOD(Functor_noexcept_ctor_ConstStdFunctionCopy_DifferentSignature_PassToMethod) {
    int callCount = 0;
    const auto func =
        std::function<void(Mso::CntPtr<StructParam>)>([&callCount](Mso::CntPtr<StructParam>) noexcept { ++callCount; });
    MyFunctorExecutor::Execute({func, Mso::TerminateOnException});
    TestCheckEqual(1, callCount);
  }

  TEST_METHOD(Functor_noexcept_ctor_StdFunctionMove_DifferentSignature_PassToMethod) {
    int callCount = 0;
    auto func =
        std::function<void(Mso::CntPtr<StructParam>)>([&callCount](Mso::CntPtr<StructParam>) noexcept { ++callCount; });
    MyFunctorExecutor::Execute({std::move(func), Mso::TerminateOnException});
    TestCheckEqual(1, callCount);
  }

  TEST_METHOD(Functor_noexcept_Assign_nullptr) {
    Mso::Functor<int(int, int) noexcept> f1 = [](int x, int y) noexcept {
      return x + y;
    };
    f1 = nullptr;
    TestCheck(f1.IsEmpty());
  }

  TEST_METHOD(Functor_noexcept_Assign_Copy) {
    Mso::Functor<int(int, int) noexcept> f1 = [](int x, int y) noexcept {
      return x + y;
    };
    Mso::Functor<int(int, int) noexcept> f2 = [](int x, int y) noexcept {
      return x - y;
    };
    TestCheck(f1 != f2);
    Mso::Functor<int(int, int) noexcept> f3(f1); // copy of f1
    f1 = f2;
    TestCheck(!f2.IsEmpty());
    TestCheck(f1 == f2);
    TestCheck(f1 != f3);
  }

  TEST_METHOD(Functor_noexcept_Assign_Move) {
    Mso::Functor<int(int, int) noexcept> f1 = [](int x, int y) noexcept {
      return x + y;
    };
    Mso::Functor<int(int, int) noexcept> f2 = [](int x, int y) noexcept {
      return x - y;
    };
    Mso::Functor<int(int, int) noexcept> f3(f1); // copy of f1
    Mso::Functor<int(int, int) noexcept> f4(f2); // copy of f2
    f1 = std::move(f2);
    TestCheck(!f1.IsEmpty());
    TestCheck(f2.IsEmpty());
    TestCheck(f1 != f3);
    TestCheck(f1 == f4);
  }

  TESTMETHOD_REQUIRES_SEH(Functor_operator_call_Empty) {
    Mso::Functor<int(int, int) noexcept> f1;
    TestCheckCrash(f1(1, 2));
  }

  TEST_METHOD(Functor_noexcept_operator_call_NonEmpty) {
    Mso::Functor<int(int, int) noexcept> f1 = ([](int x, int y) noexcept { return x + y; });
    TestCheckEqual(10, f1(4, 6));
  }

  TESTMETHOD_REQUIRES_SEH(Functor_operator_call_Throws) {
    Mso::Functor<int(int) noexcept> f1 = ([](int) noexcept->int {
      OACR_NOEXCEPT_MAYTERMINATE;
#pragma warning(suppress : 4297) // Suppress warning about throwing in noexcept function.
      throw std::runtime_error("Test error");
    });
    TestCheckTerminate(f1(5));
  }

  TESTMETHOD_REQUIRES_SEH(Functor_operator_call_StdFunction_Throws) {
    int callCount = 0;
    auto func = std::function<int(int)>([&callCount](int) -> int {
      ++callCount;
      throw std::runtime_error("Test error");
    });
    Mso::Functor<int(int) noexcept> f1{std::move(func), Mso::TerminateOnException};
    TestCheckTerminate(f1(5));
    TestCheckEqual(1, callCount);
  }

  TEST_METHOD(Functor_noexcept_operator_call_Void) {
    int result = 0;
    Mso::Functor<void(int, int) noexcept> f1 = [&result](int x, int y) noexcept {
      result = x + y;
    };
    TestCheck(!f1.IsEmpty());
    f1(4, 6);
    TestCheckEqual(10, result);
  }

  TEST_METHOD(Functor_noexcept_operator_call_WithCapture) {
    int bias = 5;
    auto add = Mso::Functor<int(int, int) noexcept>([bias](int x, int y) noexcept { return x + y + bias; });
    TestCheckEqual(15, add(4, 6));
  }

  TEST_METHOD(Functor_noexcept_IsEmpty) {
    Mso::Functor<int(int, int) noexcept> f1 = [](int x, int y) noexcept {
      return x + y;
    };
    Mso::Functor<int(int, int) noexcept> f2;
    TestCheck(!f1.IsEmpty());
    TestCheck(f2.IsEmpty());
  }

  TEST_METHOD(Functor_noexcept_operator_bool) {
    Mso::Functor<int(int, int) noexcept> f1 = [](int x, int y) noexcept {
      return x + y;
    };
    Mso::Functor<int(int, int) noexcept> f2;
    TestCheck(f1);
    TestCheck(!f2);
  }

  TEST_METHOD(Functor_noexcept_Swap) {
    Mso::Functor<int(int, int) noexcept> f1 = [](int x, int y) noexcept {
      return x + y;
    };
    Mso::Functor<int(int, int) noexcept> f2 = [](int x, int y) noexcept {
      return x - y;
    };
    Mso::Functor<int(int, int) noexcept> f3(f1); // copy of f1
    Mso::Functor<int(int, int) noexcept> f4(f2); // copy of f2
    f1.Swap(f2);
    TestCheck(f1 == f4);
    TestCheck(f2 == f3);
  }

  TEST_METHOD(Functor_noexcept_std_swap) {
    using std::swap; // The typical pattern how to call the swap method.
    Mso::Functor<int(int, int) noexcept> f1 = [](int x, int y) noexcept {
      return x + y;
    };
    Mso::Functor<int(int, int) noexcept> f2 = [](int x, int y) noexcept {
      return x - y;
    };
    Mso::Functor<int(int, int) noexcept> f3(f1); // copy of f1
    Mso::Functor<int(int, int) noexcept> f4(f2); // copy of f2
    swap(f1, f2); // Never use the std prefix for swap: the swap can be overridden in the namespace of T.
    TestCheck(f1 == f4);
    TestCheck(f2 == f3);
  }

  TEST_METHOD(Functor_noexcept_Get) {
    Mso::Functor<int(int, int) noexcept> f1 = [](int x, int y) noexcept {
      return x + y;
    };
    Mso::Functor<int(int, int) noexcept> f2;
    TestCheck(f1.Get() != nullptr);
    TestCheck(f2.Get() == nullptr);
  }

  TEST_METHOD(Functor_noexcept_Detach) {
    Mso::Functor<int(int, int) noexcept> f1 = [](int x, int y) noexcept {
      return x + y;
    };

    Mso::CntPtr<Mso::IFunctor<int, int, int>> impl{f1.Detach(), Mso::AttachTag};
    TestCheck(f1.IsEmpty());
    TestCheck(!impl.IsEmpty());
  }

  TEST_METHOD(Functor_noexcept_operator_Equal) {
    using std::swap; // The typical pattern how to call the swap method.
    Mso::Functor<int(int, int) noexcept> f1 = [](int x, int y) noexcept {
      return x + y;
    };
    Mso::Functor<int(int, int) noexcept> f2 = [](int x, int y) noexcept {
      return x - y;
    };
    Mso::Functor<int(int, int) noexcept> f3(f1); // copy of f1
    Mso::Functor<int(int, int) noexcept> f4;
    TestCheck(f1 == f3);
    TestCheck(f3 == f1);
    TestCheck(f1 != f2);
    TestCheck(f2 != f1);
    TestCheck(f4 != f1);
    TestCheck(f1 != f4);
    TestCheck(f4 == nullptr);
    TestCheck(nullptr == f4);
    TestCheck(f1 != nullptr);
    TestCheck(nullptr != f1);
  }

  TEST_METHOD(Functor_noexcept_DoNothing) {
    Mso::Functor<int(int) noexcept> f1 = Mso::Functor<int(int) noexcept>::DoNothing();
    TestCheck(!f1.IsEmpty());
    TestCheckEqual(0, f1(1));

    Mso::Functor<double(double) noexcept> f2 = Mso::Functor<double(double) noexcept>::DoNothing();
    TestCheck(!f2.IsEmpty());
    TestCheckEqual(0.0, f2(3.14));

    Mso::Functor<int(int, int) noexcept> f3 = Mso::Functor<int(int, int) noexcept>::DoNothing();
    TestCheck(!f3.IsEmpty());
    TestCheckEqual(0, f3(100, 200));

    Mso::Functor<int *(int, int)noexcept> f4 = Mso::Functor<int *(int, int)noexcept>::DoNothing();
    TestCheck(!f4.IsEmpty());
    TestCheckEqual(nullptr, f4(1000, 2000));
  }

  // Make sure that lambda can be matched to a correct Mso::Functor instantiation
  struct CheckFunctorOverload final {
    using FunctorAlias1 = Mso::Functor<void(int &) noexcept>;
    using FunctorAlias2 = Mso::Functor<void(int &, bool &) noexcept>;

    static int DoCheckOverload(const FunctorAlias1 &) noexcept {
      return 1;
    }
    static int DoCheckOverload(const FunctorAlias2 &) noexcept {
      return 2;
    }

    static int CheckOverload() noexcept {
      return DoCheckOverload([&](auto &) noexcept {}) + DoCheckOverload([&](auto &, auto &) noexcept {});
    }
  };

  TEST_METHOD(Functor_noexcept_CheckOverload) {
    TestCheckEqual(3, CheckFunctorOverload::CheckOverload());
  }
};

} // namespace FunctionalTests
