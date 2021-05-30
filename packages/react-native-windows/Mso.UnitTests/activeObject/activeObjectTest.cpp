// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "activeObject/activeObject.h"
#include <memory>
#include "eventWaitHandle/eventWaitHandle.h"
#include "functional/functorRef.h"
#include "motifCpp/libletAwareMemLeakDetection.h"
#include "motifCpp/testCheck.h"
#include "object/refCountedObject.h"

OACR_WARNING_PUSH
OACR_WARNING_DISABLE(NOEXCEPT_FUNC_THROWS, "Allow to crash in noexcept functions in unit tests.")

namespace ActiveObjectTests {

namespace {

struct CallableVoidFunctor : Mso::UnknownObject<Mso::RefCountStrategy::SimpleNoQuery, Mso::IVoidFunctor> {
  CallableVoidFunctor(bool shouldBeCalled, char const *name) noexcept
      : m_shouldBeCalled{shouldBeCalled}, m_name{name} {}

  ~CallableVoidFunctor() noexcept override {
    TestCheckEqual(m_isCalled, m_shouldBeCalled);
  }

  void Invoke() noexcept override {
    m_isCalled = true;
  }

 private:
  bool m_shouldBeCalled{false};
  bool m_isCalled{false};
  char const *m_name{nullptr};
};

struct EqualQueueFunctor : CallableVoidFunctor {
  EqualQueueFunctor(Mso::DispatchQueue const &queue, char const *name) noexcept
      : CallableVoidFunctor{/*shouldBeCalled:*/ true, name}, m_queue{queue} {}

  void Invoke() noexcept override {
    TestCheck(m_queue == Mso::DispatchQueue::CurrentQueue());
    CallableVoidFunctor::Invoke();
  }

 private:
  Mso::DispatchQueue const &m_queue;
};

struct NotEqualQueueFunctor : CallableVoidFunctor {
  NotEqualQueueFunctor(Mso::DispatchQueue const &queue, char const *name) noexcept
      : CallableVoidFunctor{/*shouldBeCalled:*/ true, name}, m_queue{queue} {}

  void Invoke() noexcept override {
    TestCheck(m_queue != Mso::DispatchQueue::CurrentQueue());
    CallableVoidFunctor::Invoke();
  }

 private:
  Mso::DispatchQueue const &m_queue;
};

struct NotCallableVoidFunctor : CallableVoidFunctor {
  NotCallableVoidFunctor(const char *name) noexcept : CallableVoidFunctor{/*shouldBeCalled:*/ false, name} {}
};

enum class TestExpectations : uint32_t {
  CtorInQueue = 0b00'00'00'01,
  CtorNotInQueue = 0b00'00'00'10,
  CtorNotCalled = 0b00'00'00'11,
  InitializeInQueue = 0b00'00'01'00,
  InitializeNotInQueue = 0b00'00'10'00,
  InitializeNotCalled = 0b00'00'11'00,
  FinalizeInQueue = 0b00'01'00'00,
  FinalizeNotInQueue = 0b00'10'00'00,
  FinalizeNotCalled = 0b00'11'00'00,
  DtorInQueue = 0b01'00'00'00,
  DtorNotInQueue = 0b10'00'00'00,
  DtorNotCalled = 0b11'00'00'00,
};

static TestExpectations operator|(TestExpectations left, TestExpectations right) noexcept {
  return static_cast<TestExpectations>(static_cast<uint32_t>(left) | static_cast<uint32_t>(right));
}

static TestExpectations operator&(TestExpectations left, TestExpectations right) noexcept {
  return static_cast<TestExpectations>(static_cast<uint32_t>(left) & static_cast<uint32_t>(right));
}

struct Ctor {
  static constexpr TestExpectations InQueue = TestExpectations::CtorInQueue;
  static constexpr TestExpectations NotInQueue = TestExpectations::CtorNotInQueue;
  static constexpr TestExpectations NotCalled = TestExpectations::CtorNotCalled;
};

struct Initialize {
  static constexpr TestExpectations InQueue = TestExpectations::InitializeInQueue;
  static constexpr TestExpectations NotCalled = TestExpectations::InitializeNotCalled;
};

struct Finalize {
  static constexpr TestExpectations InQueue = TestExpectations::FinalizeInQueue;
  static constexpr TestExpectations NotCalled = TestExpectations::FinalizeNotCalled;
};

struct Dtor {
  static constexpr TestExpectations InQueue = TestExpectations::DtorInQueue;
  static constexpr TestExpectations NotInQueue = TestExpectations::DtorNotInQueue;
  static constexpr TestExpectations NotCalled = TestExpectations::DtorNotCalled;
};

struct TestContext {
  TestContext(
      TestExpectations expectations = Ctor::NotCalled | Initialize::NotCalled | Finalize::NotCalled |
          Dtor::NotCalled) noexcept {
    switch (expectations & TestExpectations::CtorNotCalled) {
      case TestExpectations::CtorInQueue:
        OnCtor = CheckEqualQueue("Ctor");
        break;
      case TestExpectations::CtorNotInQueue:
        OnCtor = CheckNotEqualQueue("Ctor");
        break;
      default:
        OnCtor = CheckNotCalled("Ctor");
        break;
    }

    switch (expectations & TestExpectations::InitializeNotCalled) {
      case TestExpectations::InitializeInQueue:
        OnInitialize = CheckEqualQueue("Initialize");
        break;
      case TestExpectations::InitializeNotInQueue:
        OnInitialize = CheckNotEqualQueue("Initialize");
        break;
      default:
        OnInitialize = CheckNotCalled("Initialize");
        break;
    }

    switch (expectations & TestExpectations::FinalizeNotCalled) {
      case TestExpectations::FinalizeInQueue:
        OnFinalize = CheckEqualQueue("Finalize");
        break;
      case TestExpectations::FinalizeNotInQueue:
        OnFinalize = CheckNotEqualQueue("Finalize");
        break;
      default:
        OnFinalize = CheckNotCalled("Finalize");
        break;
    }

    switch (expectations & TestExpectations::DtorNotCalled) {
      case TestExpectations::DtorInQueue:
        OnDtor = CheckEqualQueue("Dtor");
        break;
      case TestExpectations::DtorNotInQueue:
        OnDtor = CheckNotEqualQueue("Dtor");
        break;
      default:
        OnDtor = CheckNotCalled("Dtor");
        break;
    }
  }

