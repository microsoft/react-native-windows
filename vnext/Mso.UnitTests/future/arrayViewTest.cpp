// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "future/future.h"
#include "motifCpp/libletAwareMemLeakDetection.h"
#include "motifCpp/testCheck.h"
#include "testCheck.h"
#include "testExecutor.h"

namespace FutureTests {

TEST_CLASS_EX (ArrayViewTest, LibletAwareMemLeakDetection) {
  TEST_METHOD(ArrayView_ctor_Default) {
    Mso::Async::ArrayView<int> a1;
    TestCheckEqual(nullptr, a1.Data());
    TestCheckEqual(0u, a1.Size());
  }

  TEST_METHOD(ArrayView_ctor_Array) {
    int xs[] = {1, 2, 3};
    Mso::Async::ArrayView<int> a1(xs);
    TestCheckEqual(3u, a1.Size());
    TestCheckEqual(1, a1[0]);
    TestCheckEqual(2, a1[1]);
    TestCheckEqual(3, a1[2]);
  }

  TEST_METHOD(ArrayView_ctor_initializer_list) {
    // We use lambda to avoid ArrayView use temporary objects.
    [](Mso::Async::ArrayView<int> a1) {
      TestCheckEqual(3u, a1.Size());
      TestCheckEqual(1, a1[0]);
      TestCheckEqual(2, a1[1]);
      TestCheckEqual(3, a1[2]);
    }({1, 2, 3});
  }

  TEST_METHOD(ArrayView_ctor_vector) {
    std::vector<int> xs = {1, 2, 3};
    Mso::Async::ArrayView<int> a1(xs.data(), xs.size());
    TestCheckEqual(3u, a1.Size());
    TestCheckEqual(1, a1[0]);
    TestCheckEqual(2, a1[1]);
    TestCheckEqual(3, a1[2]);
  }

  TEST_METHOD(ArrayView_ctor_copy) {
    std::vector<int> xs = {1, 2, 3};
    Mso::Async::ArrayView<int> a1(xs.data(), xs.size());
    Mso::Async::ArrayView<int> a2(a1);
    TestCheckEqual(3u, a2.Size());
    TestCheckEqual(1, a2[0]);
    TestCheckEqual(2, a2[1]);
    TestCheckEqual(3, a2[2]);
  }

  TEST_METHOD(ArrayView_assignment_copy) {
    std::vector<int> xs = {1, 2, 3};
    Mso::Async::ArrayView<int> a1(xs.data(), xs.size());
    Mso::Async::ArrayView<int> a2;
    a2 = a1;
    TestCheckEqual(3u, a2.Size());
    TestCheckEqual(1, a2[0]);
    TestCheckEqual(2, a2[1]);
    TestCheckEqual(3, a2[2]);
  }

  TEST_METHOD(ArrayView_Data_Size) {
    std::vector<int> xs = {1, 2, 3};
    Mso::Async::ArrayView<int> a1(xs.data(), xs.size());
    TestCheckEqual(xs.data(), a1.Data());
    TestCheckEqual(3u, a1.Size());
  }

  TEST_METHOD(ArrayView_ConstData_Size) {
    std::vector<int> xs = {1, 2, 3};
    const auto a1 = Mso::Async::ArrayView<int>(xs.data(), xs.size());
    TestCheckEqual(xs.data(), a1.Data());
    TestCheckEqual(3u, a1.Size());
  }

  TEST_METHOD(ArrayView_VoidData) {
    std::vector<int> xs = {1, 2, 3};
    Mso::Async::ArrayView<int> a1(xs.data(), xs.size());
    TestCheckEqual(xs.data(), a1.VoidData());
  }

  TEST_METHOD(ArrayView_VoidDataChecked) {
    std::vector<int> xs = {1, 2, 3};
    Mso::Async::ArrayView<int> a1(xs.data(), xs.size());
    TestCheckEqual(xs.data(), a1.VoidDataChecked(3));
  }

  TESTMETHOD_REQUIRES_SEH(ArrayView_VoidDataChecked_Fail) {
    std::vector<int> xs = {1, 2, 3};
    Mso::Async::ArrayView<int> a1(xs.data(), xs.size());
    TestCheckCrash(a1.VoidDataChecked(2));
  }

  TESTMETHOD_REQUIRES_SEH(ArrayView_VoidDataChecked_Fail2) {
    std::vector<int> xs = {1, 2, 3};
    Mso::Async::ArrayView<int> a1(xs.data(), xs.size());
    TestCheckCrash(a1.VoidDataChecked(4));
  }

  TEST_METHOD(ArrayView_VoidDataCheckedMin) {
    std::vector<int> xs = {1, 2, 3};
    Mso::Async::ArrayView<int> a1(xs.data(), xs.size());
    TestCheckEqual(xs.data(), a1.VoidDataCheckedMin(3));
  }

  TEST_METHOD(ArrayView_VoidDataCheckedMin2) {
    std::vector<int> xs = {1, 2, 3};
    Mso::Async::ArrayView<int> a1(xs.data(), xs.size());
    TestCheckEqual(xs.data(), a1.VoidDataCheckedMin(2));
  }

  TESTMETHOD_REQUIRES_SEH(ArrayView_VoidDataCheckedMin_Fail) {
    std::vector<int> xs = {1, 2, 3};
    Mso::Async::ArrayView<int> a1(xs.data(), xs.size());
    TestCheckCrash(a1.VoidDataCheckedMin(4));
  }

  TEST_METHOD(ArrayView_begin_end) {
    std::vector<int> xs = {1, 2, 3};
    Mso::Async::ArrayView<int> a1(xs.data(), xs.size());
    int val = 10;
    for (auto &a : a1) {
      a = ++val;
    }
    TestCheckEqual(11, a1[0]);
    TestCheckEqual(12, a1[1]);
    TestCheckEqual(13, a1[2]);
  }

  TEST_METHOD(ArrayView_const_begin_end) {
    std::vector<int> xs = {1, 2, 3};
    const auto a1 = Mso::Async::ArrayView<int>(xs.data(), xs.size());
    int index = 0;
    for (const auto &a : a1) {
      TestCheckEqual(++index, a);
    }
  }

  TEST_METHOD(ArrayView_op_subscript) {
    std::vector<int> xs = {1, 2, 3};
    auto a1 = Mso::Async::ArrayView<int>(xs.data(), xs.size());
    TestCheckEqual(1, a1[0]);
    TestCheckEqual(2, a1[1]);
    TestCheckEqual(3, a1[2]);
  }

  TEST_METHOD(ArrayView_const_op_subscript) {
    std::vector<int> xs = {1, 2, 3};
    const auto a1 = Mso::Async::ArrayView<int>(xs.data(), xs.size());
    TestCheckEqual(1, a1[0]);
    TestCheckEqual(2, a1[1]);
    TestCheckEqual(3, a1[2]);
  }

  TEST_METHOD(ArrayView_op_bool) {
    Mso::Async::ArrayView<int> a1;
    if (a1) {
      TestCheckFail("");
    }

    std::vector<int> xs = {1, 2, 3};
    const auto a2 = Mso::Async::ArrayView<int>(xs.data(), xs.size());
    if (!a2) {
      TestCheckFail("");
    }
  }
};

} // namespace FutureTests