  ~TestContext() noexcept {
    WaitSync();
    m_queue.AwaitTermination();
  }

  static void Invoke(const TestContext &context, Mso::VoidFunctor TestContext::*contextMethod) noexcept {
    if (context.*contextMethod) {
      (context.*contextMethod)();
    }
  }

  Mso::VoidFunctor CheckEqualQueue(const char *name) noexcept {
    return Mso::Make<EqualQueueFunctor, Mso::IVoidFunctor>(m_queue, name);
  }

  Mso::VoidFunctor CheckNotEqualQueue(const char *name) noexcept {
    return Mso::Make<NotEqualQueueFunctor, Mso::IVoidFunctor>(m_queue, name);
  }

  static Mso::VoidFunctor CheckNotCalled(const char *name) noexcept {
    return Mso::Make<NotCallableVoidFunctor, Mso::IVoidFunctor>(name);
  }

  Mso::DispatchQueue const &Queue() noexcept {
    return m_queue;
  }

  void WaitSync() noexcept {
    Mso::ManualResetEvent waiter;
    m_queue.Post(Mso::MakeDispatchCleanupTask([waiter]() noexcept { waiter.Set(); }));
    waiter.Wait();
  }

  void ShutdownQueue() noexcept {
    m_queue.Shutdown(Mso::PendingTaskAction::Cancel);
  }

  Mso::VoidFunctor OnCtor;
  Mso::VoidFunctor OnInitialize;
  Mso::VoidFunctor OnFinalize;
  Mso::VoidFunctor OnDtor;

 private:
  Mso::DispatchQueue m_queue;
};

MSO_STRUCT_GUID(ITestObject, "37dd170d-42ce-4508-bc1a-abf50c19c572")
struct ITestObject {};

struct TestObject : Mso::ActiveObject<ITestObject> {
  using Super = ActiveObjectType;

  TestObject(TestContext &context) noexcept : Super{context.Queue()}, m_context{context} {
    TestContext::Invoke(m_context, &TestContext::OnCtor);
  }

  ~TestObject() noexcept override {
    TestContext::Invoke(m_context, &TestContext::OnDtor);
  }

  using Super::InvokeInQueue;
  using Super::InvokeInQueueStrong;
  using Super::IsInitialized;
  using Super::IsInQueue;
  using Super::Queue;
  using Super::VerifyIsInQueueElseCrash;

 protected:
  TestContext &m_context;
};

struct TestObjectWithInitialize : TestObject {
  using Super = TestObject;

  TestObjectWithInitialize(TestContext &context) noexcept : Super{context} {}

  void Initialize() noexcept override {
    TestContext::Invoke(m_context, &TestContext::OnInitialize);
  }
};

struct TestObjectWithFinalize : TestObject {
  using Super = TestObject;

  TestObjectWithFinalize(TestContext &context) noexcept : Super{context} {}

  void Finalize() noexcept override {
    TestContext::Invoke(m_context, &TestContext::OnFinalize);
  }
};

struct TestObjectWithInitializeAndFinalize : TestObject {
  using Super = TestObject;

  TestObjectWithInitializeAndFinalize(TestContext &context) noexcept : Super{context} {}

  void Initialize() noexcept override {
    TestContext::Invoke(m_context, &TestContext::OnInitialize);
  }

  void Finalize() noexcept override {
    TestContext::Invoke(m_context, &TestContext::OnFinalize);
  }
};

} // namespace

TEST_CLASS_EX (ActiveObjectTest, LibletAwareMemLeakDetection) {
  TEST_METHOD(ActiveObject_CreateInQueue) {
    TestContext context{Ctor::InQueue | Initialize::NotCalled | Finalize::NotCalled | Dtor::InQueue};

    Mso::CntPtr<TestObject> obj;
    context.Queue().Post([&]() noexcept { obj = Mso::Make<TestObject>(context); });
    context.Queue().Post([&]() noexcept { obj = nullptr; });

    context.WaitSync();
  }

  TEST_METHOD(ActiveObject_CreateInQueue_WithInitialize) {
    TestContext context{Ctor::InQueue | Initialize::InQueue | Finalize::NotCalled | Dtor::InQueue};

    Mso::CntPtr<TestObjectWithInitialize> obj;
    context.Queue().Post([&]() noexcept { obj = Mso::Make<TestObjectWithInitialize>(context); });
    context.Queue().Post([&]() noexcept { obj = nullptr; });

    context.WaitSync();
  }

  TEST_METHOD(ActiveObject_CreateInQueue_WithFinalize) {
    TestContext context{Ctor::InQueue | Initialize::NotCalled | Finalize::InQueue | Dtor::InQueue};

    Mso::CntPtr<TestObjectWithFinalize> obj;
    context.Queue().Post([&]() noexcept { obj = Mso::Make<TestObjectWithFinalize>(context); });
    context.Queue().Post([&]() noexcept { obj = nullptr; });

    context.WaitSync();
  }

  TEST_METHOD(ActiveObject_CreateInQueue_WithInitializeAndFinalize) {
    TestContext context{Ctor::InQueue | Initialize::InQueue | Finalize::InQueue | Dtor::InQueue};

    Mso::CntPtr<TestObjectWithInitializeAndFinalize> obj;
    context.Queue().Post([&]() noexcept { obj = Mso::Make<TestObjectWithInitializeAndFinalize>(context); });
    context.Queue().Post([&]() noexcept { obj = nullptr; });

    context.WaitSync();
  }

  TEST_METHOD(ActiveObject_CreateOutsideQueue) {
    TestContext context{Ctor::NotInQueue | Initialize::NotCalled | Finalize::NotCalled | Dtor::InQueue};

    auto obj = Mso::Make<TestObject>(context);
    context.Queue().Post([&]() noexcept { obj = nullptr; });

    context.WaitSync();
  }

  TEST_METHOD(ActiveObject_CreateOutsideQueue_WithInitialize) {
    TestContext context{Ctor::NotInQueue | Initialize::InQueue | Finalize::NotCalled | Dtor::InQueue};

    auto obj = Mso::Make<TestObjectWithInitialize>(context);
    context.Queue().Post([&]() noexcept { obj = nullptr; });

    context.WaitSync();
  }

  TEST_METHOD(ActiveObject_CreateOutsideQueue_WithFinalize) {
    TestContext context{Ctor::NotInQueue | Initialize::NotCalled | Finalize::InQueue | Dtor::InQueue};

    auto obj = Mso::Make<TestObjectWithFinalize>(context);
    context.Queue().Post([&]() noexcept { obj = nullptr; });

    context.WaitSync();
  }

  TEST_METHOD(ActiveObject_CreateOutsideQueue_WithInitializeAndFinalize) {
    TestContext context{Ctor::NotInQueue | Initialize::InQueue | Finalize::InQueue | Dtor::InQueue};

    auto obj = Mso::Make<TestObjectWithInitializeAndFinalize>(context);
    context.Queue().Post([&]() noexcept { obj = nullptr; });

    context.WaitSync();
  }

  TEST_METHOD(ActiveObject_CreateInQueueDestroyOutsideQueue) {
    TestContext context{Ctor::InQueue | Initialize::NotCalled | Finalize::NotCalled | Dtor::NotInQueue};

    Mso::CntPtr<TestObject> obj;
    context.Queue().Post([&]() noexcept { obj = Mso::Make<TestObject>(context); });
    obj = nullptr;

    context.WaitSync();
  }

  TEST_METHOD(ActiveObject_CreateInQueueDestroyOutsideQueue_WithInitialize) {
    TestContext context{Ctor::InQueue | Initialize::InQueue | Finalize::NotCalled | Dtor::NotInQueue};

    Mso::CntPtr<TestObjectWithInitialize> obj;
    context.Queue().Post([&]() noexcept { obj = Mso::Make<TestObjectWithInitialize>(context); });
    context.WaitSync();
    obj = nullptr;

    context.WaitSync();
  }

  TEST_METHOD(ActiveObject_CreateInQueueDestroyOutsideQueue_WithFinalize) {
    TestContext context{Ctor::InQueue | Initialize::NotCalled | Finalize::InQueue | Dtor::InQueue};

    Mso::CntPtr<TestObjectWithFinalize> obj;
    context.Queue().Post([&]() noexcept { obj = Mso::Make<TestObjectWithFinalize>(context); });
    context.WaitSync();
    obj = nullptr;

    context.WaitSync();
  }

  TEST_METHOD(ActiveObject_CreateInQueueDestroyOutsideQueue_WithInitializeAndFinalize) {
    TestContext context{Ctor::InQueue | Initialize::InQueue | Finalize::InQueue | Dtor::InQueue};

    Mso::CntPtr<TestObjectWithInitializeAndFinalize> obj;
    context.Queue().Post([&]() noexcept { obj = Mso::Make<TestObjectWithInitializeAndFinalize>(context); });
    context.WaitSync();
    obj = nullptr;

    context.WaitSync();
  }

  TEST_METHOD(ActiveObject_CreateOutsidQueueeDestroyOutsideQueue) {
    TestContext context{Ctor::NotInQueue | Initialize::NotCalled | Finalize::NotCalled | Dtor::NotInQueue};

    auto obj = Mso::Make<TestObject>(context);
    obj = nullptr;

    context.WaitSync();
  }

  TEST_METHOD(ActiveObject_CreateOutsidQueueeDestroyOutsideQueue_WithInitialize) {
    TestContext context{Ctor::NotInQueue | Initialize::InQueue | Finalize::NotCalled | Dtor::NotInQueue};

    auto obj = Mso::Make<TestObjectWithInitialize>(context);
    context.WaitSync(); // Initialize method may be not called if object is destroyed too soon.
    obj = nullptr;

    context.WaitSync();
  }

  TEST_METHOD(ActiveObject_CreateOutsidQueueeDestroyOutsideQueue_WithFinalize) {
    TestContext context{Ctor::NotInQueue | Initialize::NotCalled | Finalize::InQueue | Dtor::InQueue};

    auto obj = Mso::Make<TestObjectWithFinalize>(context);
    obj = nullptr;

    context.WaitSync();
  }

  TEST_METHOD(ActiveObject_CreateOutsidQueueeDestroyOutsideQueue_WithInitializeAndFinalize) {
    TestContext context{Ctor::NotInQueue | Initialize::InQueue | Finalize::InQueue | Dtor::InQueue};

    auto obj = Mso::Make<TestObjectWithInitializeAndFinalize>(context);
    context.WaitSync(); // Initialize method may be not called if object is destroyed too soon.
    obj = nullptr;

    context.WaitSync();
  }

  TEST_METHOD(ActiveObject_Queue) {
    TestContext context{Ctor::NotInQueue | Initialize::NotCalled | Finalize::NotCalled | Dtor::NotInQueue};

    auto obj = Mso::Make<TestObject>(context);
    TestCheck(context.Queue() == obj->Queue());

    context.WaitSync();
  }

  TEST_METHOD(ActiveObject_InvokeInQueue) {
    TestContext context{Ctor::NotInQueue | Initialize::NotCalled | Finalize::NotCalled | Dtor::NotInQueue};

    auto obj = Mso::Make<TestObject>(context);
    obj->InvokeInQueue([&]() noexcept { TestCheck(context.Queue() == Mso::DispatchQueue::CurrentQueue()); });

    context.WaitSync();
  }

  TEST_METHOD(ActiveObject_InvokeInQueueStrong) {
    TestContext context{Ctor::NotInQueue | Initialize::NotCalled | Finalize::NotCalled | Dtor::NotInQueue};

    auto obj = Mso::Make<TestObject>(context);
    obj->InvokeInQueueStrong([&]() noexcept { TestCheck(context.Queue() == Mso::DispatchQueue::CurrentQueue()); });

    context.WaitSync();
  }

  TEST_METHOD(ActiveObject_IsInQueue) {
    TestContext context{Ctor::NotInQueue | Initialize::NotCalled | Finalize::NotCalled | Dtor::NotInQueue};

    auto obj = Mso::Make<TestObject>(context);
    TestCheck(!obj->IsInQueue());
    context.Queue().Post([&obj]() noexcept { TestCheck(obj->IsInQueue()); });
    obj->InvokeInQueue([&obj]() noexcept { TestCheck(obj->IsInQueue()); });

    context.WaitSync();
  }

  TESTMETHOD_REQUIRES_SEH(ActiveObject_VerifyIsInQueueElseCrash_Crash) {
    TEST_DISABLE_MEMORY_LEAK_DETECTION();
    TestContext context{Ctor::NotInQueue | Initialize::NotCalled | Finalize::NotCalled | Dtor::NotInQueue};
    auto obj = Mso::Make<TestObject>(context);
    TestCheckCrash(obj->VerifyIsInQueueElseCrash());
  }

  TEST_METHOD(ActiveObject_VerifyIsInQueueElseCrash_Success) {
    TestContext context{Ctor::NotInQueue | Initialize::NotCalled | Finalize::NotCalled | Dtor::NotInQueue};

    auto obj = Mso::Make<TestObject>(context);
    context.Queue().Post([&obj]() noexcept { obj->VerifyIsInQueueElseCrash(); });
    obj->InvokeInQueue([&obj]() noexcept { obj->VerifyIsInQueueElseCrash(); });

    context.WaitSync();
  }

  TEST_METHOD(ActiveObject_IsInitialized) {
    TestContext context{Ctor::NotInQueue | Initialize::NotCalled | Finalize::NotCalled | Dtor::NotInQueue};

    auto obj = Mso::Make<TestObject>(context);
    TestCheck(obj->IsInitialized());

    context.WaitSync();
  }

  TEST_METHOD(ActiveObject_IsInitialized_WithInitialize) {
    TestContext context{Ctor::NotInQueue | Initialize::InQueue | Finalize::NotCalled | Dtor::NotInQueue};

    auto obj = Mso::Make<TestObjectWithInitialize>(context);
    context.WaitSync(); // To wait till initialization completed in the queue.
    TestCheck(obj->IsInitialized());

    context.WaitSync();
  }

  TEST_METHOD(ActiveObject_IsInitialized_WithInitialize_Failed) {
    TestContext context{Ctor::NotInQueue | Initialize::NotCalled | Finalize::NotCalled | Dtor::NotInQueue};

    context.ShutdownQueue();
    auto obj = Mso::Make<TestObjectWithInitialize>(context);
    context.WaitSync(); // To wait till initialization would completed in the queue.
    TestCheck(!obj->IsInitialized()); // We did not initialize because queue is shutdown.

    context.WaitSync();
  }

  TEST_METHOD(ActiveObject_IsInitialized_WithInitializeAndFinalize_Failed) {
    // Do not call Finalize if not initialized
    TestContext context{Ctor::NotInQueue | Initialize::NotCalled | Finalize::NotCalled | Dtor::NotInQueue};

    context.ShutdownQueue();
    auto obj = Mso::Make<TestObjectWithInitializeAndFinalize>(context);
    context.WaitSync(); // To wait till initialization would completed in the queue.
    TestCheck(!obj->IsInitialized()); // We did not initialize because queue is shutdown.

    context.WaitSync();
  }

  TEST_METHOD(ActiveObject_QueueShutdownBeforeFinalize) {
    // Do not call Finalize if not initialized
    TestContext context{Ctor::NotInQueue | Initialize::NotCalled | Finalize::NotCalled | Dtor::NotInQueue};

    auto obj = Mso::Make<TestObjectWithInitializeAndFinalize>(context);
    context.ShutdownQueue();

    context.WaitSync();
  }

  TEST_METHOD(ActiveObject_SetOnDestructedOnce) {
    TestContext context{Ctor::NotInQueue | Initialize::NotCalled | Finalize::NotCalled | Dtor::NotInQueue};

    auto obj = Mso::Make<TestObject>(context);
    bool isDestructed{false};
    obj->SetOnDestructedOnce([&]() noexcept { isDestructed = true; });
    obj = nullptr;
    TestCheck(isDestructed);
  }

  TESTMETHOD_REQUIRES_SEH(ActiveObject_SetOnDestructedOnce_TwiceCrash) {
    TEST_DISABLE_MEMORY_LEAK_DETECTION();
    TestContext context{Ctor::NotInQueue | Initialize::NotCalled | Finalize::NotCalled | Dtor::NotInQueue};
    auto obj = Mso::Make<TestObject>(context);
    obj->SetOnDestructedOnce([]() noexcept {});
    TestCheckCrash(obj->SetOnDestructedOnce([]() noexcept {}));
  }

  TEST_METHOD(ActiveField_DestructorNoCrash) {
    TestContext context;
    Mso::ActiveField<std::unique_ptr<int>> field{context.Queue()};
  }

  TESTMETHOD_REQUIRES_SEH(ActiveField_DefaultLoadCrash) {
    TEST_DISABLE_MEMORY_LEAK_DETECTION();
    TestContext context;
    auto field = std::make_unique<Mso::ActiveField<std::unique_ptr<int>>>(context.Queue());
    TestCheckCrash(field->Load());
    context.Queue().Post([&field]() noexcept { field = nullptr; });
    context.WaitSync();
  }

  TEST_METHOD(ActiveField_DefaultLoad) {
    TestContext context;
    context.Queue().Post([&]() noexcept {
      Mso::ActiveField<std::unique_ptr<int>> field{context.Queue()};
      TestCheckEqual(nullptr, field.Load().get());
    });
    context.WaitSync();
  }

  TESTMETHOD_REQUIRES_SEH(ActiveField_LoadCrash) {
    TEST_DISABLE_MEMORY_LEAK_DETECTION();
    TestContext context;
    auto field = std::make_unique<Mso::ActiveField<std::unique_ptr<int>>>(std::make_unique<int>(5), context.Queue());
    TestCheckCrash(field->Load());
    context.Queue().Post([&field]() noexcept { field = nullptr; });
    context.WaitSync();
  }

  TEST_METHOD(ActiveField_Load) {
    TestContext context;
    context.Queue().Post([&]() noexcept {
      Mso::ActiveField<std::unique_ptr<int>> field{std::make_unique<int>(5), context.Queue()};
      TestCheckEqual(5, *field.Load());
    });
    context.WaitSync();
  }

  TESTMETHOD_REQUIRES_SEH(ActiveField_DefaultStoreCrash) {
    TEST_DISABLE_MEMORY_LEAK_DETECTION();
    TestContext context;
    auto field = std::make_unique<Mso::ActiveField<std::unique_ptr<int>>>(context.Queue());
    TestCheckCrash(field->Store(std::make_unique<int>(5)));
    context.Queue().Post([&field]() noexcept { field = nullptr; });
    context.WaitSync();
  }

  TEST_METHOD(ActiveField_DefaultStore) {
    TestContext context;
    context.Queue().Post([&]() noexcept {
      Mso::ActiveField<std::unique_ptr<int>> field{context.Queue()};
      field.Store(std::make_unique<int>(5));
      TestCheckEqual(5, *field.Load());
    });
    context.WaitSync();
  }

  TESTMETHOD_REQUIRES_SEH(ActiveField_DefaultExchangeCrash) {
    TEST_DISABLE_MEMORY_LEAK_DETECTION();
    TestContext context;
    auto field = std::make_unique<Mso::ActiveField<std::unique_ptr<int>>>(context.Queue());
    TestCheckCrash(field->Exchange(std::make_unique<int>(5)));
    context.Queue().Post([&field]() noexcept { field = nullptr; });
    context.WaitSync();
  }

  TEST_METHOD(ActiveField_DefaultExchange) {
    TestContext context;
    context.Queue().Post([&]() noexcept {
      Mso::ActiveField<std::unique_ptr<int>> field{context.Queue()};
      auto v1 = field.Exchange(std::make_unique<int>(5));
      TestCheckEqual(nullptr, v1.get());
      TestCheckEqual(5, *field.Load());
      auto v2 = field.Exchange(std::make_unique<int>(10));
      TestCheckEqual(5, *v2);
      TestCheckEqual(10, *field.Load());
      auto v3 = field.Exchange(nullptr);
      TestCheckEqual(10, *v3);
      TestCheckEqual(nullptr, field.Load().get());
    });
    context.WaitSync();
  }

  TEST_METHOD(ActiveReadableField_DestructorNoCrash) {
    TestContext context;
    std::mutex readMutex;
    Mso::ActiveReadableField<std::unique_ptr<int>> field(context.Queue(), readMutex);
  }

  TESTMETHOD_REQUIRES_SEH(ActiveReadableField_DefaultLoadCrash) {
    TEST_DISABLE_MEMORY_LEAK_DETECTION();
    TestContext context;
    std::mutex readMutex;
    auto field = std::make_unique<Mso::ActiveReadableField<std::unique_ptr<int>>>(context.Queue(), readMutex);
    TestCheckCrash(field->Load());
    context.Queue().Post([&field]() noexcept { field = nullptr; });
    context.WaitSync();
  }

  TEST_METHOD(ActiveReadableField_DefaultLoad) {
    TestContext context;
    context.Queue().Post([&]() noexcept {
      std::mutex readMutex;
      Mso::ActiveReadableField<std::unique_ptr<int>> field{context.Queue(), readMutex};
      TestCheckEqual(nullptr, field.Load().get());
    });
    context.WaitSync();
  }

  TESTMETHOD_REQUIRES_SEH(ActiveReadableField_LoadCrash) {
    TEST_DISABLE_MEMORY_LEAK_DETECTION();
    TestContext context;
    std::mutex readMutex;
    auto field = std::make_unique<Mso::ActiveReadableField<std::unique_ptr<int>>>(
        std::make_unique<int>(5), context.Queue(), readMutex);
    TestCheckCrash(field->Load());
    context.Queue().Post([&field]() noexcept { field = nullptr; });
    context.WaitSync();
  }

  TEST_METHOD(ActiveReadableField_Load) {
    TestContext context;
    context.Queue().Post([&]() noexcept {
      std::mutex readMutex;
      Mso::ActiveReadableField<std::unique_ptr<int>> field{std::make_unique<int>(5), context.Queue(), readMutex};
      TestCheckEqual(5, *field.Load());
    });
    context.WaitSync();
  }

  TEST_METHOD(ActiveReadableField_DefaultLoadWithLock) {
    TestContext context;
    std::mutex readMutex;
    auto field = std::make_unique<Mso::ActiveReadableField<std::shared_ptr<int>>>(context.Queue(), readMutex);
    TestCheckEqual(nullptr, field->LoadWithLock().get());
    context.Queue().Post([&field]() noexcept { field = nullptr; });
    context.WaitSync();
  }

  TEST_METHOD(ActiveReadableField_DefaultLoadWithLockPost) {
    TestContext context;
    context.Queue().Post([&]() noexcept {
      std::mutex readMutex;
      Mso::ActiveReadableField<std::shared_ptr<int>> field{context.Queue(), readMutex};
      TestCheckEqual(nullptr, field.LoadWithLock().get());
    });
    context.WaitSync();
  }

  TEST_METHOD(ActiveReadableField_LoadWithLock) {
    TestContext context;
    std::mutex readMutex;
    auto field = std::make_unique<Mso::ActiveReadableField<std::shared_ptr<int>>>(
        std::make_shared<int>(5), context.Queue(), readMutex);
    TestCheckEqual(5, *field->LoadWithLock());
    context.Queue().Post([&field]() noexcept { field = nullptr; });
    context.WaitSync();
  }

  TEST_METHOD(ActiveReadableField_LoadWithLockPost) {
    TestContext context;
    context.Queue().Post([&]() noexcept {
      std::mutex readMutex;
      Mso::ActiveReadableField<std::shared_ptr<int>> field{std::make_shared<int>(5), context.Queue(), readMutex};
      TestCheckEqual(5, *field.LoadWithLock());
    });
    context.WaitSync();
  }

  TEST_METHOD(ActiveReadableField_LoadWithLockLocked) {
    TestContext context;
    std::mutex readMutex;
    auto field = std::make_unique<Mso::ActiveReadableField<std::shared_ptr<int>>>(
        std::make_shared<int>(5), context.Queue(), readMutex);
    std::unique_lock<std::mutex> lock{readMutex};
    TestCheckEqual(5, *field->LoadWithLock(lock));
    context.Queue().Post([&field]() noexcept { field = nullptr; });
    context.WaitSync();
  }

  TEST_METHOD(ActiveReadableField_LoadWithLockPostLocked) {
    TestContext context;
    context.Queue().Post([&]() noexcept {
      std::mutex readMutex;
      Mso::ActiveReadableField<std::shared_ptr<int>> field{std::make_shared<int>(5), context.Queue(), readMutex};
      std::unique_lock<std::mutex> lock{readMutex};
      TestCheckEqual(5, *field.LoadWithLock(lock));
    });
    context.WaitSync();
  }

  TESTMETHOD_REQUIRES_SEH(ActiveReadableField_LoadWithLockLockedCrash) {
    TEST_DISABLE_MEMORY_LEAK_DETECTION();
    TestContext context;
    std::mutex readMutex;
    auto field = std::make_unique<Mso::ActiveReadableField<std::shared_ptr<int>>>(
        std::make_shared<int>(5), context.Queue(), readMutex);
    std::unique_lock<std::mutex> lock{readMutex};
    lock.unlock(); // Unlocked causes a crash
    TestCheckCrash(field->LoadWithLock(lock));
    context.Queue().Post([&field]() noexcept { field = nullptr; });
    context.WaitSync();
  }

  TESTMETHOD_REQUIRES_SEH(ActiveReadableField_LoadWithLockLockedCrash2) {
    TEST_DISABLE_MEMORY_LEAK_DETECTION();
    TestContext context;
    std::mutex readMutex;
    auto field = std::make_unique<Mso::ActiveReadableField<std::shared_ptr<int>>>(
        std::make_shared<int>(5), context.Queue(), readMutex);
    std::mutex mutex; // Using different mutex causes a crash
    std::unique_lock<std::mutex> lock{mutex};
    TestCheckCrash(field->LoadWithLock(lock));
    context.Queue().Post([&field]() noexcept { field = nullptr; });
    context.WaitSync();
  }

  TESTMETHOD_REQUIRES_SEH(ActiveReadableField_DefaultExchangeCrash) {
    TEST_DISABLE_MEMORY_LEAK_DETECTION();
    TestContext context;
    std::mutex readMutex;
    auto field = std::make_unique<Mso::ActiveReadableField<std::unique_ptr<int>>>(context.Queue(), readMutex);
    TestCheckCrash(field->Exchange(std::make_unique<int>(5)));
    context.Queue().Post([&field]() noexcept { field = nullptr; });
    context.WaitSync();
  }

  TEST_METHOD(ActiveReadableField_DefaultExchange) {
    TestContext context;
    context.Queue().Post([&]() noexcept {
      std::mutex readMutex;
      Mso::ActiveReadableField<std::unique_ptr<int>> field{context.Queue(), readMutex};
      auto v1 = field.Exchange(std::make_unique<int>(5));
      TestCheckEqual(nullptr, v1.get());
      TestCheckEqual(5, *field.Load());
      auto v2 = field.Exchange(std::make_unique<int>(10));
      TestCheckEqual(5, *v2);
      TestCheckEqual(10, *field.Load());
      auto v3 = field.Exchange(nullptr);
      TestCheckEqual(10, *v3);
      TestCheckEqual(nullptr, field.Load().get());
    });
    context.WaitSync();
  }

  TESTMETHOD_REQUIRES_SEH(ActiveReadableField_DefaultExchangeLockedCrash) {
    TEST_DISABLE_MEMORY_LEAK_DETECTION();
    TestContext context;
    std::mutex readMutex;
    auto field = std::make_unique<Mso::ActiveReadableField<std::unique_ptr<int>>>(context.Queue(), readMutex);
    std::unique_lock<std::mutex> lock{readMutex};
    TestCheckCrash(field->Exchange(std::make_unique<int>(5), lock));
    context.Queue().Post([&field]() noexcept { field = nullptr; });
    context.WaitSync();
  }

  TEST_METHOD(ActiveReadableField_DefaultExchangeLocked) {
    TestContext context;
    context.Queue().Post([&]() noexcept {
      std::mutex readMutex;
      Mso::ActiveReadableField<std::unique_ptr<int>> field{context.Queue(), readMutex};
      std::unique_lock<std::mutex> lock{readMutex};
      auto v1 = field.Exchange(std::make_unique<int>(5), lock);
      TestCheckEqual(nullptr, v1.get());
      TestCheckEqual(5, *field.Load());
      auto v2 = field.Exchange(std::make_unique<int>(10), lock);
      TestCheckEqual(5, *v2);
      TestCheckEqual(10, *field.Load());
      auto v3 = field.Exchange(nullptr, lock);
      TestCheckEqual(10, *v3);
      TestCheckEqual(nullptr, field.Load().get());
    });
    context.WaitSync();
  }

  TESTMETHOD_REQUIRES_SEH(ActiveReadableField_ExchangeWithLockLockedCrash) {
    TEST_DISABLE_MEMORY_LEAK_DETECTION();
    TestContext context;
    context.Queue().Post([&]() noexcept {
      std::mutex readMutex;
      Mso::ActiveReadableField<std::shared_ptr<int>> field{context.Queue(), readMutex};
      std::unique_lock<std::mutex> lock{readMutex};
      lock.unlock(); // Unlocked causes a crash
      TestCheckCrash(field.Exchange(std::make_shared<int>(5), lock));
    });
    context.WaitSync();
  }

  TESTMETHOD_REQUIRES_SEH(ActiveReadableField_ExchangeWithLockLockedCrash2) {
    TEST_DISABLE_MEMORY_LEAK_DETECTION();
    TestContext context;
    context.Queue().Post([&]() noexcept {
      std::mutex readMutex;
      Mso::ActiveReadableField<std::shared_ptr<int>> field{context.Queue(), readMutex};
      std::mutex mutex; // Using different mutex causes a crash
      std::unique_lock<std::mutex> lock{mutex};
      TestCheckCrash(field.Exchange(std::make_shared<int>(5), lock));
    });
    context.WaitSync();
  }

 private:
  // Some tests verify that we crash. These tests cause memory leaks.
  // We collect memory leaks per each test to ignore leaks in tests for crash.
  // MemoryLeakDetectionHook::TrackPerTest m_trackLeakPerTest;
};

} // namespace ActiveObjectTests

OACR_WARNING_POP
