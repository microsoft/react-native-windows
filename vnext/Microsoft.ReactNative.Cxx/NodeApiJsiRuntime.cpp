// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#define NAPI_EXPERIMENTAL

#include "NodeApiJsiRuntime.h"

#include <algorithm>
#include <array>
#include <mutex>
#include <optional>
#include <sstream>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

// JSI version defines set of features available in the API.
// Each significant API change must be under a new version.
// These macros must be defined in jsi.h, but define them here too
// in case if this code is used with unmodified jsi.h.
#ifndef JSI_VERSION
#define JSI_VERSION 10
#endif

#ifndef JSI_NO_CONST_3
#if JSI_VERSION >= 3
#define JSI_NO_CONST_3
#else
#define JSI_NO_CONST_3 const
#endif
#endif

#ifndef JSI_CONST_10
#if JSI_VERSION >= 10
#define JSI_CONST_10 const
#else
#define JSI_CONST_10
#endif
#endif

using namespace facebook;
using namespace std::string_view_literals;

// We use macros to report errors.
// Macros provide more flexibility to show assert and provide failure context.

#if defined(__clang__) || defined(__GNUC__)
#define CRASH_NOW() __builtin_trap()
#elif defined(_MSC_VER)
#include <intrin.h>
#define CRASH_NOW() __fastfail(/*FAST_FAIL_FATAL_APP_EXIT*/ 7)
#else
#define CRASH_NOW() *((volatile int *)0) = 1
#endif

// Check condition and crash process if it fails.
#define CHECK_ELSE_CRASH(condition, message)               \
  do {                                                     \
    if (!(condition)) {                                    \
      assert(false && "Failed: " #condition && (message)); \
      CRASH_NOW();                                         \
    }                                                      \
  } while (false)

// Check condition and throw native exception if it fails.
#define CHECK_ELSE_THROW(condition, message) \
  do {                                       \
    if (!(condition)) {                      \
      throwNativeException(message);         \
    }                                        \
  } while (false)

// Check Node-API result and and throw JS exception if it is not napi_ok.
#define CHECK_NAPI(...)                             \
  do {                                              \
    napi_status temp_error_code_ = (__VA_ARGS__);   \
    if (temp_error_code_ != napi_status::napi_ok) { \
      runtime.throwJSException(temp_error_code_);   \
    }                                               \
  } while (false)

// Check Node-API result and and crash if it is not napi_ok.
#define CHECK_NAPI_ELSE_CRASH(expression)              \
  do {                                                 \
    napi_status temp_error_code_ = (expression);       \
    if (temp_error_code_ != napi_status::napi_ok) {    \
      CHECK_ELSE_CRASH(false, "Failed: " #expression); \
    }                                                  \
  } while (false)

// Check Node-API result and return it when it is an error.
#define NAPI_CALL(expression)                       \
  do {                                              \
    napi_status temp_error_code_ = (expression);    \
    if (temp_error_code_ != napi_status::napi_ok) { \
      return temp_error_code_;                      \
    }                                               \
  } while (false)

#ifdef __cpp_lib_span
#include <span>
#endif // __cpp_lib_span

namespace Microsoft::NodeApiJsi {

namespace {

#ifdef __cpp_lib_span
using std::span;
#else
/**
 * @brief A span of values that can be used to pass arguments to a function.
 *
 * This should be replaced with std::span once C++20 is supported.
 */
template <typename T>
class span {
 public:
  constexpr span() noexcept : data_{nullptr}, size_{0} {}
  constexpr span(T *data, size_t size) noexcept : data_{data}, size_{size} {}
  template <std::size_t N>
  constexpr span(T (&arr)[N]) noexcept : data_{arr}, size_{N} {}

  [[nodiscard]] constexpr T *data() const noexcept {
    return data_;
  }

  [[nodiscard]] constexpr size_t size() const noexcept {
    return size_;
  }

  [[nodiscard]] constexpr T *begin() const noexcept {
    return data_;
  }

  [[nodiscard]] constexpr T *end() const noexcept {
    return *(data_ + size_);
  }

  const T &operator[](size_t index) const noexcept {
    return *(data_ + index);
  }

 private:
  T *data_;
  size_t size_;
};
#endif // __cpp_lib_span

// To be used as a key in a unordered_map.
class StringKey {
 public:
  explicit StringKey(std::string &&string) noexcept;
  explicit StringKey(std::string_view view) noexcept;
  explicit StringKey(const char *data, size_t length) noexcept;
  StringKey(StringKey &&other) noexcept;
  StringKey &operator=(StringKey &&other) noexcept;
  StringKey(const StringKey &other) = delete;
  StringKey &operator=(const StringKey &other) = delete;
  ~StringKey();

  std::string_view getStringView() const;
  bool equalTo(const StringKey &other) const;
  size_t hash() const;

  struct Hash {
    size_t operator()(const StringKey &key) const;
  };

  struct EqualTo {
    bool operator()(const StringKey &left, const StringKey &right) const;
  };

 private:
  enum class Type {
    String,
    View,
  };

 private:
  union {
    std::string string_;
    std::string_view view_;
  };
  Type type_{Type::String};
  size_t hash_;
};

struct NodeApiAttachTag {
} attachTag;

// Implementation of N-API JSI Runtime
class NodeApiJsiRuntime : public jsi::Runtime {
 public:
  NodeApiJsiRuntime(
      napi_env env,
      JSRuntimeApi *jsrApi,
      std::function<void()> onDelete) noexcept;
  ~NodeApiJsiRuntime() override;

  jsi::Value evaluateJavaScript(
      const std::shared_ptr<const jsi::Buffer> &buffer,
      const std::string &sourceURL) override;
  std::shared_ptr<const jsi::PreparedJavaScript> prepareJavaScript(
      const std::shared_ptr<const jsi::Buffer> &buffer,
      std::string sourceURL) override;
  jsi::Value evaluatePreparedJavaScript(
      const std::shared_ptr<const jsi::PreparedJavaScript> &js) override;
#if JSI_VERSION >= 12
  void queueMicrotask(const jsi::Function &callback) override;
#endif
#if JSI_VERSION >= 4
  bool drainMicrotasks(int maxMicrotasksHint = -1) override;
#endif
  jsi::Object global() override;
  std::string description() override;
  bool isInspectable() override;

 protected:
  PointerValue *cloneSymbol(const PointerValue *pointerValue) override;
#if JSI_VERSION >= 6
  PointerValue *cloneBigInt(const PointerValue *pointerValue) override;
#endif
  PointerValue *cloneString(const PointerValue *pointerValue) override;
  PointerValue *cloneObject(const PointerValue *pointerValue) override;
  PointerValue *clonePropNameID(const PointerValue *pointerValue) override;

  jsi::PropNameID createPropNameIDFromAscii(const char *str, size_t length)
      override;
  jsi::PropNameID createPropNameIDFromUtf8(const uint8_t *utf8, size_t length)
      override;
  jsi::PropNameID createPropNameIDFromString(const jsi::String &str) override;
#if JSI_VERSION >= 5
  jsi::PropNameID createPropNameIDFromSymbol(const jsi::Symbol &sym) override;
#endif
  std::string utf8(const jsi::PropNameID &id) override;
  bool compare(const jsi::PropNameID &lhs, const jsi::PropNameID &rhs) override;

  std::string symbolToString(const jsi::Symbol &s) override;

#if JSI_VERSION >= 8
  jsi::BigInt createBigIntFromInt64(int64_t value) override;
  jsi::BigInt createBigIntFromUint64(uint64_t value) override;
  bool bigintIsInt64(const jsi::BigInt &value) override;
  bool bigintIsUint64(const jsi::BigInt &value) override;
  uint64_t truncate(const jsi::BigInt &value) override;
  jsi::String bigintToString(const jsi::BigInt &value, int radix) override;
#endif

  jsi::String createStringFromAscii(const char *str, size_t length) override;
  jsi::String createStringFromUtf8(const uint8_t *utf8, size_t length) override;
  std::string utf8(const jsi::String &str) override;

  jsi::Object createObject() override;
  jsi::Object createObject(std::shared_ptr<jsi::HostObject> ho) override;
  std::shared_ptr<jsi::HostObject> getHostObject(const jsi::Object &) override;
  jsi::HostFunctionType &getHostFunction(const jsi::Function &) override;

#if JSI_VERSION >= 7
  bool hasNativeState(const jsi::Object &value) override;
  std::shared_ptr<jsi::NativeState> getNativeState(
      const jsi::Object &value) override;
  void setNativeState(
      const jsi::Object &value,
      std::shared_ptr<jsi::NativeState> state) override;
#endif

  jsi::Value getProperty(const jsi::Object &obj, const jsi::PropNameID &name)
      override;
  jsi::Value getProperty(const jsi::Object &obj, const jsi::String &name)
      override;

#if JSI_VERSION >= 21
  jsi::Value getProperty(const jsi::Object &obj, const jsi::Value &name)
      override;
#endif

  bool hasProperty(const jsi::Object &obj, const jsi::PropNameID &name)
      override;
  bool hasProperty(const jsi::Object &obj, const jsi::String &name) override;

#if JSI_VERSION >= 21
  bool hasProperty(const jsi::Object &obj, const jsi::Value &name) override;
#endif

  void setPropertyValue(
      JSI_CONST_10 jsi::Object &obj,
      const jsi::PropNameID &name,
      const jsi::Value &value) override;
  void setPropertyValue(
      JSI_CONST_10 jsi::Object &obj,
      const jsi::String &name,
      const jsi::Value &value) override;

#if JSI_VERSION >= 21
  void setPropertyValue(
      const jsi::Object &obj,
      const jsi::Value &name,
      const jsi::Value &value) override;

  void deleteProperty(const jsi::Object &obj, const jsi::PropNameID &name)
      override;
  void deleteProperty(const jsi::Object &obj, const jsi::String &name) override;

  void deleteProperty(const jsi::Object &obj, const jsi::Value &name) override;
#endif

  bool isArray(const jsi::Object &obj) const override;
  bool isArrayBuffer(const jsi::Object &obj) const override;
  bool isFunction(const jsi::Object &obj) const override;
  bool isHostObject(const jsi::Object &obj) const override;
  bool isHostFunction(const jsi::Function &func) const override;
  jsi::Array getPropertyNames(const jsi::Object &obj) override;

  jsi::WeakObject createWeakObject(const jsi::Object &obj) override;
  jsi::Value lockWeakObject(
      JSI_NO_CONST_3 JSI_CONST_10 jsi::WeakObject &weakObj) override;

  jsi::Array createArray(size_t length) override;
#if JSI_VERSION >= 9
  jsi::ArrayBuffer createArrayBuffer(
      std::shared_ptr<jsi::MutableBuffer> buffer) override;
#endif
  size_t size(const jsi::Array &arr) override;
  size_t size(const jsi::ArrayBuffer &arrBuf) override;
  uint8_t *data(const jsi::ArrayBuffer &arrBuff) override;
  jsi::Value getValueAtIndex(const jsi::Array &arr, size_t index) override;
  void setValueAtIndexImpl(
      JSI_CONST_10 jsi::Array &arr,
      size_t index,
      const jsi::Value &value) override;

  jsi::Function createFunctionFromHostFunction(
      const jsi::PropNameID &name,
      unsigned int paramCount,
      jsi::HostFunctionType func) override;
  jsi::Value call(
      const jsi::Function &func,
      const jsi::Value &jsThis,
      const jsi::Value *args,
      size_t count) override;
  jsi::Value callAsConstructor(
      const jsi::Function &func,
      const jsi::Value *args,
      size_t count) override;

  ScopeState *pushScope() override;
  void popScope(ScopeState *) override;

  bool strictEquals(const jsi::Symbol &a, const jsi::Symbol &b) const override;
#if JSI_VERSION >= 6
  bool strictEquals(const jsi::BigInt &a, const jsi::BigInt &b) const override;
#endif
  bool strictEquals(const jsi::String &a, const jsi::String &b) const override;
  bool strictEquals(const jsi::Object &a, const jsi::Object &b) const override;

  bool instanceOf(const jsi::Object &obj, const jsi::Function &func) override;

#if JSI_VERSION >= 11
  void setExternalMemoryPressure(const jsi::Object &obj, size_t amount)
      override;
#endif

 private:
  // RAII class to open and close the environment scope.
  class NodeApiScope {
   public:
    NodeApiScope(const NodeApiJsiRuntime &runtime) noexcept;
    NodeApiScope(NodeApiJsiRuntime &runtime) noexcept;
    ~NodeApiScope() noexcept;

    NodeApiScope(const NodeApiScope &) = delete;
    NodeApiScope &operator=(const NodeApiScope &) = delete;

   private:
    NodeApiJsiRuntime &runtime_;
    NodeApiEnvScope envScope_;
    ScopeState *scopeState_{};
  };

  // RAII class to open and close the environment scope.
  class NodeApiPointerValueScope {
   public:
    NodeApiPointerValueScope(NodeApiJsiRuntime &runtime) noexcept;
    ~NodeApiPointerValueScope() noexcept;

    NodeApiPointerValueScope(const NodeApiPointerValueScope &) = delete;
    NodeApiPointerValueScope &operator=(const NodeApiPointerValueScope &) =
        delete;

   private:
    NodeApiJsiRuntime &runtime_;
  };

  // Sets the variable in the constructor and then restores its value in the
  // destructor.
  template <typename T>
  class AutoRestore {
   public:
    AutoRestore(T &varRef, T value);
    ~AutoRestore();

    AutoRestore(const AutoRestore &) = delete;
    AutoRestore &operator=(const AutoRestore &) = delete;

   private:
    T &varRef_;
    T oldValue_;
  };

  enum class NodeApiPointerValueKind : uint8_t {
    Object,
    WeakObject,
    String,
    StringPropNameID,
    Symbol,
    BigInt,
  };

  class NodeApiRefCountedPointerValue;

  class NodeApiRefCount {
   public:
    static void incRefCount(std::atomic<int32_t> &value) noexcept {
      int refCount = value.fetch_add(1, std::memory_order_relaxed) + 1;
      CHECK_ELSE_CRASH(refCount > 1, "The ref count cannot bounce from zero.");
      CHECK_ELSE_CRASH(
          refCount < std::numeric_limits<int32_t>::max(),
          "The ref count is too big.");
    }

    static bool decRefCount(std::atomic<int32_t> &value) noexcept {
      int refCount = value.fetch_sub(1, std::memory_order_release) - 1;
      CHECK_ELSE_CRASH(refCount >= 0, "The ref count must not be negative.");
      if (refCount == 0) {
        std::atomic_thread_fence(std::memory_order_acquire);
        return true;
      }
      return false;
    }

    static bool isZero(std::atomic<int32_t> &value) noexcept {
      return value.load(std::memory_order_relaxed) == 0;
    }
  };

  // A smart pointer for types that implement intrusive ref count using
  // methods incRefCount and decRefCount.
  template <typename T>
  class NodeApiRefCountedPtr final {
   public:
    NodeApiRefCountedPtr() noexcept = default;

    explicit NodeApiRefCountedPtr(T *ptr, NodeApiAttachTag) noexcept
        : ptr_(ptr) {}

    NodeApiRefCountedPtr(const NodeApiRefCountedPtr &other) noexcept
        : ptr_(other.ptr_) {
      if (ptr_ != nullptr) {
        ptr_->incRefCount();
      }
    }

    NodeApiRefCountedPtr(NodeApiRefCountedPtr &&other)
        : ptr_(std::exchange(other.ptr_, nullptr)) {}

    ~NodeApiRefCountedPtr() noexcept {
      if (ptr_ != nullptr) {
        ptr_->decRefCount();
      }
    }

    NodeApiRefCountedPtr &operator=(std::nullptr_t) noexcept {
      if (ptr_ != nullptr) {
        ptr_->decRefCount();
      }
      ptr_ = nullptr;
      return *this;
    }

    NodeApiRefCountedPtr &operator=(
        const NodeApiRefCountedPtr &other) noexcept {
      if (this != &other) {
        NodeApiRefCountedPtr temp(std::move(*this));
        ptr_ = other.ptr_;
        if (ptr_ != nullptr) {
          ptr_->incRefCount();
        }
      }
      return *this;
    }

    NodeApiRefCountedPtr &operator=(NodeApiRefCountedPtr &&other) noexcept {
      if (this != &other) {
        NodeApiRefCountedPtr temp(std::move(*this));
        ptr_ = std::exchange(other.ptr_, nullptr);
      }
      return *this;
    }

    T *operator->() const noexcept {
      return ptr_;
    }

    T *get() const noexcept {
      return ptr_;
    }

    explicit operator bool() const noexcept {
      return ptr_ != nullptr;
    }

    T *release() noexcept {
      return std::exchange(ptr_, nullptr);
    }

   private:
    T *ptr_{};
  };

  // Removes the `napi_value value_` field.
  class NodeApiStackValueDeleter {
   public:
    void operator()(NodeApiRefCountedPointerValue *ptr) const noexcept;
  };

  // Removes the NodeApiRefCountedPointerValue instance and ignores the
  // `napi_ref ref_` field.
  class NodeApiRefDeleter {
   public:
    void operator()(NodeApiRefCountedPointerValue *ptr) const noexcept;
  };

  using NodeApiRefHolder = NodeApiRefCountedPtr<NodeApiRefCountedPointerValue>;
  using NodeApiStackValuePtr =
      std::unique_ptr<NodeApiRefCountedPointerValue, NodeApiStackValueDeleter>;
  using NodeApiRefPtr =
      std::unique_ptr<NodeApiRefCountedPointerValue, NodeApiRefDeleter>;

  // NodeApiPendingDeletions helps to delete PointerValues in a thread safe way
  // from the JS thread. According to JSI spec the PointerValue's release method
  // can be called from any thread, while Node-API can only manage objects in
  // the JS thread. So, when a PointerValue's ref count goes to zero after
  // calling the release method, the PointerValue is added into the
  // pointerValuesToDelete_ vector, and then NodeApiJsiRuntime deletes them
  // later from the JS thread. Note that the napi_delete_reference can only be
  // called before the napi_env is destroyed. Thus, we remove the pointer to
  // napi_env as soon as NodeApiJsiRuntime destructor starts.
  class NodeApiPendingDeletions {
   public:
    // Create new instance of NodeApiPendingDeletions.
    static NodeApiRefCountedPtr<NodeApiPendingDeletions> create() noexcept {
      return NodeApiRefCountedPtr<NodeApiPendingDeletions>(
          new NodeApiPendingDeletions(), attachTag);
    }

    // Add PointerValues to delete from JS thread. The method can be called from
    // any thread.
    void addPointerValueToDelete(NodeApiRefPtr pointerValueToDelete) noexcept {
      std::scoped_lock lock{mutex_};
      pointerValuesToDeletePool_[poolSelector_].push_back(
          std::move(pointerValueToDelete));
    }

    // Delete all PointerValues scheduled for deletion along with their napi_ref
    // instances. It must be called from a JS thread.
    void deletePointerValues(NodeApiJsiRuntime &runtime) noexcept {
      {
        // TODO: Does it affect the performance to take the lock every time?
        // Should we use an atomic variable?
        std::scoped_lock lock{mutex_};
        if (pointerValuesToDeletePool_[poolSelector_].empty()) {
          return;
        }

        // Switch the pool entries.
        poolSelector_ = poolSelector_ ^ 1;
      }

      std::vector<NodeApiRefPtr> &deleteInJSThread =
          pointerValuesToDeletePool_[poolSelector_ ^ 1];
      for (auto &pointerValue : deleteInJSThread) {
        pointerValue.release()->deleteNodeApiRef(runtime);
      }
      deleteInJSThread.resize(0);
    }

   private:
    friend class NodeApiRefCountedPtr<NodeApiPendingDeletions>;

    NodeApiPendingDeletions() noexcept = default;

    void incRefCount() noexcept {
      NodeApiRefCount::incRefCount(refCount_);
    }

    void decRefCount() noexcept {
      if (NodeApiRefCount::decRefCount(refCount_)) {
        delete this;
      }
    }

   private:
    mutable std::atomic<int32_t> refCount_{1};
    std::recursive_mutex mutex_;
    // One of the vectors is used from different threads under the mutex_ lock
    // to add items, while another is from JS thread to remove items. Since we
    // never change the capacity of the vectors, it should help avoiding memory
    // allocations at some point.
    std::vector<NodeApiRefPtr> pointerValuesToDeletePool_[2]{
        std::vector<NodeApiRefPtr>(),
        std::vector<NodeApiRefPtr>()};
    // Index of the pool to access under mutex.
    int32_t poolSelector_{0};
  };

  // NodeApiPointerValue is used by jsi::Pointer derived classes.
  struct NodeApiPointerValue : PointerValue {
    virtual NodeApiRefCountedPointerValue *clone(
        NodeApiJsiRuntime &runtime) const noexcept = 0;
    virtual napi_value getValue(NodeApiJsiRuntime &runtime) noexcept = 0;
    virtual NodeApiPointerValueKind getKind() const noexcept = 0;
  };

  // NodeApiStackOnlyPointerValue helps to avoid memory allocation in some
  // scenarios. It is used by the JsiValueView, JsiValueViewArgs, and
  // PropNameIDView classes to keep temporary PointerValues on the call stack
  // when we call functions. Note that the clone() method return a new instance
  // of the NodeApiRefCountedPointerValue.
  class NodeApiStackOnlyPointerValue final : public NodeApiPointerValue {
   public:
    NodeApiStackOnlyPointerValue(
        napi_value value,
        NodeApiPointerValueKind pointerKind) noexcept;

    void invalidate() noexcept override;
    NodeApiRefCountedPointerValue *clone(
        NodeApiJsiRuntime &runtime) const noexcept override;
    napi_value getValue(NodeApiJsiRuntime &runtime) noexcept override;
    NodeApiPointerValueKind getKind() const noexcept override;

    NodeApiStackOnlyPointerValue(const NodeApiStackOnlyPointerValue &) = delete;
    NodeApiStackOnlyPointerValue &operator=(
        const NodeApiStackOnlyPointerValue &) = delete;

   private:
    napi_value value_{};
    NodeApiPointerValueKind pointerKind_{NodeApiPointerValueKind::Object};
  };

  // TODO: Use arena allocator for NodeApiRefCountedPointerValue.

  // NodeApiRefCountedPointerValue is a ref counted implementation of
  // PointerValue that is allocated in the heap.
  //
  // Its lifetime is controlled by the atomic `refCount_` field. Since the
  // `refCount_` can be changed from any thread, we do not remove the instance
  // immediately when the `refCount_` becomes zero. Instead, we add it to the
  // `NodeApiJsiRuntime::pendingDeletions_` list and delete it later from the JS
  // thread. If `node_value value_` field is not null, then the
  // `NodeApiRefCountedPointerValue` instance is also referenced from the
  // `NodeApiJsiRuntime::stackValues_` list.
  //
  // The `NodeApiJsiRuntime::pendingDeletions_` is responsible for deleting
  // `napi_ref ref_` and it deletes `NodeApiRefCountedPointerValue` instance if
  // the `node_value value_` field is null. While
  // `NodeApiJsiRuntime::stackValues_` is responsible for deleting
  // `NodeApiRefCountedPointerValue` instance if `node_value value_` field is
  // not null and `napi_ref ref_` is null. In case if
  // `NodeApiJsiRuntime::pendingDeletions_` or `NodeApiJsiRuntime::stackValues_`
  // cannot delete the instance, they set their "owned" `value_` or `ref_`
  // fields to null.
  //
  // Some NodeApiRefCountedPointerValue are created with napi_value and may
  // never get napi_ref. When stack scope is closed we check the `refCount_`. If
  // it is not zero, then we ensure that it has an associated `napi_ref` or we
  // create one.
  //
  // All methods except for invalidate() and decRefCount() must be called from
  // the JS thread.
  class NodeApiRefCountedPointerValue final : public NodeApiPointerValue {
    friend class NodeApiStackValueDeleter;
    friend class NodeApiRefDeleter;
    friend class NodeApiRefCountedPtr<NodeApiRefCountedPointerValue>;

   public:
    // Creates new NodeApiRefCountedPointerValue and adds it to the
    // NodeApiJsiRuntime::stackValues_.
    static NodeApiRefHolder make(
        NodeApiJsiRuntime &runtime,
        napi_value value,
        NodeApiPointerValueKind pointerKind,
        int32_t initialRefCount = 1);

    // Calls `make` method and forces creation of `napi_ref`.
    static NodeApiRefHolder makeNodeApiRef(
        NodeApiJsiRuntime &runtime,
        napi_value value,
        NodeApiPointerValueKind pointerKind,
        int32_t initialRefCount = 1);

    void invalidate() noexcept override;
    NodeApiRefCountedPointerValue *clone(
        NodeApiJsiRuntime &runtime) const noexcept override;
    napi_value getValue(NodeApiJsiRuntime &runtime) noexcept override;
    NodeApiPointerValueKind getKind() const noexcept override;

    // Returns true if the refCount_ is not zero.
    bool usedByJsiPointer() const noexcept;

    // Removes `napi_value value_` field.
    // In case if `refCount_` is not null, it ensures existence of the `napi_ref
    // ref_` field.
    void deleteStackValue(NodeApiJsiRuntime &runtime) noexcept;

    // Removes napi_ref and deletes `NodeApiRefCountedPointerValue` if `value_`
    // is null.
    void deleteNodeApiRef(NodeApiJsiRuntime &runtime) noexcept;

    NodeApiRefCountedPointerValue(const NodeApiRefCountedPointerValue &) =
        delete;
    NodeApiRefCountedPointerValue &operator=(
        const NodeApiRefCountedPointerValue &) = delete;

   private:
    NodeApiRefCountedPointerValue(
        NodeApiJsiRuntime &runtime,
        napi_value value,
        NodeApiPointerValueKind pointerKind,
        int32_t initialRefCount) noexcept;

    void incRefCount() const noexcept;
    void decRefCount() const noexcept;

    // Creates `napi_ref ref_` field for the `napi_value value_` field.
    NodeApiRefCountedPointerValue *createNodeApiRef(NodeApiJsiRuntime &runtime);

   private:
    NodeApiRefCountedPtr<NodeApiPendingDeletions> pendingDeletions_;
    napi_value value_{};
    napi_ref ref_{};
    mutable std::atomic<int32_t> refCount_{1};
    const NodeApiPointerValueKind pointerKind_{NodeApiPointerValueKind::Object};
    bool canBeDeletedFromStack_{false};

    static constexpr char kPrimitivePropertyName[] = "X";
  };

  // SmallBuffer keeps InplaceSize elements in place in the class, and uses heap
  // memory for more elements.
  template <typename T, size_t InplaceSize>
  class SmallBuffer {
   public:
    SmallBuffer(size_t size) noexcept;

    T *data() noexcept;
    size_t size() const noexcept;

    SmallBuffer(const SmallBuffer &) = delete;
    SmallBuffer &operator=(const SmallBuffer &) = delete;

   private:
    size_t size_{};
    std::array<T, InplaceSize> stackData_{};
    std::unique_ptr<T[]> heapData_{};
  };

  // The number of arguments that we keep on stack. We use heap if we have more
  // arguments.
  constexpr static size_t MaxStackArgCount = 8;

  // NodeApiValueArgs helps optimize passing arguments to Node-API functions.
  // If number of arguments is below or equal to MaxStackArgCount, they are kept
  // on the call stack, otherwise arguments are allocated on the heap.
  class NodeApiValueArgs {
   public:
    NodeApiValueArgs(NodeApiJsiRuntime &runtime, span<const jsi::Value> args);
    operator span<napi_value>();

    NodeApiValueArgs(const NodeApiValueArgs &) = delete;
    NodeApiValueArgs &operator=(const NodeApiValueArgs &) = delete;

   private:
    SmallBuffer<napi_value, MaxStackArgCount> args_;
  };

  // Helps to use the stack storage for a temporary conversion from napi_value
  // to jsi::Value. It also helps to avoid a conversion to a relatively
  // expensive napi_ref.
  class JsiValueView {
   public:
    union StoreType {
      NodeApiStackOnlyPointerValue value_;
      std::array<std::byte, sizeof(NodeApiStackOnlyPointerValue)> bytes_;
      StoreType() {}
      ~StoreType() {}
    };

   public:
    JsiValueView(NodeApiJsiRuntime *runtime, napi_value jsValue);
    operator const jsi::Value &() const noexcept;

    static jsi::Value
    initValue(NodeApiJsiRuntime *runtime, napi_value jsValue, StoreType *store);

    JsiValueView(const JsiValueView &) = delete;
    JsiValueView &operator=(const JsiValueView &) = delete;

   private:
    StoreType pointerStore_;
    jsi::Value value_{};
  };

  // Helps to use stack storage for passing arguments that must be temporarily
  // converted from napi_value to jsi::Value. It helps to avoid conversion to a
  // relatively expensive napi_ref.
  class JsiValueViewArgs {
   public:
    JsiValueViewArgs(
        NodeApiJsiRuntime *runtime,
        span<napi_value> args) noexcept;
    const jsi::Value *data() noexcept;
    size_t size() const noexcept;

    JsiValueViewArgs(const JsiValueViewArgs &);
    JsiValueViewArgs &operator=(const JsiValueViewArgs &);

   private:
    using StoreType = JsiValueView::StoreType;
    SmallBuffer<StoreType, MaxStackArgCount> pointerStore_{0};
    SmallBuffer<jsi::Value, MaxStackArgCount> args_{0};
  };

  // Helps to use stack storage for a temporary conversion from napi_value to
  // jsi::PropNameID. It helps to avoid conversions to a relatively expensive
  // napi_ref.
  class PropNameIDView {
   public:
    PropNameIDView(NodeApiJsiRuntime *runtime, napi_value propertyId) noexcept;
    operator const jsi::PropNameID &() const noexcept;

    PropNameIDView(const PropNameIDView &);
    PropNameIDView &operator=(const PropNameIDView &);

   private:
    using StoreType = JsiValueView::StoreType;
    StoreType pointerStore_{};
    jsi::PropNameID const propertyId_;
  };

  // Wraps up the jsi::HostFunctionType along with the NodeApiJsiRuntime.
  class HostFunctionWrapper {
   public:
    HostFunctionWrapper(
        jsi::HostFunctionType &&hostFunction,
        NodeApiJsiRuntime &runtime);

    jsi::HostFunctionType &hostFunction() noexcept;
    NodeApiJsiRuntime &runtime() noexcept;

    HostFunctionWrapper(const HostFunctionWrapper &) = delete;
    HostFunctionWrapper &operator=(const HostFunctionWrapper &) = delete;

   private:
    jsi::HostFunctionType hostFunction_;
    NodeApiJsiRuntime &runtime_;
  };

  // Wraps up the jsr_prepared_script.
  class NodeApiPreparedJavaScript final : public jsi::PreparedJavaScript {
   public:
    NodeApiPreparedJavaScript(
        napi_env env,
        jsr_prepared_script script,
        std::string sourceURL)
        : env_(env), script_(script), sourceURL_(std::move(sourceURL)) {}

    ~NodeApiPreparedJavaScript() override {
      JSRuntimeApi::current()->jsr_delete_prepared_script(env_, script_);
    }

    jsr_prepared_script getScript() const {
      return script_;
    }

    const std::string &sourceURL() const {
      return sourceURL_;
    }

    NodeApiPreparedJavaScript(const NodeApiPreparedJavaScript &) = delete;
    NodeApiPreparedJavaScript &operator=(const NodeApiPreparedJavaScript &) =
        delete;

   private:
    napi_env env_;
    jsr_prepared_script script_;
    std::string sourceURL_;
  };

 private: // Error-handling utility methods
  template <typename... Args>
  jsi::JSError makeJSError(Args &&...args);
  [[noreturn]] void throwJSException(napi_status errorCode) const;
  [[noreturn]] void throwNativeException(char const *errorMessage) const;
  void rewriteErrorMessage(napi_value jsError) const;
  template <typename TLambda>
  auto runInMethodContext(char const *methodName, TLambda lambda);
  template <typename TLambda>
  napi_value handleCallbackExceptions(TLambda lambda) const noexcept;
  bool setException(napi_value error) const noexcept;
  bool setException(std::string_view message) const noexcept;

 private: // Shared Node-API call helpers
  napi_valuetype typeOf(napi_value value) const;
  bool strictEquals(napi_value left, napi_value right) const;
  napi_value getUndefined() const;
  napi_value getNull() const;
  napi_value getGlobal() const;
  napi_value getBoolean(bool value) const;
  bool getValueBool(napi_value value) const;
  napi_value createInt32(int32_t value) const;
  napi_value createUInt32(uint32_t value) const;
  napi_value createDouble(double value) const;
  double getValueDouble(napi_value value) const;
  napi_value createStringLatin1(std::string_view value) const;
  napi_value createStringUtf8(std::string_view value) const;
  napi_value createStringUtf8(const uint8_t *data, size_t length) const;
  std::string stringToStdString(napi_value stringValue) const;
  napi_value getPropertyIdFromName(std::string_view value) const;
  napi_value getPropertyIdFromName(const uint8_t *data, size_t length) const;
  napi_value getPropertyIdFromName(napi_value str) const;
  napi_value getPropertyIdFromSymbol(napi_value sym) const;
  std::string propertyIdToStdString(napi_value propertyId);
  napi_value createSymbol(std::string_view symbolDescription) const;
  std::string symbolToStdString(napi_value symbolValue);
  napi_value callFunction(
      napi_value thisArg,
      napi_value function,
      span<napi_value> args = {}) const;
  napi_value constructObject(napi_value constructor, span<napi_value> args = {})
      const;
  bool instanceOf(napi_value object, napi_value constructor) const;
  napi_value createNodeApiObject() const;
  bool hasProperty(napi_value object, napi_value propertyId) const;
  napi_value getProperty(napi_value object, napi_value propertyId) const;
  void setProperty(napi_value object, napi_value propertyId, napi_value value)
      const;
  bool deleteProperty(napi_value object, napi_value propertyId) const;
  void setProperty(
      napi_value object,
      napi_value propertyId,
      napi_value value,
      napi_property_attributes attrs) const;
  napi_value createNodeApiArray(size_t length) const;
  bool isArray(napi_value value) const;
  size_t getArrayLength(napi_value value) const;
  napi_value getElement(napi_value arr, size_t index) const;
  void setElement(napi_value array, uint32_t index, napi_value value) const;
  static napi_value __cdecl jsiHostFunctionCallback(
      napi_env env,
      napi_callback_info info) noexcept;
  napi_value createExternalFunction(
      napi_value name,
      int32_t paramCount,
      napi_callback callback,
      void *callbackData);
  napi_value createExternalObject(
      void *data,
      node_api_nogc_finalize finalizeCallback) const;
  template <typename T>
  napi_value createExternalObject(std::unique_ptr<T> &&data) const;
  void *getExternalData(napi_value object) const;
  const std::shared_ptr<jsi::HostObject> &getJsiHostObject(napi_value obj);
  napi_value getHostObjectProxyHandler();
  template <
      napi_value (NodeApiJsiRuntime::*trapMethod)(span<napi_value>),
      size_t argCount>
  void setProxyTrap(napi_value handler, napi_value propertyName);
  napi_value hostObjectHasTrap(span<napi_value> args);
  napi_value hostObjectGetTrap(span<napi_value> args);
  napi_value hostObjectSetTrap(span<napi_value> args);
  napi_value hostObjectOwnKeysTrap(span<napi_value> args);
  napi_value hostObjectGetOwnPropertyDescriptorTrap(span<napi_value> args);

 private: // Miscellaneous utility methods
  span<const uint8_t> toSpan(const jsi::Buffer &buffer);
  jsi::Value toJsiValue(napi_value value) const;
  napi_value getNodeApiValue(const jsi::Value &value) const;
  napi_value getNodeApiValue(const jsi::Pointer &ptr) const;
  napi_value getNodeApiValue(const NodeApiRefHolder &ref) const;
  NodeApiRefCountedPointerValue *cloneNodeApiPointerValue(
      const PointerValue *pointerValue);
  std::optional<uint32_t> toArrayIndex(
      std::string::const_iterator first,
      std::string::const_iterator last);

  template <
      typename T,
      std::enable_if_t<std::is_same_v<jsi::Object, T>, int> = 0>
  T makeJsiPointer(napi_value value) const;
  template <
      typename T,
      std::enable_if_t<std::is_same_v<jsi::String, T>, int> = 0>
  T makeJsiPointer(napi_value value) const;
  template <
      typename T,
      std::enable_if_t<std::is_same_v<jsi::Symbol, T>, int> = 0>
  T makeJsiPointer(napi_value value) const;
#if JSI_VERSION >= 6
  template <
      typename T,
      std::enable_if_t<std::is_same_v<jsi::BigInt, T>, int> = 0>
  T makeJsiPointer(napi_value value) const;
#endif
  template <
      typename TTo,
      typename TFrom,
      std::enable_if_t<std::is_base_of_v<jsi::Pointer, TTo>, int> = 0,
      std::enable_if_t<std::is_base_of_v<jsi::Pointer, TFrom>, int> = 0>
  TTo cloneAs(const TFrom &pointer) const;
  NodeApiRefHolder makeNodeApiRef(
      napi_value value,
      NodeApiPointerValueKind pointerKind,
      int32_t initialRefCount = 1);

  void addStackValue(NodeApiStackValuePtr stackPointer);
  void pushPointerValueScope() noexcept;
  void popPointerValueScope() noexcept;

  napi_env getEnv() const noexcept {
    return env_;
  }

 private: // Fields
  napi_env env_{};
  JSRuntimeApi *jsrApi_;
  std::function<void()> onDelete_;
  std::string sourceURL_;

  // Property ID cache to improve execution speed.
  struct PropertyId {
    NodeApiRefHolder Error;
    NodeApiRefHolder Object;
    NodeApiRefHolder Proxy;
    NodeApiRefHolder Symbol;
    NodeApiRefHolder byteLength;
    NodeApiRefHolder configurable;
    NodeApiRefHolder enumerable;
    NodeApiRefHolder get;
    NodeApiRefHolder getOwnPropertyDescriptor;
    NodeApiRefHolder has;
    NodeApiRefHolder hostFunctionSymbol;
    NodeApiRefHolder hostObjectSymbol;
    NodeApiRefHolder length;
    NodeApiRefHolder message;
    NodeApiRefHolder ownKeys;
    NodeApiRefHolder propertyIsEnumerable;
    NodeApiRefHolder prototype;
    NodeApiRefHolder set;
    NodeApiRefHolder stack;
    NodeApiRefHolder toString;
    NodeApiRefHolder value;
    NodeApiRefHolder writable;
  } propertyId_;

  // Cache of commonly used values.
  struct CachedValue {
    NodeApiRefHolder Error;
    NodeApiRefHolder Global;
    NodeApiRefHolder HostObjectProxyHandler;
    NodeApiRefHolder ProxyConstructor;
    NodeApiRefHolder SymbolToString;
  } cachedValue_;

  bool hasPendingJSError_{false};

  std::vector<size_t> stackScopes_;
  std::vector<NodeApiStackValuePtr> stackValues_;

  // TODO: implement GC for propNameIDs_
  std::unordered_map<
      StringKey,
      NodeApiRefHolder,
      StringKey::Hash,
      StringKey::EqualTo>
      propNameIDs_;

  NodeApiJsiRuntime &runtime{*this};
  NodeApiRefCountedPtr<NodeApiPendingDeletions> pendingDeletions_{
      NodeApiPendingDeletions::create()};
};

//=====================================================================================================================
// StringKey implementation
//=====================================================================================================================

StringKey::StringKey(std::string &&string) noexcept
    : string_(std::move(string)),
      type_(Type::String),
      hash_(std::hash<std::string_view>{}(string_)) {}

StringKey::StringKey(std::string_view view) noexcept
    : view_(view),
      type_(Type::View),
      hash_(std::hash<std::string_view>{}(view_)) {}

StringKey::StringKey(const char *data, size_t length) noexcept
    : view_(data, length),
      type_(Type::View),
      hash_(std::hash<std::string_view>{}(view_)) {}

StringKey::StringKey(StringKey &&other) noexcept
    : type_(other.type_), hash_(std::exchange(other.hash_, 0)) {
  if (type_ == Type::String) {
    ::new (std::addressof(string_)) std::string(std::move(other.string_));
  } else {
    ::new (std::addressof(view_))
        std::string_view(std::exchange(other.view_, std::string_view()));
  }
}

StringKey &StringKey::operator=(StringKey &&other) noexcept {
  if (this != &other) {
    this->~StringKey();
    ::new (this) StringKey(std::move(other));
  }
  return *this;
}

StringKey::~StringKey() {
  if (type_ == Type::String) {
    std::addressof(string_)->~basic_string();
  } else {
    std::addressof(view_)->~basic_string_view();
  }
}

std::string_view StringKey::getStringView() const {
  return (type_ == Type::String) ? std::string_view(string_) : view_;
}

bool StringKey::equalTo(const StringKey &other) const {
  return getStringView().compare(other.getStringView()) == 0;
}

size_t StringKey::hash() const {
  return hash_;
}

size_t StringKey::Hash::operator()(const StringKey &key) const {
  return key.hash();
}

bool StringKey::EqualTo::operator()(
    const StringKey &left,
    const StringKey &right) const {
  return left.equalTo(right);
}

//=====================================================================================================================
// NodeApiJsiRuntime implementation
//=====================================================================================================================

NodeApiJsiRuntime::NodeApiJsiRuntime(
    napi_env env,
    JSRuntimeApi *jsrApi,
    std::function<void()> onDelete) noexcept
    : env_(env), jsrApi_(jsrApi), onDelete_(std::move(onDelete)) {
  NodeApiScope scope{*this};
  propertyId_.Error = makeNodeApiRef(
      getPropertyIdFromName("Error"), NodeApiPointerValueKind::String);
  propertyId_.Object = makeNodeApiRef(
      getPropertyIdFromName("Object"), NodeApiPointerValueKind::String);
  propertyId_.Proxy = makeNodeApiRef(
      getPropertyIdFromName("Proxy"), NodeApiPointerValueKind::String);
  propertyId_.Symbol = makeNodeApiRef(
      getPropertyIdFromName("Symbol"), NodeApiPointerValueKind::String);
  propertyId_.byteLength = makeNodeApiRef(
      getPropertyIdFromName("byteLength"), NodeApiPointerValueKind::String);
  propertyId_.configurable = makeNodeApiRef(
      getPropertyIdFromName("configurable"), NodeApiPointerValueKind::String);
  propertyId_.enumerable = makeNodeApiRef(
      getPropertyIdFromName("enumerable"), NodeApiPointerValueKind::String);
  propertyId_.get = makeNodeApiRef(
      getPropertyIdFromName("get"), NodeApiPointerValueKind::String);
  propertyId_.getOwnPropertyDescriptor = makeNodeApiRef(
      getPropertyIdFromName("getOwnPropertyDescriptor"),
      NodeApiPointerValueKind::String);
  propertyId_.has = makeNodeApiRef(
      getPropertyIdFromName("has"), NodeApiPointerValueKind::String);
  propertyId_.hostFunctionSymbol = makeNodeApiRef(
      createSymbol("hostFunctionSymbol"), NodeApiPointerValueKind::Symbol);
  propertyId_.hostObjectSymbol = makeNodeApiRef(
      createSymbol("hostObjectSymbol"), NodeApiPointerValueKind::Symbol);
  propertyId_.length = makeNodeApiRef(
      getPropertyIdFromName("length"), NodeApiPointerValueKind::String);
  propertyId_.message = makeNodeApiRef(
      getPropertyIdFromName("message"), NodeApiPointerValueKind::String);
  propertyId_.ownKeys = makeNodeApiRef(
      getPropertyIdFromName("ownKeys"), NodeApiPointerValueKind::String);
  propertyId_.propertyIsEnumerable = makeNodeApiRef(
      getPropertyIdFromName("propertyIsEnumerable"),
      NodeApiPointerValueKind::String);
  propertyId_.prototype = makeNodeApiRef(
      getPropertyIdFromName("prototype"), NodeApiPointerValueKind::String);
  propertyId_.set = makeNodeApiRef(
      getPropertyIdFromName("set"), NodeApiPointerValueKind::String);
  propertyId_.stack = makeNodeApiRef(
      getPropertyIdFromName("stack"), NodeApiPointerValueKind::String);
  propertyId_.toString = makeNodeApiRef(
      getPropertyIdFromName("toString"), NodeApiPointerValueKind::String);
  propertyId_.value = makeNodeApiRef(
      getPropertyIdFromName("value"), NodeApiPointerValueKind::String);
  propertyId_.writable = makeNodeApiRef(
      getPropertyIdFromName("writable"), NodeApiPointerValueKind::String);

  cachedValue_.Global =
      makeNodeApiRef(getGlobal(), NodeApiPointerValueKind::Object);
  cachedValue_.Error = makeNodeApiRef(
      getProperty(
          getNodeApiValue(cachedValue_.Global),
          getNodeApiValue(propertyId_.Error)),
      NodeApiPointerValueKind::Object);
}

NodeApiJsiRuntime::~NodeApiJsiRuntime() {
  if (onDelete_) {
    onDelete_();
  }
}

jsi::Value NodeApiJsiRuntime::evaluateJavaScript(
    const std::shared_ptr<const jsi::Buffer> &buffer,
    const std::string &sourceURL) {
  return evaluatePreparedJavaScript(prepareJavaScript(buffer, sourceURL));
}

std::shared_ptr<const jsi::PreparedJavaScript>
NodeApiJsiRuntime::prepareJavaScript(
    const std::shared_ptr<const jsi::Buffer> &sourceBuffer,
    std::string sourceURL) {
  NodeApiScope scope{*this};
  jsr_prepared_script script{};
  napi_status status = jsrApi_->jsr_create_prepared_script(
      env_,
      sourceBuffer->data(),
      sourceBuffer->size(),
      [](void * /*data*/, void *deleterData) {
        delete reinterpret_cast<std::shared_ptr<const jsi::Buffer> *>(
            deleterData);
      },
      new std::shared_ptr<const jsi::Buffer>(sourceBuffer),
      sourceURL.c_str(),
      &script);
  CHECK_NAPI(status); // Not for the call to keep better automated formatting.
  return std::make_shared<NodeApiPreparedJavaScript>(
      env_, script, std::move(sourceURL));
}

jsi::Value NodeApiJsiRuntime::evaluatePreparedJavaScript(
    const std::shared_ptr<const jsi::PreparedJavaScript> &js) {
  NodeApiScope scope{*this};
  auto preparedScript =
      static_cast<const NodeApiPreparedJavaScript *>(js.get());
  AutoRestore<std::string> sourceURLScope{
      sourceURL_, preparedScript->sourceURL()};
  napi_value result{};
  CHECK_NAPI(jsrApi_->jsr_prepared_script_run(
      env_, preparedScript->getScript(), &result));
  return toJsiValue(result);
}

#if JSI_VERSION >= 12
void NodeApiJsiRuntime::queueMicrotask(const jsi::Function &callback) {
  NodeApiScope scope{*this};
  napi_value callbackValue = getNodeApiValue(callback);
  CHECK_NAPI(jsrApi_->jsr_queue_microtask(env_, callbackValue));
}
#endif

#if JSI_VERSION >= 4
bool NodeApiJsiRuntime::drainMicrotasks(int maxMicrotasksHint) {
  bool result{};
  CHECK_NAPI(jsrApi_->jsr_drain_microtasks(env_, maxMicrotasksHint, &result));
  return result;
}
#endif

jsi::Object NodeApiJsiRuntime::global() {
  return make<jsi::Object>(cachedValue_.Global->clone(*this));
}

std::string NodeApiJsiRuntime::description() {
  const char *desc{};
  CHECK_NAPI(jsrApi_->jsr_get_description(env_, &desc));
  return desc;
}

bool NodeApiJsiRuntime::isInspectable() {
  bool result{};
  CHECK_NAPI(jsrApi_->jsr_is_inspectable(env_, &result));
  return result;
}

jsi::Runtime::PointerValue *NodeApiJsiRuntime::cloneSymbol(
    const jsi::Runtime::PointerValue *pointerValue) {
  return cloneNodeApiPointerValue(pointerValue);
}

#if JSI_VERSION >= 6
jsi::Runtime::PointerValue *NodeApiJsiRuntime::cloneBigInt(
    const jsi::Runtime::PointerValue *pointerValue) {
  return cloneNodeApiPointerValue(pointerValue);
}
#endif

jsi::Runtime::PointerValue *NodeApiJsiRuntime::cloneString(
    const jsi::Runtime::PointerValue *pointerValue) {
  return cloneNodeApiPointerValue(pointerValue);
}

jsi::Runtime::PointerValue *NodeApiJsiRuntime::cloneObject(
    const jsi::Runtime::PointerValue *pointerValue) {
  return cloneNodeApiPointerValue(pointerValue);
}

jsi::Runtime::PointerValue *NodeApiJsiRuntime::clonePropNameID(
    const jsi::Runtime::PointerValue *pointerValue) {
  return cloneNodeApiPointerValue(pointerValue);
}

jsi::PropNameID NodeApiJsiRuntime::createPropNameIDFromAscii(
    const char *str,
    size_t length) {
  NodeApiScope scope{*this};
  StringKey keyName{str, length};
  auto it = propNameIDs_.find(keyName);
  if (it != propNameIDs_.end()) {
    return make<jsi::PropNameID>(it->second->clone(*this));
  }

  napi_value obj = createNodeApiObject();
  napi_value propName{};
  CHECK_NAPI(jsrApi_->napi_create_string_latin1(env_, str, length, &propName));
  CHECK_NAPI(jsrApi_->napi_set_property(env_, obj, propName, getUndefined()));
  napi_value props{};
  CHECK_NAPI(jsrApi_->napi_get_all_property_names(
      env_,
      obj,
      napi_key_own_only,
      napi_key_skip_symbols,
      napi_key_numbers_to_strings,
      &props));
  napi_value propNameId = getElement(props, 0);
  NodeApiRefHolder propNameRef =
      makeNodeApiRef(propNameId, NodeApiPointerValueKind::StringPropNameID, 2);
  jsi::PropNameID result = make<jsi::PropNameID>(propNameRef.get());
  propNameIDs_.try_emplace(
      StringKey(std::string(keyName.getStringView())), std::move(propNameRef));
  return result;
}

jsi::PropNameID NodeApiJsiRuntime::createPropNameIDFromUtf8(
    const uint8_t *utf8,
    size_t length) {
  NodeApiScope scope{*this};
  StringKey keyName{reinterpret_cast<const char *>(utf8), length};
  auto it = propNameIDs_.find(keyName);
  if (it != propNameIDs_.end()) {
    return make<jsi::PropNameID>(it->second->clone(*this));
  }

  napi_value obj = createNodeApiObject();
  napi_value propName{};
  CHECK_NAPI(jsrApi_->napi_create_string_utf8(
      env_, reinterpret_cast<const char *>(utf8), length, &propName));
  CHECK_NAPI(jsrApi_->napi_set_property(env_, obj, propName, getUndefined()));
  napi_value props{};
  CHECK_NAPI(jsrApi_->napi_get_all_property_names(
      env_,
      obj,
      napi_key_own_only,
      napi_key_skip_symbols,
      napi_key_numbers_to_strings,
      &props));
  napi_value propNameId = getElement(props, 0);
  NodeApiRefHolder propNameRef =
      makeNodeApiRef(propNameId, NodeApiPointerValueKind::StringPropNameID, 2);
  jsi::PropNameID result = make<jsi::PropNameID>(propNameRef.get());
  propNameIDs_.try_emplace(
      StringKey(std::string(keyName.getStringView())), std::move(propNameRef));
  return result;
}

jsi::PropNameID NodeApiJsiRuntime::createPropNameIDFromString(
    const jsi::String &str) {
  NodeApiScope scope{*this};
  const NodeApiPointerValue *pv =
      static_cast<const NodeApiPointerValue *>(getPointerValue(str));
  if (pv->getKind() == NodeApiPointerValueKind::StringPropNameID) {
    return make<jsi::PropNameID>(pv->clone(*this));
  }

  StringKey keyName(utf8(str));
  auto it = propNameIDs_.find(keyName);
  if (it != propNameIDs_.end()) {
    return make<jsi::PropNameID>(it->second->clone(*this));
  }

  napi_value napiStr = const_cast<NodeApiPointerValue *>(pv)->getValue(*this);

  napi_value obj = createNodeApiObject();
  setProperty(obj, napiStr, getUndefined());
  napi_value props{};
  CHECK_NAPI(jsrApi_->napi_get_all_property_names(
      env_,
      obj,
      napi_key_own_only,
      napi_key_skip_symbols,
      napi_key_numbers_to_strings,
      &props));
  napi_value propNameId = getElement(props, 0);
  NodeApiRefHolder propNameRef =
      makeNodeApiRef(propNameId, NodeApiPointerValueKind::StringPropNameID, 2);
  jsi::PropNameID result = make<jsi::PropNameID>(propNameRef.get());
  propNameIDs_.try_emplace(
      StringKey(std::string(keyName.getStringView())), std::move(propNameRef));
  return result;
}

#if JSI_VERSION >= 5
jsi::PropNameID NodeApiJsiRuntime::createPropNameIDFromSymbol(
    const jsi::Symbol &sym) {
  // TODO: Should we ensure uniqueness of symbols?
  return cloneAs<jsi::PropNameID>(sym);
}
#endif

std::string NodeApiJsiRuntime::utf8(const jsi::PropNameID &id) {
  NodeApiScope scope{*this};
  return propertyIdToStdString(getNodeApiValue(id));
}

bool NodeApiJsiRuntime::compare(
    const jsi::PropNameID &lhs,
    const jsi::PropNameID &rhs) {
  NodeApiScope scope{*this};
  return getPointerValue(lhs) == getPointerValue(rhs) ||
      strictEquals(getNodeApiValue(lhs), getNodeApiValue(rhs));
}

std::string NodeApiJsiRuntime::symbolToString(const jsi::Symbol &sym) {
  NodeApiScope scope{*this};
  return symbolToStdString(getNodeApiValue(sym));
}

#if JSI_VERSION >= 8
jsi::BigInt NodeApiJsiRuntime::createBigIntFromInt64(int64_t val) {
  NodeApiScope scope{*this};
  napi_value bigint{};
  CHECK_NAPI(jsrApi_->napi_create_bigint_int64(env_, val, &bigint));
  return makeJsiPointer<jsi::BigInt>(bigint);
}

jsi::BigInt NodeApiJsiRuntime::createBigIntFromUint64(uint64_t val) {
  NodeApiScope scope{*this};
  napi_value bigint{};
  CHECK_NAPI(jsrApi_->napi_create_bigint_uint64(env_, val, &bigint));
  return makeJsiPointer<jsi::BigInt>(bigint);
}

bool NodeApiJsiRuntime::bigintIsInt64(const jsi::BigInt &bigint) {
  NodeApiScope scope{*this};
  napi_value value = getNodeApiValue(bigint);
  bool lossless{false};
  int64_t result{};
  CHECK_NAPI(
      jsrApi_->napi_get_value_bigint_int64(env_, value, &result, &lossless));
  return lossless;
}

bool NodeApiJsiRuntime::bigintIsUint64(const jsi::BigInt &bigint) {
  NodeApiScope scope{*this};
  napi_value value = getNodeApiValue(bigint);
  bool lossless{false};
  uint64_t result{};
  CHECK_NAPI(
      jsrApi_->napi_get_value_bigint_uint64(env_, value, &result, &lossless));
  return lossless;
}

uint64_t NodeApiJsiRuntime::truncate(const jsi::BigInt &bigint) {
  NodeApiScope scope{*this};
  napi_value value = getNodeApiValue(bigint);
  bool lossless{false};
  uint64_t result{};
  CHECK_NAPI(
      jsrApi_->napi_get_value_bigint_uint64(env_, value, &result, &lossless));
  return result;
}

inline uint32_t constexpr maxCharsPerDigitInRadix(int32_t radix) {
  // To compute the lower bound of bits in a BigIntDigitType "covered" by a
  // char. For power of 2 radixes, it is known (exactly) that each character
  // covers log2(radix) bits. For non-power of 2 radixes, a lower bound is
  // log2(greatest power of 2 that is less than radix).
  uint32_t minNumBitsPerChar = radix < 4 ? 1
      : radix < 8                        ? 2
      : radix < 16                       ? 3
      : radix < 32                       ? 4
                                         : 5;

  // With minNumBitsPerChar being the lower bound estimate of how many bits each
  // char can represent, the upper bound of how many chars "fit" in a bigint
  // digit is ceil(sizeofInBits(bigint digit) / minNumBitsPerChar).
  uint32_t numCharsPerDigits =
      static_cast<uint32_t>(sizeof(uint64_t)) / (1 << minNumBitsPerChar);

  return numCharsPerDigits;
}

// Return the high 32 bits of a 64 bit value.
constexpr inline uint32_t Hi_32(uint64_t Value) {
  return static_cast<uint32_t>(Value >> 32);
}

// Return the low 32 bits of a 64 bit value.
constexpr inline uint32_t Lo_32(uint64_t Value) {
  return static_cast<uint32_t>(Value);
}

// Make a 64-bit integer from a high / low pair of 32-bit integers.
constexpr inline uint64_t Make_64(uint32_t High, uint32_t Low) {
  return ((uint64_t)High << 32) | (uint64_t)Low;
}

jsi::String NodeApiJsiRuntime::bigintToString(
    const jsi::BigInt &bigint,
    int32_t radix) {
  NodeApiScope scope{*this};
  if (radix < 2 || radix > 36) {
    throw makeJSError("Invalid radix ", radix, " to BigInt.toString");
  }

  napi_value value = getNodeApiValue(bigint);
  size_t wordCount{};
  CHECK_NAPI(jsrApi_->napi_get_value_bigint_words(
      env_, value, nullptr, &wordCount, nullptr));
  uint64_t stackWords[8]{};
  std::unique_ptr<uint64_t[]> heapWords;
  uint64_t *words = stackWords;
  if (wordCount > std::size(stackWords)) {
    heapWords = std::unique_ptr<uint64_t[]>(new uint64_t[wordCount]);
    words = heapWords.get();
  }
  int32_t signBit{};
  CHECK_NAPI(jsrApi_->napi_get_value_bigint_words(
      env_, value, &signBit, &wordCount, words));

  if (wordCount == 0) {
    return createStringFromAscii("0", 1);
  }

  // avoid trashing the heap by pre-allocating the largest possible string
  // returned by this function. The "1" below is to account for a possible "-"
  // sign.
  std::string digits;
  digits.reserve(1 + wordCount * maxCharsPerDigitInRadix(radix));

  // Use 32-bit values for calculations to get 64-bit results.
  // For the little-endian machines we just cast the words array.
  // TODO: Add support for big-endian.
  uint32_t *halfWords = reinterpret_cast<uint32_t *>(words);
  size_t count = wordCount * 2;
  for (size_t i = count; i > 0 && halfWords[i - 1] == 0; --i) {
    --count;
  }

  uint32_t divisor = static_cast<uint32_t>(radix);
  uint32_t remainder = 0;
  uint64_t word0 = words[0];

  do {
    // We rewrite the halfWords array as we divide it by radix.
    if (count <= 2) {
      remainder = word0 % divisor;
      word0 = word0 / divisor;
    } else {
      for (size_t i = count; i > 0; --i) {
        uint64_t partialDividend = Make_64(remainder, halfWords[i - 1]);
        if (partialDividend == 0) {
          halfWords[i] = 0;
          remainder = 0;
          if (i == count) {
            if (--count == 2) {
              word0 = words[0];
            }
          }
        } else if (partialDividend < divisor) {
          halfWords[i] = 0;
          remainder = Lo_32(partialDividend);
          if (i == count) {
            if (--count == 2) {
              word0 = words[0];
            }
          }
        } else if (partialDividend == divisor) {
          halfWords[i] = 1;
          remainder = 0;
        } else {
          halfWords[i] = Lo_32(partialDividend / divisor);
          remainder = Lo_32(partialDividend % divisor);
        }
      }
    }

    if (remainder < 10) {
      digits.push_back(static_cast<char>('0' + remainder));
    } else {
      digits.push_back(static_cast<char>('a' + remainder - 10));
    }
  } while (count > 2 || word0 != 0);

  if (signBit) {
    digits.push_back('-');
  }

  std::reverse(digits.begin(), digits.end());
  return createStringFromAscii(digits.data(), digits.size());
}
#endif

jsi::String NodeApiJsiRuntime::createStringFromAscii(
    const char *str,
    size_t length) {
  NodeApiScope scope{*this};
  return makeJsiPointer<jsi::String>(createStringLatin1({str, length}));
}

jsi::String NodeApiJsiRuntime::createStringFromUtf8(
    const uint8_t *str,
    size_t length) {
  NodeApiScope scope{*this};
  return makeJsiPointer<jsi::String>(createStringUtf8(str, length));
}

std::string NodeApiJsiRuntime::utf8(const jsi::String &str) {
  NodeApiScope scope{*this};
  return stringToStdString(getNodeApiValue(str));
}

jsi::Object NodeApiJsiRuntime::createObject() {
  NodeApiScope scope{*this};
  return makeJsiPointer<jsi::Object>(createNodeApiObject());
}

jsi::Object NodeApiJsiRuntime::createObject(
    std::shared_ptr<jsi::HostObject> hostObject) {
  NodeApiScope scope{*this};
  // The hostObjectHolder keeps the hostObject as external data.
  // Then, the hostObjectHolder is wrapped up by a Proxy object to provide
  // access to the hostObject's get, set, and getPropertyNames methods. There is
  // a special symbol property ID, 'hostObjectSymbol', used to access the
  // hostObjectWrapper from the Proxy.
  napi_value hostObjectHolder = createExternalObject(
      std::make_unique<std::shared_ptr<jsi::HostObject>>(
          std::move(hostObject)));
  napi_value obj = createNodeApiObject();
  setProperty(
      obj, getNodeApiValue(propertyId_.hostObjectSymbol), hostObjectHolder);
  if (!cachedValue_.ProxyConstructor) {
    cachedValue_.ProxyConstructor = makeNodeApiRef(
        getProperty(
            getNodeApiValue(cachedValue_.Global),
            getNodeApiValue(propertyId_.Proxy)),
        NodeApiPointerValueKind::Object);
  }
  napi_value args[] = {obj, getHostObjectProxyHandler()};
  napi_value proxy =
      constructObject(getNodeApiValue(cachedValue_.ProxyConstructor), args);
  return makeJsiPointer<jsi::Object>(proxy);
}

std::shared_ptr<jsi::HostObject> NodeApiJsiRuntime::getHostObject(
    const jsi::Object &obj) {
  NodeApiScope scope{*this};
  return getJsiHostObject(getNodeApiValue(obj));
}

jsi::HostFunctionType &NodeApiJsiRuntime::getHostFunction(
    const jsi::Function &func) {
  NodeApiScope scope{*this};
  napi_value hostFunctionHolder = getProperty(
      getNodeApiValue(func), getNodeApiValue((propertyId_.hostFunctionSymbol)));
  if (typeOf(hostFunctionHolder) == napi_valuetype::napi_external) {
    return static_cast<HostFunctionWrapper *>(
               getExternalData(hostFunctionHolder))
        ->hostFunction();
  } else {
    throw jsi::JSINativeException(
        "getHostFunction() can only be called with HostFunction.");
  }
}

#if JSI_VERSION >= 7
bool NodeApiJsiRuntime::hasNativeState(const jsi::Object &obj) {
  NodeApiScope scope{*this};
  void *nativeState{};
  napi_status status =
      jsrApi_->napi_unwrap(env_, getNodeApiValue(obj), &nativeState);
  return status == napi_ok && nativeState != nullptr;
}

std::shared_ptr<jsi::NativeState> NodeApiJsiRuntime::getNativeState(
    const jsi::Object &obj) {
  NodeApiScope scope{*this};
  void *nativeState{};
  CHECK_NAPI(jsrApi_->napi_unwrap(env_, getNodeApiValue(obj), &nativeState));
  if (nativeState != nullptr) {
    return *reinterpret_cast<std::shared_ptr<jsi::NativeState> *>(nativeState);
  } else {
    return std::shared_ptr<jsi::NativeState>();
  }
}

void NodeApiJsiRuntime::setNativeState(
    const jsi::Object &obj,
    std::shared_ptr<jsi::NativeState> state) {
  NodeApiScope scope{*this};
  if (hasNativeState(obj)) {
    void *nativeState{};
    CHECK_NAPI(
        jsrApi_->napi_remove_wrap(env_, getNodeApiValue(obj), &nativeState));
    if (nativeState != nullptr) {
      std::shared_ptr<jsi::NativeState> oldState{std::move(
          *reinterpret_cast<std::shared_ptr<jsi::NativeState> *>(nativeState))};
    }
  }

  if (state) {
    CHECK_NAPI(jsrApi_->napi_wrap(
        env_,
        getNodeApiValue(obj),
        new std::shared_ptr<jsi::NativeState>(std::move(state)),
        [](node_api_nogc_env /*env*/, void *data, void * /*finalize_hint*/) {
          std::shared_ptr<jsi::NativeState> oldState{std::move(
              *reinterpret_cast<std::shared_ptr<jsi::NativeState> *>(data))};
        },
        nullptr,
        nullptr));
  }
}
#endif

jsi::Value NodeApiJsiRuntime::getProperty(
    const jsi::Object &obj,
    const jsi::PropNameID &name) {
  NodeApiScope scope{*this};
  return toJsiValue(getProperty(getNodeApiValue(obj), getNodeApiValue(name)));
}

jsi::Value NodeApiJsiRuntime::getProperty(
    const jsi::Object &obj,
    const jsi::String &name) {
  NodeApiScope scope{*this};
  return toJsiValue(getProperty(getNodeApiValue(obj), getNodeApiValue(name)));
}

#if JSI_VERSION >= 21
jsi::Value NodeApiJsiRuntime::getProperty(
    const jsi::Object &obj,
    const jsi::Value &name) {
  NodeApiScope scope{*this};
  return toJsiValue(getProperty(getNodeApiValue(obj), getNodeApiValue(name)));
}
#endif

bool NodeApiJsiRuntime::hasProperty(
    const jsi::Object &obj,
    const jsi::PropNameID &name) {
  NodeApiScope scope{*this};
  return hasProperty(getNodeApiValue(obj), getNodeApiValue(name));
}

bool NodeApiJsiRuntime::hasProperty(
    const jsi::Object &obj,
    const jsi::String &name) {
  NodeApiScope scope{*this};
  return hasProperty(getNodeApiValue(obj), getNodeApiValue(name));
}

#if JSI_VERSION >= 21
bool NodeApiJsiRuntime::hasProperty(
    const jsi::Object &obj,
    const jsi::Value &name) {
  NodeApiScope scope{*this};
  return hasProperty(getNodeApiValue(obj), getNodeApiValue(name));
}
#endif

void NodeApiJsiRuntime::setPropertyValue(
    JSI_CONST_10 jsi::Object &obj,
    const jsi::PropNameID &name,
    const jsi::Value &value) {
  NodeApiScope scope{*this};
  setProperty(
      getNodeApiValue(obj), getNodeApiValue(name), getNodeApiValue(value));
}

void NodeApiJsiRuntime::setPropertyValue(
    JSI_CONST_10 jsi::Object &obj,
    const jsi::String &name,
    const jsi::Value &value) {
  NodeApiScope scope{*this};
  setProperty(
      getNodeApiValue(obj), getNodeApiValue(name), getNodeApiValue(value));
}

#if JSI_VERSION >= 21
void NodeApiJsiRuntime::setPropertyValue(
    const jsi::Object &obj,
    const jsi::Value &name,
    const jsi::Value &value) {
  NodeApiScope scope{*this};
  setProperty(
      getNodeApiValue(obj), getNodeApiValue(name), getNodeApiValue(value));
}

void NodeApiJsiRuntime::deleteProperty(
    const jsi::Object &obj,
    const jsi::PropNameID &name) {
  NodeApiScope scope{*this};
  bool res = deleteProperty(getNodeApiValue(obj), getNodeApiValue(name));
  if (!res) {
    throw jsi::JSError(*this, "Failed to delete property");
  }
}

void NodeApiJsiRuntime::deleteProperty(
    const jsi::Object &obj,
    const jsi::String &name) {
  NodeApiScope scope{*this};
  bool res = deleteProperty(getNodeApiValue(obj), getNodeApiValue(name));
  if (!res) {
    throw jsi::JSError(*this, "Failed to delete property");
  }
}

void NodeApiJsiRuntime::deleteProperty(
    const jsi::Object &obj,
    const jsi::Value &name) {
  NodeApiScope scope{*this};
  bool res = deleteProperty(getNodeApiValue(obj), getNodeApiValue(name));
  if (!res) {
    throw jsi::JSError(*this, "Failed to delete property");
  }
}
#endif

bool NodeApiJsiRuntime::isArray(const jsi::Object &obj) const {
  NodeApiScope scope{*this};
  return isArray(getNodeApiValue(obj));
}

bool NodeApiJsiRuntime::isArrayBuffer(const jsi::Object &obj) const {
  NodeApiScope scope{*this};
  bool result{};
  CHECK_NAPI(jsrApi_->napi_is_arraybuffer(env_, getNodeApiValue(obj), &result));
  return result;
}

bool NodeApiJsiRuntime::isFunction(const jsi::Object &obj) const {
  NodeApiScope scope{*this};
  return typeOf(getNodeApiValue(obj)) == napi_valuetype::napi_function;
}

bool NodeApiJsiRuntime::isHostObject(const jsi::Object &obj) const {
  NodeApiScope scope{*this};
  napi_value hostObjectHolder = getProperty(
      getNodeApiValue(obj), getNodeApiValue(propertyId_.hostObjectSymbol));
  if (typeOf(hostObjectHolder) == napi_valuetype::napi_external) {
    return getExternalData(hostObjectHolder) != nullptr;
  } else {
    return false;
  }
}

bool NodeApiJsiRuntime::isHostFunction(const jsi::Function &func) const {
  NodeApiScope scope{*this};
  napi_value hostFunctionHolder = getProperty(
      getNodeApiValue(func), getNodeApiValue(propertyId_.hostFunctionSymbol));
  if (typeOf(hostFunctionHolder) == napi_valuetype::napi_external) {
    return getExternalData(hostFunctionHolder) != nullptr;
  } else {
    return false;
  }
}

jsi::Array NodeApiJsiRuntime::getPropertyNames(const jsi::Object &obj) {
  NodeApiScope scope{*this};
  napi_value properties;
  CHECK_NAPI(jsrApi_->napi_get_all_property_names(
      env_,
      getNodeApiValue(obj),
      napi_key_collection_mode::napi_key_include_prototypes,
      napi_key_filter(napi_key_enumerable | napi_key_skip_symbols),
      napi_key_numbers_to_strings,
      &properties));
  return makeJsiPointer<jsi::Object>(properties).asArray(*this);
}

jsi::WeakObject NodeApiJsiRuntime::createWeakObject(const jsi::Object &obj) {
  NodeApiScope scope{*this};
  return make<jsi::WeakObject>(NodeApiRefCountedPointerValue::make(
                                   *const_cast<NodeApiJsiRuntime *>(this),
                                   getNodeApiValue(obj),
                                   NodeApiPointerValueKind::WeakObject)
                                   .release());
}

jsi::Value NodeApiJsiRuntime::lockWeakObject(
    JSI_NO_CONST_3 JSI_CONST_10 jsi::WeakObject &weakObject) {
  NodeApiScope scope{*this};
  napi_value value = getNodeApiValue(weakObject);
  if (value) {
    return toJsiValue(value);
  } else {
    return jsi::Value::undefined();
  }
}

jsi::Array NodeApiJsiRuntime::createArray(size_t length) {
  NodeApiScope scope{*this};
  return makeJsiPointer<jsi::Object>(createNodeApiArray(length)).asArray(*this);
}

#if JSI_VERSION >= 9
jsi::ArrayBuffer NodeApiJsiRuntime::createArrayBuffer(
    std::shared_ptr<jsi::MutableBuffer> buffer) {
  NodeApiScope scope{*this};
  napi_value result{};
  void *data = buffer->data();
  size_t size = buffer->size();
  CHECK_NAPI(jsrApi_->napi_create_external_arraybuffer(
      env_,
      data,
      size,
      [](node_api_nogc_env /*env*/, void * /*data*/, void *finalizeHint) {
        std::shared_ptr<jsi::MutableBuffer> buffer{
            std::move(*reinterpret_cast<std::shared_ptr<jsi::MutableBuffer> *>(
                finalizeHint))};
      },
      new std::shared_ptr<jsi::MutableBuffer>{std::move(buffer)},
      &result));
  return makeJsiPointer<jsi::Object>(result).getArrayBuffer(*this);
}
#endif

size_t NodeApiJsiRuntime::size(const jsi::Array &arr) {
  NodeApiScope scope{*this};
  return getArrayLength(getNodeApiValue(arr));
}

size_t NodeApiJsiRuntime::size(const jsi::ArrayBuffer &arrBuf) {
  NodeApiScope scope{*this};
  size_t result{};
  CHECK_NAPI(jsrApi_->napi_get_arraybuffer_info(
      env_, getNodeApiValue(arrBuf), nullptr, &result));
  return result;
}

uint8_t *NodeApiJsiRuntime::data(const jsi::ArrayBuffer &arrBuf) {
  NodeApiScope scope{*this};
  uint8_t *result{};
  CHECK_NAPI(jsrApi_->napi_get_arraybuffer_info(
      env_,
      getNodeApiValue(arrBuf),
      reinterpret_cast<void **>(&result),
      nullptr));
  return result;
}

jsi::Value NodeApiJsiRuntime::getValueAtIndex(
    const jsi::Array &arr,
    size_t index) {
  NodeApiScope scope{*this};
  return toJsiValue(getElement(getNodeApiValue(arr), index));
}

void NodeApiJsiRuntime::setValueAtIndexImpl(
    JSI_CONST_10 jsi::Array &arr,
    size_t index,
    const jsi::Value &value) {
  NodeApiScope scope{*this};
  setElement(
      getNodeApiValue(arr),
      static_cast<uint32_t>(index),
      getNodeApiValue(value));
}

jsi::Function NodeApiJsiRuntime::createFunctionFromHostFunction(
    const jsi::PropNameID &name,
    unsigned int paramCount,
    jsi::HostFunctionType func) {
  NodeApiScope scope{*this};
  auto hostFunctionWrapper =
      std::make_unique<HostFunctionWrapper>(std::move(func), *this);
  napi_value function = createExternalFunction(
      getNodeApiValue(name),
      static_cast<int32_t>(paramCount),
      jsiHostFunctionCallback,
      hostFunctionWrapper.get());

  const napi_value hostFunctionHolder =
      createExternalObject(std::move(hostFunctionWrapper));
  setProperty(
      function,
      getNodeApiValue(propertyId_.hostFunctionSymbol),
      hostFunctionHolder,
      napi_property_attributes::napi_default);
  return makeJsiPointer<jsi::Object>(function).getFunction(*this);
}

jsi::Value NodeApiJsiRuntime::call(
    const jsi::Function &func,
    const jsi::Value &jsThis,
    const jsi::Value *args,
    size_t count) {
  NodeApiScope scope{*this};
  return toJsiValue(callFunction(
      getNodeApiValue(jsThis),
      getNodeApiValue(func),
      NodeApiValueArgs(*this, span<const jsi::Value>(args, count))));
}

jsi::Value NodeApiJsiRuntime::callAsConstructor(
    const jsi::Function &func,
    const jsi::Value *args,
    size_t count) {
  NodeApiScope scope{*this};
  return toJsiValue(constructObject(
      getNodeApiValue(func),
      NodeApiValueArgs(*this, span<jsi::Value const>(args, count))));
}

jsi::Runtime::ScopeState *NodeApiJsiRuntime::pushScope() {
  napi_handle_scope result{};
  CHECK_NAPI(jsrApi_->napi_open_handle_scope(env_, &result));
  pushPointerValueScope();
  return reinterpret_cast<jsi::Runtime::ScopeState *>(result);
}

void NodeApiJsiRuntime::popScope(jsi::Runtime::ScopeState *state) {
  popPointerValueScope();
  CHECK_NAPI(jsrApi_->napi_close_handle_scope(
      env_, reinterpret_cast<napi_handle_scope>(state)));
}

bool NodeApiJsiRuntime::strictEquals(const jsi::Symbol &a, const jsi::Symbol &b)
    const {
  NodeApiScope scope{*this};
  return strictEquals(getNodeApiValue(a), getNodeApiValue(b));
}

#if JSI_VERSION >= 6
bool NodeApiJsiRuntime::strictEquals(const jsi::BigInt &a, const jsi::BigInt &b)
    const {
  NodeApiScope scope{*this};
  return strictEquals(getNodeApiValue(a), getNodeApiValue(b));
}
#endif

bool NodeApiJsiRuntime::strictEquals(const jsi::String &a, const jsi::String &b)
    const {
  NodeApiScope scope{*this};
  return strictEquals(getNodeApiValue(a), getNodeApiValue(b));
}

bool NodeApiJsiRuntime::strictEquals(const jsi::Object &a, const jsi::Object &b)
    const {
  NodeApiScope scope{*this};
  return strictEquals(getNodeApiValue(a), getNodeApiValue(b));
}

bool NodeApiJsiRuntime::instanceOf(
    const jsi::Object &obj,
    const jsi::Function &func) {
  NodeApiScope scope{*this};
  return instanceOf(getNodeApiValue(obj), getNodeApiValue(func));
}

#if JSI_VERSION >= 11
void NodeApiJsiRuntime::setExternalMemoryPressure(
    const jsi::Object & /*obj*/,
    size_t /*amount*/) {
  // TODO: implement
}
#endif

//=====================================================================================================================
// NodeApiJsiRuntime::NodeApiScope implementation
//=====================================================================================================================

NodeApiJsiRuntime::NodeApiScope::NodeApiScope(
    const NodeApiJsiRuntime &runtime) noexcept
    : NodeApiScope(const_cast<NodeApiJsiRuntime &>(runtime)) {}

NodeApiJsiRuntime::NodeApiScope::NodeApiScope(
    NodeApiJsiRuntime &runtime) noexcept
    : runtime_(runtime),
      envScope_(runtime_.getEnv()),
      scopeState_(runtime_.pushScope()) {
  runtime_.pushPointerValueScope();
}

NodeApiJsiRuntime::NodeApiScope::~NodeApiScope() noexcept {
  runtime_.popPointerValueScope();
  runtime_.popScope(scopeState_);
}

//=====================================================================================================================
// NodeApiJsiRuntime::NodeApiPointerValueScope implementation
//=====================================================================================================================

NodeApiJsiRuntime::NodeApiPointerValueScope::NodeApiPointerValueScope(
    NodeApiJsiRuntime &runtime) noexcept
    : runtime_(runtime) {
  runtime_.pushPointerValueScope();
}

NodeApiJsiRuntime::NodeApiPointerValueScope::
    ~NodeApiPointerValueScope() noexcept {
  runtime_.popPointerValueScope();
}

//=====================================================================================================================
// NodeApiJsiRuntime::AutoRestore implementation
//=====================================================================================================================

template <typename T>
NodeApiJsiRuntime::AutoRestore<T>::AutoRestore(T &varRef, T newValue)
    : varRef_{varRef}, oldValue_{std::exchange(varRef, newValue)} {}

template <typename T>
NodeApiJsiRuntime::AutoRestore<T>::~AutoRestore() {
  varRef_ = oldValue_;
}

//=====================================================================================================================
// NodeApiJsiRuntime::NodeApiStackOnlyPointerValue implementation
//=====================================================================================================================

NodeApiJsiRuntime::NodeApiStackOnlyPointerValue::NodeApiStackOnlyPointerValue(
    napi_value value,
    NodeApiPointerValueKind pointerKind) noexcept
    : value_(value), pointerKind_(pointerKind) {}

// Intentionally do nothing since the value is allocated on the stack.
void NodeApiJsiRuntime::NodeApiStackOnlyPointerValue::invalidate() noexcept {}

NodeApiJsiRuntime::NodeApiRefCountedPointerValue *
NodeApiJsiRuntime::NodeApiStackOnlyPointerValue::clone(
    NodeApiJsiRuntime &runtime) const noexcept {
  return NodeApiRefCountedPointerValue::make(runtime, value_, pointerKind_)
      .release();
}

napi_value NodeApiJsiRuntime::NodeApiStackOnlyPointerValue::getValue(
    NodeApiJsiRuntime & /*runtime*/) noexcept {
  return value_;
}

NodeApiJsiRuntime::NodeApiPointerValueKind
NodeApiJsiRuntime::NodeApiStackOnlyPointerValue::getKind() const noexcept {
  return pointerKind_;
}

//=====================================================================================================================
// NodeApiJsiRuntime::NodeApiPointerValue implementation
//=====================================================================================================================

NodeApiJsiRuntime::NodeApiRefCountedPointerValue::NodeApiRefCountedPointerValue(
    NodeApiJsiRuntime &runtime,
    napi_value value,
    NodeApiJsiRuntime::NodeApiPointerValueKind pointerKind,
    int32_t initialRefCount) noexcept
    : pendingDeletions_(runtime.pendingDeletions_),
      value_(value),
      refCount_(initialRefCount),
      pointerKind_(pointerKind) {}

/*static*/ NodeApiJsiRuntime::NodeApiRefHolder
NodeApiJsiRuntime::NodeApiRefCountedPointerValue::make(
    NodeApiJsiRuntime &runtime,
    napi_value value,
    NodeApiJsiRuntime::NodeApiPointerValueKind pointerKind,
    int32_t initialRefCount) {
  NodeApiRefHolder result{
      new NodeApiRefCountedPointerValue(
          runtime, value, pointerKind, initialRefCount),
      attachTag};
  runtime.addStackValue(NodeApiStackValuePtr{result.get()});
  return result;
}

/*static*/ NodeApiJsiRuntime::NodeApiRefHolder
NodeApiJsiRuntime::NodeApiRefCountedPointerValue::makeNodeApiRef(
    NodeApiJsiRuntime &runtime,
    napi_value value,
    NodeApiPointerValueKind pointerKind,
    int32_t initialRefCount) {
  NodeApiRefHolder result{make(runtime, value, pointerKind, initialRefCount)};
  result->createNodeApiRef(runtime);
  return result;
}

void NodeApiJsiRuntime::NodeApiRefCountedPointerValue::invalidate() noexcept {
  decRefCount();
}

NodeApiJsiRuntime::NodeApiRefCountedPointerValue *
NodeApiJsiRuntime::NodeApiRefCountedPointerValue::clone(
    NodeApiJsiRuntime & /*runtime*/) const noexcept {
  incRefCount();
  return const_cast<NodeApiRefCountedPointerValue *>(this);
}

napi_value NodeApiJsiRuntime::NodeApiRefCountedPointerValue::getValue(
    NodeApiJsiRuntime &runtime) noexcept {
  if (value_ != nullptr) {
    return value_;
  }

  if (ref_ == nullptr) {
    return nullptr;
  }

  JSRuntimeApi *jsrApi = JSRuntimeApi::current();
  if (pointerKind_ == NodeApiPointerValueKind::Object ||
      pointerKind_ == NodeApiPointerValueKind::WeakObject) {
    CHECK_NAPI_ELSE_CRASH(
        jsrApi->napi_get_reference_value(runtime.getEnv(), ref_, &value_));
  } else {
    napi_value obj{};
    CHECK_NAPI_ELSE_CRASH(
        jsrApi->napi_get_reference_value(runtime.getEnv(), ref_, &obj));
    // TODO: Should we use an interned property key?
    CHECK_NAPI_ELSE_CRASH(jsrApi->napi_get_named_property(
        runtime.getEnv(), obj, kPrimitivePropertyName, &value_));
  }

  if (value_ != nullptr) {
    runtime.addStackValue(NodeApiStackValuePtr(this));
  }

  return value_;
}

NodeApiJsiRuntime::NodeApiPointerValueKind
NodeApiJsiRuntime::NodeApiRefCountedPointerValue::getKind() const noexcept {
  return pointerKind_;
}

bool NodeApiJsiRuntime::NodeApiRefCountedPointerValue::usedByJsiPointer()
    const noexcept {
  return !NodeApiRefCount::isZero(refCount_);
}

void NodeApiJsiRuntime::NodeApiRefCountedPointerValue::deleteStackValue(
    NodeApiJsiRuntime &runtime) noexcept {
  CHECK_ELSE_CRASH(value_, "value_ must not be null");
  if (canBeDeletedFromStack_) {
    delete this;
    return;
  }

  if (usedByJsiPointer() && ref_ == nullptr) {
    createNodeApiRef(runtime);
  }

  value_ = nullptr;
}

void NodeApiJsiRuntime::NodeApiRefCountedPointerValue::deleteNodeApiRef(
    NodeApiJsiRuntime &runtime) noexcept {
  if (ref_ != nullptr) {
    CHECK_NAPI_ELSE_CRASH(
        JSRuntimeApi::current()->napi_delete_reference(runtime.getEnv(), ref_));
    ref_ = nullptr;
  }

  if (value_ != nullptr) {
    canBeDeletedFromStack_ = true;
  } else {
    delete this;
  }
}

void NodeApiJsiRuntime::NodeApiStackValueDeleter::operator()(
    NodeApiRefCountedPointerValue *ptr) const noexcept {
  if (ptr == nullptr) {
    return;
  }

  ptr->value_ = nullptr;
  if (ptr->canBeDeletedFromStack_) {
    delete ptr;
  }
}

void NodeApiJsiRuntime::NodeApiRefDeleter::operator()(
    NodeApiRefCountedPointerValue *ptr) const noexcept {
  if (ptr == nullptr) {
    return;
  }

  if (ptr->value_ != nullptr) {
    ptr->canBeDeletedFromStack_ = true;
  } else {
    delete ptr;
  }
}

void NodeApiJsiRuntime::NodeApiRefCountedPointerValue::incRefCount()
    const noexcept {
  NodeApiRefCount::incRefCount(refCount_);
}

void NodeApiJsiRuntime::NodeApiRefCountedPointerValue::decRefCount()
    const noexcept {
  if (NodeApiRefCount::decRefCount(refCount_)) {
    pendingDeletions_->addPointerValueToDelete(
        NodeApiRefPtr(const_cast<NodeApiRefCountedPointerValue *>(this)));
  }
}

NodeApiJsiRuntime::NodeApiRefCountedPointerValue *
NodeApiJsiRuntime::NodeApiRefCountedPointerValue::createNodeApiRef(
    NodeApiJsiRuntime &runtime) {
  JSRuntimeApi *jsrApi = JSRuntimeApi::current();
  CHECK_ELSE_CRASH(value_ != nullptr, "value_ must not be null");
  CHECK_ELSE_CRASH(ref_ == nullptr, "ref_ must be null");
  if (pointerKind_ == NodeApiPointerValueKind::Object) {
    CHECK_NAPI_ELSE_CRASH(
        jsrApi->napi_create_reference(runtime.getEnv(), value_, 1, &ref_));
  } else if (pointerKind_ != NodeApiPointerValueKind::WeakObject) {
    napi_value obj{};
    CHECK_NAPI_ELSE_CRASH(jsrApi->napi_create_object(runtime.getEnv(), &obj));
    CHECK_NAPI_ELSE_CRASH(jsrApi->napi_set_named_property(
        runtime.getEnv(), obj, kPrimitivePropertyName, value_));
    CHECK_NAPI_ELSE_CRASH(
        jsrApi->napi_create_reference(runtime.getEnv(), obj, 1, &ref_));
  } else {
    CHECK_NAPI_ELSE_CRASH(
        jsrApi->napi_create_reference(runtime.getEnv(), value_, 0, &ref_));
  }
  return this;
}

//=====================================================================================================================
// NodeApiJsiRuntime::SmallBuffer implementation
//=====================================================================================================================

template <typename T, size_t InplaceSize>
NodeApiJsiRuntime::SmallBuffer<T, InplaceSize>::SmallBuffer(
    size_t size) noexcept
    : size_{size},
      heapData_{size_ > InplaceSize ? std::make_unique<T[]>(size_) : nullptr} {}

template <typename T, size_t InplaceSize>
T *NodeApiJsiRuntime::SmallBuffer<T, InplaceSize>::data() noexcept {
  return heapData_ ? heapData_.get() : stackData_.data();
}

template <typename T, size_t InplaceSize>
size_t NodeApiJsiRuntime::SmallBuffer<T, InplaceSize>::size() const noexcept {
  return size_;
}

//=====================================================================================================================
// NodeApiJsiRuntime::NodeApiValueArgs implementation
//=====================================================================================================================

NodeApiJsiRuntime::NodeApiValueArgs::NodeApiValueArgs(
    NodeApiJsiRuntime &runtime,
    span<const jsi::Value> args)
    : args_{args.size()} {
  napi_value *jsArgs = args_.data();
  for (size_t i = 0; i < args.size(); ++i) {
    jsArgs[i] = runtime.getNodeApiValue(args[i]);
  }
}

NodeApiJsiRuntime::NodeApiValueArgs::operator span<napi_value>() {
  return span<napi_value>{args_.data(), args_.size()};
}

//=====================================================================================================================
// NodeApiJsiRuntime::JsiValueView implementation
//=====================================================================================================================

NodeApiJsiRuntime::JsiValueView::JsiValueView(
    NodeApiJsiRuntime *runtime,
    napi_value jsValue)
    : value_{initValue(runtime, jsValue, std::addressof(pointerStore_))} {}

NodeApiJsiRuntime::JsiValueView::operator const jsi::Value &() const noexcept {
  return value_;
}

/*static*/ jsi::Value NodeApiJsiRuntime::JsiValueView::initValue(
    NodeApiJsiRuntime *runtime,
    napi_value value,
    StoreType *store) {
  switch (runtime->typeOf(value)) {
    case napi_valuetype::napi_undefined:
      return jsi::Value::undefined();
    case napi_valuetype::napi_null:
      return jsi::Value::null();
    case napi_valuetype::napi_boolean:
      return jsi::Value{runtime->getValueBool(value)};
    case napi_valuetype::napi_number:
      return jsi::Value{runtime->getValueDouble(value)};
    case napi_valuetype::napi_string:
      return make<jsi::String>(new (store) NodeApiStackOnlyPointerValue(
          value, NodeApiPointerValueKind::String));
    case napi_valuetype::napi_symbol:
      return make<jsi::Symbol>(new (store) NodeApiStackOnlyPointerValue(
          value, NodeApiPointerValueKind::Symbol));
    case napi_valuetype::napi_object:
    case napi_valuetype::napi_function:
    case napi_valuetype::napi_external:
      return make<jsi::Object>(new (store) NodeApiStackOnlyPointerValue(
          value, NodeApiPointerValueKind::Object));
#if JSI_VERSION >= 8
    case napi_valuetype::napi_bigint:
      return make<jsi::BigInt>(new (store) NodeApiStackOnlyPointerValue(
          value, NodeApiPointerValueKind::BigInt));
#endif
    default:
      throw jsi::JSINativeException("Unexpected value type");
  }
}

//=====================================================================================================================
// NodeApiJsiRuntime::JsiValueViewArgs implementation
//=====================================================================================================================

NodeApiJsiRuntime::JsiValueViewArgs::JsiValueViewArgs(
    NodeApiJsiRuntime *runtime,
    span<napi_value> args) noexcept
    : pointerStore_{args.size()}, args_{args.size()} {
  JsiValueView::StoreType *pointerStore = pointerStore_.data();
  jsi::Value *jsiArgs = args_.data();
  for (size_t i = 0; i < args_.size(); ++i) {
    jsiArgs[i] = JsiValueView::initValue(
        runtime, args[i], std::addressof(pointerStore[i]));
  }
}

jsi::Value const *NodeApiJsiRuntime::JsiValueViewArgs::data() noexcept {
  return args_.data();
}

size_t NodeApiJsiRuntime::JsiValueViewArgs::size() const noexcept {
  return args_.size();
}

//=====================================================================================================================
// NodeApiJsiRuntime::PropNameIDView implementation
//=====================================================================================================================

// TODO: account for symbol
NodeApiJsiRuntime::PropNameIDView::PropNameIDView(
    NodeApiJsiRuntime * /*runtime*/,
    napi_value propertyId) noexcept
    : propertyId_{make<jsi::PropNameID>(
          new (std::addressof(pointerStore_)) NodeApiStackOnlyPointerValue(
              propertyId,
              NodeApiPointerValueKind::StringPropNameID))} {}

NodeApiJsiRuntime::PropNameIDView::operator jsi::PropNameID const &()
    const noexcept {
  return propertyId_;
}

//=====================================================================================================================
// NodeApiJsiRuntime::HostFunctionWrapper implementation
//=====================================================================================================================

NodeApiJsiRuntime::HostFunctionWrapper::HostFunctionWrapper(
    jsi::HostFunctionType &&type,
    NodeApiJsiRuntime &runtime)
    : hostFunction_{std::move(type)}, runtime_{runtime} {}

jsi::HostFunctionType &
NodeApiJsiRuntime::HostFunctionWrapper::hostFunction() noexcept {
  return hostFunction_;
}

NodeApiJsiRuntime &NodeApiJsiRuntime::HostFunctionWrapper::runtime() noexcept {
  return runtime_;
}

//=====================================================================================================================
// NodeApiJsiRuntime implementation
//=====================================================================================================================

template <typename... Args>
jsi::JSError NodeApiJsiRuntime::makeJSError(Args &&...args) {
  std::ostringstream errorStream;
  ((errorStream << std::forward<Args>(args)), ...);
  return jsi::JSError(*this, errorStream.str());
}

[[noreturn]] void NodeApiJsiRuntime::throwJSException(
    napi_status status) const {
  auto formatStatusError = [](napi_status status) -> std::string {
    // TODO: (vmoroz) use a more sophisticated error formatting.
    std::ostringstream errorStream;
    errorStream << "A call to Node-API returned error code 0x" << std::hex
                << static_cast<int>(status) << '.';
    return errorStream.str();
  };

  NodeApiScope scope{*this};
  // Retrieve the exception value and clear as we will rethrow it as a C++
  // exception.
  napi_value jsError{};
  CHECK_NAPI_ELSE_CRASH(
      jsrApi_->napi_get_and_clear_last_exception(env_, &jsError));
  napi_valuetype jsErrorType;
  CHECK_NAPI_ELSE_CRASH(jsrApi_->napi_typeof(env_, jsError, &jsErrorType));
  if (jsErrorType == napi_undefined) {
    throw jsi::JSINativeException(formatStatusError(status).c_str());
  }
  jsi::Value jsiJSError = toJsiValue(jsError);

  std::string msg = "No message";
  std::string stack = "No stack";
  if (jsErrorType == napi_string) {
    // If the exception is a string, use it as the message.
    msg = stringToStdString(jsError);
  } else if (jsErrorType == napi_object) {
    // If the exception is an object try to retrieve its message and stack
    // properties.

    /// Attempt to retrieve a string property \p sym from \c jsError and store
    /// it in \p out. Ignore any catchable errors and non-string properties.
    auto getStrProp = [this, jsError](const char *sym, std::string &out) {
      napi_value value{};
      napi_status propStatus =
          jsrApi_->napi_get_named_property(env_, jsError, sym, &value);
      if (propStatus != napi_ok) {
        // An exception was thrown while retrieving the property, if it is
        // catchable, suppress it. Otherwise, rethrow this exception without
        // trying to invoke any more JavaScript.
        napi_value newJSError{};
        CHECK_NAPI_ELSE_CRASH(
            jsrApi_->napi_get_and_clear_last_exception(env_, &newJSError));
        napi_valuetype newJSErrorType;
        CHECK_NAPI_ELSE_CRASH(
            jsrApi_->napi_typeof(env_, newJSError, &newJSErrorType));

        if (propStatus != napi_cannot_run_js)
          return;

        // An uncatchable error occurred, it is unsafe to do anything that
        // might execute more JavaScript.
        if (newJSErrorType != napi_undefined) {
          throw jsi::JSError(
              toJsiValue(newJSError),
              "Uncatchable exception thrown while creating error",
              "No stack");
        } else {
          std::ostringstream errorStream;
          errorStream << "A call to Node-API returned error code 0x" << std::hex
                      << propStatus << '.';
          throw jsi::JSINativeException(errorStream.str().c_str());
        }
      }

      // If the property is a string, update out. Otherwise ignore it.
      napi_valuetype valueType;
      CHECK_NAPI_ELSE_CRASH(jsrApi_->napi_typeof(env_, value, &valueType));
      if (valueType == napi_string) {
        out = stringToStdString(value);
      }
    };

    getStrProp("message", msg);
    getStrProp("stack", stack);
  }

  // Use the constructor of jsi::JSError that cannot run additional
  // JS, since that may then result in additional exceptions and infinite
  // recursion.
  throw jsi::JSError(std::move(jsiJSError), msg, stack);
}

// Throws jsi::JSINativeException with a message.
[[noreturn]] void NodeApiJsiRuntime::throwNativeException(
    char const *errorMessage) const {
  throw jsi::JSINativeException(errorMessage);
}

// Rewrites error messages to match the JSI unit test expectations.
void NodeApiJsiRuntime::rewriteErrorMessage(napi_value jsError) const {
  // The code below must work correctly even if the 'message' getter throws.
  // In case when it throws, we ignore that exception.
  napi_value message{};
  napi_status status = jsrApi_->napi_get_property(
      env_, jsError, getNodeApiValue(propertyId_.message), &message);
  if (status != napi_ok) {
    // If the 'message' property getter throws, then we clear the exception and
    // ignore it.
    napi_value ignoreJSError{};
    jsrApi_->napi_get_and_clear_last_exception(env_, &ignoreJSError);
  } else if (typeOf(message) == napi_string) {
    // JSI unit tests expect V8- or JSC-like messages for the stack overflow.
    std::string messageStr = stringToStdString(message);
    if (messageStr == "Out of stack space" ||
        messageStr.find("Maximum call stack") != std::string::npos) {
      setProperty(
          jsError,
          getNodeApiValue(propertyId_.message),
          createStringUtf8("RangeError : Maximum call stack size exceeded"sv));
    }
  }

  // Make sure that the call stack has the current URL
  if (!sourceURL_.empty()) {
    napi_value stack{};
    status = jsrApi_->napi_get_property(
        env_, jsError, getNodeApiValue(propertyId_.stack), &stack);
    if (status != napi_ok) {
      // If the 'stack' property getter throws, then we clear the exception and
      // ignore it.
      napi_value ignoreJSError{};
      jsrApi_->napi_get_and_clear_last_exception(env_, &ignoreJSError);
    } else if (typeOf(message) == napi_string) {
      // JSI unit tests expect URL to be part of the call stack.
      std::string stackStr = stringToStdString(stack);
      if (stackStr.find(sourceURL_) == std::string::npos) {
        stackStr += sourceURL_ + '\n' + stackStr;
        setProperty(
            jsError,
            getNodeApiValue(propertyId_.stack),
            createStringUtf8(stackStr.c_str()));
      }
    }
  }
}

// Evaluates lambda and augments exception messages with the method's name.
template <typename TLambda>
auto NodeApiJsiRuntime::runInMethodContext(
    char const *methodName,
    TLambda lambda) {
  try {
    return lambda();
  } catch (jsi::JSError const &) {
    throw; // do not augment the JSError exceptions.
  } catch (std::exception const &ex) {
    throwNativeException(
        (std::string{"Exception in "} + methodName + ": " + ex.what()).c_str());
  } catch (...) {
    throwNativeException(
        (std::string{"Exception in "} + methodName + ": <unknown>").c_str());
  }
}

// Evaluates lambda and converts all exceptions to Node-API errors.
template <typename TLambda>
napi_value NodeApiJsiRuntime::handleCallbackExceptions(
    TLambda lambda) const noexcept {
  try {
    try {
      return lambda();
    } catch (jsi::JSError const &jsError) {
      // This block may throw exceptions
      setException(getNodeApiValue(jsError.value()));
    }
  } catch (std::exception const &ex) {
    setException(ex.what());
  } catch (...) {
    setException("Unexpected error");
  }

  return getUndefined();
}

// Throws JavaScript exception using Node-API.
bool NodeApiJsiRuntime::setException(napi_value error) const noexcept {
  // This method must not throw. We return false in case of error.
  return jsrApi_->napi_throw(env_, error) == napi_status::napi_ok;
}

// Throws JavaScript error exception with the provided message using Node-API.
bool NodeApiJsiRuntime::setException(std::string_view message) const noexcept {
  // This method must not throw. We return false in case of error.
  return jsrApi_->napi_throw_error(env_, "Unknown", message.data()) ==
      napi_status::napi_ok;
}

// Gets type of the napi_value.
napi_valuetype NodeApiJsiRuntime::typeOf(napi_value value) const {
  napi_valuetype result{};
  CHECK_NAPI(jsrApi_->napi_typeof(env_, value, &result));
  return result;
}

// Returns true if two napi_values are strict equal per JavaScript rules.
bool NodeApiJsiRuntime::strictEquals(napi_value left, napi_value right) const {
  bool result{false};
  CHECK_NAPI(jsrApi_->napi_strict_equals(env_, left, right, &result));
  return result;
}

// Gets the napi_value for the JavaScript's undefined value.
napi_value NodeApiJsiRuntime::getUndefined() const {
  napi_value result{nullptr};
  CHECK_NAPI(jsrApi_->napi_get_undefined(env_, &result));
  return result;
}

// Gets the napi_value for the JavaScript's null value.
napi_value NodeApiJsiRuntime::getNull() const {
  napi_value result{};
  CHECK_NAPI(jsrApi_->napi_get_null(env_, &result));
  return result;
}

// Gets the napi_value for the JavaScript's global object.
napi_value NodeApiJsiRuntime::getGlobal() const {
  napi_value result{nullptr};
  CHECK_NAPI(jsrApi_->napi_get_global(env_, &result));
  return result;
}

// Gets the napi_value for the JavaScript's true and false values.
napi_value NodeApiJsiRuntime::getBoolean(bool value) const {
  napi_value result{nullptr};
  CHECK_NAPI(jsrApi_->napi_get_boolean(env_, value, &result));
  return result;
}

// Gets value of the Boolean napi_value.
bool NodeApiJsiRuntime::getValueBool(napi_value value) const {
  bool result{false};
  CHECK_NAPI(jsrApi_->napi_get_value_bool(env_, value, &result));
  return result;
}

// Creates napi_value with an int32_t value.
napi_value NodeApiJsiRuntime::createInt32(int32_t value) const {
  napi_value result{};
  CHECK_NAPI(jsrApi_->napi_create_int32(env_, value, &result));
  return result;
}

// Creates napi_value with an int32_t value.
napi_value NodeApiJsiRuntime::createUInt32(uint32_t value) const {
  napi_value result{};
  CHECK_NAPI(jsrApi_->napi_create_uint32(env_, value, &result));
  return result;
}

// Creates napi_value with a double value.
napi_value NodeApiJsiRuntime::createDouble(double value) const {
  napi_value result{};
  CHECK_NAPI(jsrApi_->napi_create_double(env_, value, &result));
  return result;
}

// Gets value of the Double napi_value.
double NodeApiJsiRuntime::getValueDouble(napi_value value) const {
  double result{0};
  CHECK_NAPI(jsrApi_->napi_get_value_double(env_, value, &result));
  return result;
}

// Creates a napi_value string from the extended ASCII symbols that correspond
// to the Latin1 encoding. Each character is a byte-sized value from 0 to 255.
napi_value NodeApiJsiRuntime::createStringLatin1(std::string_view value) const {
  CHECK_ELSE_THROW(value.data(), "Cannot convert a nullptr to a JS string.");
  napi_value result{};
  CHECK_NAPI(jsrApi_->napi_create_string_latin1(
      env_, value.data(), value.size(), &result));
  return result;
}

// Creates a napi_value string from a UTF-8 string.
napi_value NodeApiJsiRuntime::createStringUtf8(std::string_view value) const {
  CHECK_ELSE_THROW(value.data(), "Cannot convert a nullptr to a JS string.");
  napi_value result{};
  CHECK_NAPI(jsrApi_->napi_create_string_utf8(
      env_, value.data(), value.size(), &result));
  return result;
}

// Creates a napi_value string from a UTF-8 string. Use data and length instead
// of string_view.
napi_value NodeApiJsiRuntime::createStringUtf8(
    const uint8_t *data,
    size_t length) const {
  return createStringUtf8({reinterpret_cast<const char *>(data), length});
}

// Gets std::string from the napi_value string.
std::string NodeApiJsiRuntime::stringToStdString(napi_value stringValue) const {
  std::string result;
  CHECK_ELSE_THROW(
      typeOf(stringValue) == napi_valuetype::napi_string,
      "Cannot convert a non JS string Node-API Value to a std::string.");
  size_t strLength{};
  CHECK_NAPI(jsrApi_->napi_get_value_string_utf8(
      env_, stringValue, nullptr, 0, &strLength));
  result.assign(strLength, '\0');
  size_t copiedLength{};
  CHECK_NAPI(jsrApi_->napi_get_value_string_utf8(
      env_, stringValue, &result[0], result.length() + 1, &copiedLength));
  CHECK_ELSE_THROW(result.length() == copiedLength, "Unexpected string length");
  return result;
}

// Gets or creates a unique string value from an UTF-8 string_view.
napi_value NodeApiJsiRuntime::getPropertyIdFromName(
    std::string_view value) const {
  napi_value result{};
  CHECK_NAPI(jsrApi_->napi_create_string_utf8(
      env_, value.data(), value.size(), &result));
  return result;
}

// Gets or creates a unique string value from an UTF-8 data/length range.
napi_value NodeApiJsiRuntime::getPropertyIdFromName(
    const uint8_t *data,
    size_t length) const {
  return getPropertyIdFromName({reinterpret_cast<const char *>(data), length});
}

// Gets or creates a unique string value from napi_value string.
napi_value NodeApiJsiRuntime::getPropertyIdFromName(napi_value str) const {
  return str;
}

// Gets or creates a unique string value from napi_value symbol.
napi_value NodeApiJsiRuntime::getPropertyIdFromSymbol(napi_value sym) const {
  return sym;
}

// Converts property id value to std::string.
std::string NodeApiJsiRuntime::propertyIdToStdString(napi_value propertyId) {
  if (typeOf(propertyId) == napi_symbol) {
    return symbolToStdString(propertyId);
  }

  return stringToStdString(propertyId);
}

// Creates a JavaScript symbol napi_value.
napi_value NodeApiJsiRuntime::createSymbol(
    std::string_view symbolDescription) const {
  napi_value result{};
  napi_value description = createStringUtf8(symbolDescription);
  CHECK_NAPI(jsrApi_->napi_create_symbol(env_, description, &result));
  return result;
}

// Calls Symbol.toString() and returns it as std::string.
std::string NodeApiJsiRuntime::symbolToStdString(napi_value symbolValue) {
  if (!cachedValue_.SymbolToString) {
    napi_value symbolCtor = getProperty(
        getNodeApiValue(cachedValue_.Global),
        getNodeApiValue(propertyId_.Symbol));
    napi_value symbolPrototype =
        getProperty(symbolCtor, getNodeApiValue(propertyId_.prototype));
    cachedValue_.SymbolToString = makeNodeApiRef(
        getProperty(symbolPrototype, getNodeApiValue(propertyId_.toString)),
        NodeApiPointerValueKind::Object);
  }
  napi_value jsString = callFunction(
      symbolValue, getNodeApiValue(cachedValue_.SymbolToString), {});
  return stringToStdString(jsString);
}

// Calls a JavaScript function.
napi_value NodeApiJsiRuntime::callFunction(
    napi_value thisArg,
    napi_value function,
    span<napi_value> args) const {
  napi_value result{};
  CHECK_NAPI(jsrApi_->napi_call_function(
      env_, thisArg, function, args.size(), args.data(), &result));
  return result;
}

// Constructs a new JavaScript Object using a constructor function.
napi_value NodeApiJsiRuntime::constructObject(
    napi_value constructor,
    span<napi_value> args) const {
  napi_value result{};
  CHECK_NAPI(jsrApi_->napi_new_instance(
      env_, constructor, args.size(), args.data(), &result));
  return result;
}

// Returns true if object was constructed using the provided constructor.
bool NodeApiJsiRuntime::instanceOf(napi_value object, napi_value constructor)
    const {
  bool result{false};
  CHECK_NAPI(jsrApi_->napi_instanceof(env_, object, constructor, &result));
  return result;
}

// Creates new JavaScript Object.
napi_value NodeApiJsiRuntime::createNodeApiObject() const {
  napi_value result{};
  CHECK_NAPI(jsrApi_->napi_create_object(env_, &result));
  return result;
}

// Returns true if the object has a property with the provided property ID.
bool NodeApiJsiRuntime::hasProperty(napi_value object, napi_value propertyId)
    const {
  bool result{};
  CHECK_NAPI(jsrApi_->napi_has_property(env_, object, propertyId, &result));
  return result;
}

// Gets object property value.
napi_value NodeApiJsiRuntime::getProperty(
    napi_value object,
    napi_value propertyId) const {
  napi_value result{};
  CHECK_NAPI(jsrApi_->napi_get_property(env_, object, propertyId, &result));
  return result;
}

// Sets object property value.
void NodeApiJsiRuntime::setProperty(
    napi_value object,
    napi_value propertyId,
    napi_value value) const {
  CHECK_NAPI(jsrApi_->napi_set_property(env_, object, propertyId, value));
}

// Deletes object property value.
bool NodeApiJsiRuntime::deleteProperty(napi_value object, napi_value propertyId)
    const {
  bool result{};
  CHECK_NAPI(jsrApi_->napi_delete_property(env_, object, propertyId, &result));
  return result;
}

// Sets object property value with the provided property accessibility
// attributes.
void NodeApiJsiRuntime::setProperty(
    napi_value object,
    napi_value propertyId,
    napi_value value,
    napi_property_attributes attrs) const {
  napi_property_descriptor descriptor{};
  descriptor.name = propertyId;
  descriptor.value = value;
  descriptor.attributes = attrs;
  CHECK_NAPI(jsrApi_->napi_define_properties(env_, object, 1, &descriptor));
}

// Creates a new JavaScript Array with the provided length.
napi_value NodeApiJsiRuntime::createNodeApiArray(size_t length) const {
  napi_value result{};
  CHECK_NAPI(jsrApi_->napi_create_array_with_length(env_, length, &result));
  return result;
}

bool NodeApiJsiRuntime::isArray(napi_value value) const {
  bool result{};
  CHECK_NAPI(jsrApi_->napi_is_array(env_, value, &result));
  return result;
}

size_t NodeApiJsiRuntime::getArrayLength(napi_value value) const {
  uint32_t result{};
  CHECK_NAPI(jsrApi_->napi_get_array_length(env_, value, &result));
  return result;
}

napi_value NodeApiJsiRuntime::getElement(napi_value arr, size_t index) const {
  napi_value result{};
  CHECK_NAPI(jsrApi_->napi_get_element(
      env_, arr, static_cast<int32_t>(index), &result));
  return result;
}

// Sets array element.
void NodeApiJsiRuntime::setElement(
    napi_value array,
    uint32_t index,
    napi_value value) const {
  CHECK_NAPI(jsrApi_->napi_set_element(env_, array, index, value));
}

// The Node-API external function callback used for the JSI host function
// implementation.
/*static*/ napi_value __cdecl NodeApiJsiRuntime::jsiHostFunctionCallback(
    napi_env env,
    napi_callback_info info) noexcept {
  HostFunctionWrapper *hostFuncWrapper{};
  size_t argc{};
  CHECK_NAPI_ELSE_CRASH(
      JSRuntimeApi::current()->napi_get_cb_info(
          env,
          info,
          &argc,
          nullptr,
          nullptr,
          reinterpret_cast<void **>(&hostFuncWrapper)));
  CHECK_ELSE_CRASH(hostFuncWrapper, "Cannot find the host function");
  NodeApiJsiRuntime &runtime = hostFuncWrapper->runtime();
  NodeApiPointerValueScope scope{runtime};

  return runtime.handleCallbackExceptions(
      [&env, &info, &argc, &runtime, &hostFuncWrapper]() {
        SmallBuffer<napi_value, MaxStackArgCount> napiArgs(argc);
        napi_value thisArg{};
        CHECK_NAPI_ELSE_CRASH(
            JSRuntimeApi::current()->napi_get_cb_info(
                env, info, &argc, napiArgs.data(), &thisArg, nullptr));
        CHECK_ELSE_CRASH(napiArgs.size() == argc, "Wrong argument count");
        const JsiValueView jsiThisArg{&runtime, thisArg};
        JsiValueViewArgs jsiArgs(
            &runtime, span<napi_value>(napiArgs.data(), napiArgs.size()));

        const jsi::HostFunctionType &hostFunc = hostFuncWrapper->hostFunction();
        return runtime.runInMethodContext(
            "HostFunction", [&hostFunc, &runtime, &jsiThisArg, &jsiArgs]() {
              return runtime.getNodeApiValue(hostFunc(
                  runtime, jsiThisArg, jsiArgs.data(), jsiArgs.size()));
            });
      });
}

// Creates an external function.
napi_value NodeApiJsiRuntime::createExternalFunction(
    napi_value name,
    int32_t paramCount,
    napi_callback callback,
    void *callbackData) {
  std::string funcName = stringToStdString(name);
  napi_value function{};
  CHECK_NAPI(jsrApi_->napi_create_function(
      env_,
      funcName.data(),
      funcName.length(),
      callback,
      callbackData,
      &function));
  setProperty(
      function,
      getNodeApiValue(propertyId_.length),
      createInt32(paramCount),
      napi_property_attributes::napi_default);

  return function;
}

// Creates an object that wraps up external data.
napi_value NodeApiJsiRuntime::createExternalObject(
    void *data,
    node_api_nogc_finalize finalizeCallback) const {
  napi_value result{};
  CHECK_NAPI(jsrApi_->napi_create_external(
      env_, data, finalizeCallback, nullptr, &result));
  return result;
}

// Wraps up std::unique_ptr as an external object.
template <typename T>
napi_value NodeApiJsiRuntime::createExternalObject(
    std::unique_ptr<T> &&data) const {
  node_api_nogc_finalize finalize = [](node_api_nogc_env /*env*/,
                                       void *dataToDestroy,
                                       void * /*finalizerHint*/) {
    // We wrap dataToDestroy in a unique_ptr to avoid calling delete explicitly.
    std::unique_ptr<T> dataDeleter{static_cast<T *>(dataToDestroy)};
  };
  napi_value object = createExternalObject(data.get(), finalize);

  // We only call data.release() after the createExternalObject succeeds.
  // Otherwise, when createExternalObject fails and an exception is thrown,
  // the memory that data used to own will be leaked.
  data.release();
  return object;
}

// Gets external data wrapped by an external object.
void *NodeApiJsiRuntime::getExternalData(napi_value object) const {
  void *result{};
  CHECK_NAPI(jsrApi_->napi_get_value_external(env_, object, &result));
  return result;
}

// Gets JSI host object wrapped into a napi_value object.
const std::shared_ptr<jsi::HostObject> &NodeApiJsiRuntime::getJsiHostObject(
    napi_value obj) {
  const napi_value hostObjectHolder =
      getProperty(obj, getNodeApiValue(propertyId_.hostObjectSymbol));

  if (typeOf(hostObjectHolder) == napi_valuetype::napi_external) {
    if (void *data = getExternalData(hostObjectHolder)) {
      return *static_cast<std::shared_ptr<jsi::HostObject> *>(data);
    }
  }

  throw jsi::JSINativeException("Cannot get HostObjects.");
}

// Gets cached or creates Proxy handler to implement the JSI host object.
napi_value NodeApiJsiRuntime::getHostObjectProxyHandler() {
  if (!cachedValue_.HostObjectProxyHandler) {
    const napi_value handler = createNodeApiObject();
    setProxyTrap<&NodeApiJsiRuntime::hostObjectHasTrap, 2>(
        handler, getNodeApiValue(propertyId_.has));
    setProxyTrap<&NodeApiJsiRuntime::hostObjectGetTrap, 3>(
        handler, getNodeApiValue(propertyId_.get));
    setProxyTrap<&NodeApiJsiRuntime::hostObjectSetTrap, 4>(
        handler, getNodeApiValue(propertyId_.set));
    setProxyTrap<&NodeApiJsiRuntime::hostObjectOwnKeysTrap, 1>(
        handler, getNodeApiValue(propertyId_.ownKeys));
    setProxyTrap<&NodeApiJsiRuntime::hostObjectGetOwnPropertyDescriptorTrap, 2>(
        handler, getNodeApiValue(propertyId_.getOwnPropertyDescriptor));
    cachedValue_.HostObjectProxyHandler =
        makeNodeApiRef(handler, NodeApiPointerValueKind::Object);
  }

  return getNodeApiValue(cachedValue_.HostObjectProxyHandler);
}

// Sets Proxy trap method as a pointer to NodeApiJsiRuntime instance method.
template <
    napi_value (NodeApiJsiRuntime::*trapMethod)(span<napi_value>),
    size_t argCount>
void NodeApiJsiRuntime::setProxyTrap(
    napi_value handler,
    napi_value propertyName) {
  napi_callback proxyTrap = [](napi_env env, napi_callback_info info) noexcept {
    NodeApiJsiRuntime *runtime{};
    napi_value args[argCount]{};
    size_t actualArgCount{argCount};
    CHECK_NAPI_ELSE_CRASH(
        JSRuntimeApi::current()->napi_get_cb_info(
            env,
            info,
            &actualArgCount,
            args,
            nullptr,
            reinterpret_cast<void **>(&runtime)));
    CHECK_ELSE_CRASH(
        actualArgCount == argCount, "proxy trap requires argCount arguments.");
    NodeApiPointerValueScope scope{*runtime};
    return runtime->handleCallbackExceptions([&runtime, &args]() {
      return (runtime->*trapMethod)(span<napi_value>(args, argCount));
    });
  };

  setProperty(
      handler,
      propertyName,
      createExternalFunction(propertyName, argCount, proxyTrap, this));
}

// The host object Proxy 'has' trap implementation.
napi_value NodeApiJsiRuntime::hostObjectHasTrap(span<napi_value> args) {
  // args[0] - the Proxy target object.
  // args[1] - the name of the property to check.
  napi_value propertyName = args[1];
  const auto &hostObject = getJsiHostObject(args[0]);
  return runInMethodContext(
      "HostObject::has", [&hostObject, &propertyName, this]() {
        // std::vector<jsi::PropNameID> ownKeys =
        // hostObject->getPropertyNames(*this); for (jsi::PropNameID &ownKey :
        // ownKeys) {
        //   if (strictEquals(propertyName, getNodeApiValue(ownKey))) {
        return getBoolean(true);
        //   }
        // }
        // return getBoolean(false);
      });
}

// The host object Proxy 'get' trap implementation.
napi_value NodeApiJsiRuntime::hostObjectGetTrap(span<napi_value> args) {
  // args[0] - the Proxy target object.
  // args[1] - the name of the property to set.
  // args[2] - the Proxy object (unused).
  napi_value target = args[0];
  napi_value propertyName = args[1];
  bool isTargetOwnProp{};
  CHECK_NAPI(jsrApi_->napi_has_own_property(
      env_, target, propertyName, &isTargetOwnProp));
  if (isTargetOwnProp) {
    return getProperty(target, propertyName);
  }
  const auto &hostObject = getJsiHostObject(args[0]);
  PropNameIDView propertyId{this, propertyName};
  return runInMethodContext(
      "HostObject::get", [&hostObject, &propertyId, this]() {
        return getNodeApiValue(hostObject->get(*this, propertyId));
      });
}

// The host object Proxy 'set' trap implementation.
napi_value NodeApiJsiRuntime::hostObjectSetTrap(span<napi_value> args) {
  // args[0] - the Proxy target object.
  // args[1] - the name of the property to set.
  // args[2] - the new value of the property to set.
  // args[3] - the Proxy object (unused).
  const auto &hostObject = getJsiHostObject(args[0]);
  PropNameIDView propertyId{this, args[1]};
  JsiValueView value{this, args[2]};
  runInMethodContext(
      "HostObject::set", [&hostObject, &propertyId, &value, this]() {
        hostObject->set(*this, propertyId, value);
      });
  return getUndefined();
}

// The host object Proxy 'ownKeys' trap implementation.
napi_value NodeApiJsiRuntime::hostObjectOwnKeysTrap(span<napi_value> args) {
  // args[0] - the Proxy target object.
  napi_value target = args[0];

  napi_value targetOwnKeys{};
  CHECK_NAPI(jsrApi_->napi_get_all_property_names(
      env_,
      target,
      napi_key_own_only,
      napi_key_all_properties,
      napi_key_numbers_to_strings,
      &targetOwnKeys));
  CHECK_ELSE_THROW(isArray(targetOwnKeys), "Expected an array");
  size_t targetOwnKeysLength = getArrayLength(targetOwnKeys);

  const auto &hostObject = getJsiHostObject(target);
  std::vector<jsi::PropNameID> hostOwnKeys = runInMethodContext(
      "HostObject::getPropertyNames",
      [&hostObject, this]() { return hostObject->getPropertyNames(*this); });

  std::vector<jsi::PropNameID> ownKeys;
  std::unordered_set<const PointerValue *> uniqueOwnKeys;
  // Minus one hostObjectSymbol key.
  ownKeys.reserve(targetOwnKeysLength - 1 + hostOwnKeys.size());
  uniqueOwnKeys.reserve(targetOwnKeysLength - 1 + hostOwnKeys.size());

  // Read all target own keys.
  if (targetOwnKeysLength > 1) {
    auto addPropNameId =
        [this, &uniqueOwnKeys, &ownKeys](jsi::PropNameID &&propNameId) {
          const PointerValue *pv = getPointerValue(propNameId);
          auto inserted = uniqueOwnKeys.insert(pv);
          CHECK_ELSE_THROW(inserted.second, "Target has non-unique keys");
          ownKeys.push_back(std::move(propNameId));
        };
    for (size_t i = 0; i < targetOwnKeysLength; ++i) {
      napi_value key = getElement(targetOwnKeys, i);
      napi_valuetype keyType = typeOf(key);
      if (keyType == napi_string) {
        addPropNameId(
            createPropNameIDFromString(makeJsiPointer<jsi::String>(key)));
#if JSI_VERSION >= 8
      } else if (keyType == napi_symbol) {
        if (strictEquals(key, getNodeApiValue(propertyId_.hostObjectSymbol))) {
          continue;
        }
        addPropNameId(
            createPropNameIDFromSymbol(makeJsiPointer<jsi::Symbol>(key)));
#endif
      } else {
        throwNativeException("Unexpected key type");
      }
    }
  }

  // Read all unique host own keys.
  for (jsi::PropNameID &propNameId : hostOwnKeys) {
    const PointerValue *pv = getPointerValue(propNameId);
    auto inserted = uniqueOwnKeys.insert(pv);
    if (inserted.second) {
      ownKeys.push_back(std::move(propNameId));
    }
  }

  // Put indexed properties before named ones.
  struct Index {
    uint32_t index;
    napi_value value;
  };
  std::vector<Index> indexKeys;
  std::vector<napi_value> nonIndexKeys;
  nonIndexKeys.reserve(ownKeys.size());
  for (const jsi::PropNameID &key : ownKeys) {
    napi_value napiKey = getNodeApiValue(key);
    napi_valuetype valueType = typeOf(napiKey);
    if (valueType == napi_string) {
      std::string keyStr = stringToStdString(napiKey);
      std::optional<uint32_t> indexKey =
          toArrayIndex(keyStr.begin(), keyStr.end());
      if (indexKey.has_value()) {
        indexKeys.push_back(Index{indexKey.value(), napiKey});
        continue;
      }
    }
    nonIndexKeys.push_back(napiKey);
  }

  std::sort(
      indexKeys.begin(),
      indexKeys.end(),
      [](const Index &left, const Index &right) {
        return left.index < right.index;
      });

  napi_value ownKeyArray = createNodeApiArray(0);
  uint32_t index = 0;
  for (const Index &indexKey : indexKeys) {
    setElement(ownKeyArray, index++, indexKey.value);
  }
  for (napi_value napiKey : nonIndexKeys) {
    setElement(ownKeyArray, index++, napiKey);
  }

  return ownKeyArray;
}

// The host object Proxy 'getOwnPropertyDescriptor' trap implementation.
napi_value NodeApiJsiRuntime::hostObjectGetOwnPropertyDescriptorTrap(
    span<napi_value> args) {
  // args[0] - the Proxy target object.
  // args[1] - the property
  const auto &hostObject = getJsiHostObject(args[0]);
  PropNameIDView propertyId{this, args[1]};

  return runInMethodContext(
      "HostObject::getOwnPropertyDescriptor",
      [&hostObject, &propertyId, this]() {
        auto getPropDescriptor = [](napi_value name, napi_value value) {
          return napi_property_descriptor{
              nullptr,
              name,
              nullptr,
              nullptr,
              nullptr,
              value,
              napi_default_jsproperty,
              nullptr};
        };
        napi_value trueValue = getBoolean(true);
        napi_property_descriptor properties[]{
            getPropDescriptor(
                getNodeApiValue(propertyId_.value),
                getNodeApiValue(hostObject->get(*this, propertyId))),
            getPropDescriptor(getNodeApiValue(propertyId_.writable), trueValue),
            getPropDescriptor(
                getNodeApiValue(propertyId_.enumerable), trueValue),
            getPropDescriptor(
                getNodeApiValue(propertyId_.configurable), trueValue)};
        napi_value descriptor = createNodeApiObject();
        CHECK_NAPI(jsrApi_->napi_define_properties(
            env_, descriptor, std::size(properties), properties));
        return descriptor;
      });
}

// Converts jsi::Bufer to span.
span<const uint8_t> NodeApiJsiRuntime::toSpan(const jsi::Buffer &buffer) {
  return span<const uint8_t>(buffer.data(), buffer.size());
}

// Creates jsi::Value from napi_value.
jsi::Value NodeApiJsiRuntime::toJsiValue(napi_value value) const {
  switch (typeOf(value)) {
    case napi_valuetype::napi_undefined:
      return jsi::Value::undefined();
    case napi_valuetype::napi_null:
      return jsi::Value::null();
    case napi_valuetype::napi_boolean:
      return jsi::Value{getValueBool(value)};
    case napi_valuetype::napi_number:
      return jsi::Value{getValueDouble(value)};
    case napi_valuetype::napi_string:
      return jsi::Value{makeJsiPointer<jsi::String>(value)};
    case napi_valuetype::napi_symbol:
      return jsi::Value{makeJsiPointer<jsi::Symbol>(value)};
    case napi_valuetype::napi_object:
    case napi_valuetype::napi_function:
    case napi_valuetype::napi_external:
      return jsi::Value{makeJsiPointer<jsi::Object>(value)};
#if JSI_VERSION >= 6
    case napi_valuetype::napi_bigint:
      return jsi::Value{makeJsiPointer<jsi::BigInt>(value)};
#endif
    default:
      throw jsi::JSINativeException("Unexpected value type");
  }
}

napi_value NodeApiJsiRuntime::getNodeApiValue(const jsi::Value &value) const {
  if (value.isUndefined()) {
    return getUndefined();
  } else if (value.isNull()) {
    return getNull();
  } else if (value.isBool()) {
    return getBoolean(value.getBool());
  } else if (value.isNumber()) {
    return createDouble(value.getNumber());
  } else if (value.isSymbol()) {
    return getNodeApiValue(
        value.getSymbol(*const_cast<NodeApiJsiRuntime *>(this)));
  } else if (value.isString()) {
    return getNodeApiValue(
        value.getString(*const_cast<NodeApiJsiRuntime *>(this)));
  } else if (value.isObject()) {
    return getNodeApiValue(
        value.getObject(*const_cast<NodeApiJsiRuntime *>(this)));
#if JSI_VERSION >= 8
  } else if (value.isBigInt()) {
    return getNodeApiValue(
        value.getBigInt(*const_cast<NodeApiJsiRuntime *>(this)));
#endif
  } else {
    throw jsi::JSINativeException("Unexpected jsi::Value type");
  }
}

napi_value NodeApiJsiRuntime::getNodeApiValue(const jsi::Pointer &ptr) const {
  return const_cast<NodeApiPointerValue *>(
             static_cast<const NodeApiPointerValue *>(getPointerValue(ptr)))
      ->getValue(*const_cast<NodeApiJsiRuntime *>(this));
}

napi_value NodeApiJsiRuntime::getNodeApiValue(
    const NodeApiRefHolder &ref) const {
  return ref->getValue(*const_cast<NodeApiJsiRuntime *>(this));
}

NodeApiJsiRuntime::NodeApiRefCountedPointerValue *
NodeApiJsiRuntime::cloneNodeApiPointerValue(const PointerValue *pointerValue) {
  return static_cast<const NodeApiPointerValue *>(pointerValue)->clone(*this);
}

// Adopted from Hermes code.
std::optional<uint32_t> NodeApiJsiRuntime::toArrayIndex(
    std::string::const_iterator first,
    std::string::const_iterator last) {
  // Empty string is invalid.
  if (first == last)
    return std::nullopt;

  // Leading 0 is special.
  if (*first == '0') {
    ++first;
    // Just "0"?
    if (first == last)
      return 0;
    // Leading 0 is invalid otherwise.
    return std::nullopt;
  }

  uint32_t res = 0;
  do {
    auto ch = *first;
    if (ch < '0' || ch > '9')
      return std::nullopt;
    uint64_t tmp = (uint64_t)res * 10 + (ch - '0');
    // Check for overflow.
    if (tmp & ((uint64_t)0xFFFFFFFFu << 32))
      return std::nullopt;
    res = (uint32_t)tmp;
  } while (++first != last);

  // 0xFFFFFFFF is not a valid array index.
  if (res == 0xFFFFFFFFu)
    return std::nullopt;

  return res;
}

template <typename T, std::enable_if_t<std::is_same_v<jsi::Object, T>, int>>
T NodeApiJsiRuntime::makeJsiPointer(napi_value value) const {
  return make<T>(NodeApiRefCountedPointerValue::make(
                     *const_cast<NodeApiJsiRuntime *>(this),
                     value,
                     NodeApiPointerValueKind::Object)
                     .release());
}

template <typename T, std::enable_if_t<std::is_same_v<jsi::String, T>, int>>
T NodeApiJsiRuntime::makeJsiPointer(napi_value value) const {
  return make<T>(NodeApiRefCountedPointerValue::make(
                     *const_cast<NodeApiJsiRuntime *>(this),
                     value,
                     NodeApiPointerValueKind::String)
                     .release());
}

template <typename T, std::enable_if_t<std::is_same_v<jsi::Symbol, T>, int>>
T NodeApiJsiRuntime::makeJsiPointer(napi_value value) const {
  return make<T>(NodeApiRefCountedPointerValue::make(
                     *const_cast<NodeApiJsiRuntime *>(this),
                     value,
                     NodeApiPointerValueKind::Symbol)
                     .release());
}

#if JSI_VERSION >= 6
template <typename T, std::enable_if_t<std::is_same_v<jsi::BigInt, T>, int>>
T NodeApiJsiRuntime::makeJsiPointer(napi_value value) const {
  return make<T>(NodeApiRefCountedPointerValue::make(
                     *const_cast<NodeApiJsiRuntime *>(this),
                     value,
                     NodeApiPointerValueKind::BigInt)
                     .release());
}
#endif

template <
    typename TTo,
    typename TFrom,
    std::enable_if_t<std::is_base_of_v<jsi::Pointer, TTo>, int>,
    std::enable_if_t<std::is_base_of_v<jsi::Pointer, TFrom>, int>>
TTo NodeApiJsiRuntime::cloneAs(const TFrom &pointer) const {
  return make<TTo>(static_cast<const NodeApiRefCountedPointerValue *>(
                       getPointerValue(pointer))
                       ->clone(*const_cast<NodeApiJsiRuntime *>(this)));
}

NodeApiJsiRuntime::NodeApiRefHolder NodeApiJsiRuntime::makeNodeApiRef(
    napi_value value,
    NodeApiPointerValueKind pointerKind,
    int32_t initialRefCount) {
  return NodeApiRefCountedPointerValue::make(
      *this, value, pointerKind, initialRefCount);
}

void NodeApiJsiRuntime::addStackValue(NodeApiStackValuePtr stackPointer) {
  stackValues_.push_back(std::move(stackPointer));
}

void NodeApiJsiRuntime::pushPointerValueScope() noexcept {
  stackScopes_.push_back(stackValues_.size());
}

void NodeApiJsiRuntime::popPointerValueScope() noexcept {
  CHECK_ELSE_CRASH(!stackScopes_.empty(), "There are no scopes to pop");
  size_t newStackSize = stackScopes_.back();
  auto beginIterator = stackValues_.begin() + newStackSize;
  stackScopes_.pop_back();
  std::for_each(
      beginIterator, stackValues_.end(), [this](NodeApiStackValuePtr &ptr) {
        ptr.release()->deleteStackValue(*this);
      });
  stackValues_.resize(newStackSize);

  pendingDeletions_->deletePointerValues(*this);
}

} // namespace

std::unique_ptr<jsi::Runtime> makeNodeApiJsiRuntime(
    napi_env env,
    JSRuntimeApi *jsrApi,
    std::function<void()> onDelete) noexcept {
  return std::make_unique<NodeApiJsiRuntime>(env, jsrApi, std::move(onDelete));
}

} // namespace Microsoft::NodeApiJsi

EXTERN_C_START

// Default implementation of jsr_get_description if it is not provided by JS
// engine. It returns "NodeApiJsiRuntime" string.
napi_status NAPI_CDECL
default_jsr_get_description(napi_env /*env*/, const char **result) {
  if (result != nullptr) {
    *result = "NodeApiJsiRuntime";
  }
  return napi_ok;
}

// Default implementation of jsr_queue_microtask if it is not provided by JS
// engine. It does nothing.
napi_status NAPI_CDECL
default_jsr_queue_microtask(napi_env /*env*/, napi_value /*callback*/) {
  return napi_generic_failure;
}

// Default implementation of jsr_drain_microtasks if it is not provided by JS
// engine. It does nothing.
napi_status NAPI_CDECL default_jsr_drain_microtasks(
    napi_env /*env*/,
    int32_t /*max_count_hint*/,
    bool *result) {
  if (result != nullptr) {
    *result = true; // All tasks are drained
  }
  return napi_ok;
}

// Default implementation of jsr_is_inspectable if it is not provided by JS
// engine. It always returns false.
napi_status NAPI_CDECL
default_jsr_is_inspectable(napi_env /*env*/, bool *result) {
  if (result != nullptr) {
    *result = false;
  }
  return napi_ok;
}

// Default implementation of jsr_open_napi_env_scope if it is not provided by JS
// engine.
napi_status NAPI_CDECL default_jsr_open_napi_env_scope(
    napi_env /*env*/,
    jsr_napi_env_scope * /*scope*/) {
  return napi_ok;
}

// Default implementation of jsr_close_napi_env_scope if it is not provided by
// JS engine.
napi_status NAPI_CDECL default_jsr_close_napi_env_scope(
    napi_env /*env*/,
    jsr_napi_env_scope /*scope*/) {
  return napi_ok;
}

// TODO: Ensure that we either load all three functions or use their default
// versions and never mix and match.

// Default implementation of jsr_create_prepared_script if it is not provided by
// JS engine. It return napi_ref as a jsr_prepared_script that wraps up an
// object with a "script" property string.
napi_status NAPI_CDECL default_jsr_create_prepared_script(
    napi_env env,
    const uint8_t *script_data,
    size_t script_length,
    jsr_data_delete_cb script_delete_cb,
    void *deleter_data,
    const char * /*source_url*/,
    jsr_prepared_script *result) {
  Microsoft::NodeApiJsi::JSRuntimeApi *jsrApi =
      Microsoft::NodeApiJsi::JSRuntimeApi::current();
  napi_value script{}, obj{};
  // Do not use NAPI_CALL - we must finalize the buffer right after we attempted
  // the string creation.
  napi_status status = jsrApi->napi_create_string_utf8(
      env, reinterpret_cast<const char *>(script_data), script_length, &script);
  if (script_delete_cb != nullptr) {
    script_delete_cb(const_cast<uint8_t *>(script_data), deleter_data);
  }
  NAPI_CALL(status);
  NAPI_CALL(jsrApi->napi_create_object(env, &obj));
  NAPI_CALL(jsrApi->napi_set_named_property(env, obj, "script", script));
  return jsrApi->napi_create_reference(
      env, obj, 1, reinterpret_cast<napi_ref *>(result));
}

// Default implementation of jsr_delete_prepared_script if it is not provided by
// JS engine. It deletes prepared_script as a napi_ref.
napi_status NAPI_CDECL default_jsr_delete_prepared_script(
    napi_env env,
    jsr_prepared_script prepared_script) {
  Microsoft::NodeApiJsi::JSRuntimeApi *jsrApi =
      Microsoft::NodeApiJsi::JSRuntimeApi::current();
  return jsrApi->napi_delete_reference(
      env, reinterpret_cast<napi_ref>(prepared_script));
}

// Default implementation of jsr_prepared_script_run if it is not provided by JS
// engine. It interprets prepared_script as a napi_ref to an object with a
// "script" property string.
napi_status NAPI_CDECL default_jsr_prepared_script_run(
    napi_env env,
    jsr_prepared_script prepared_script,
    napi_value *result) {
  Microsoft::NodeApiJsi::JSRuntimeApi *jsrApi =
      Microsoft::NodeApiJsi::JSRuntimeApi::current();
  napi_value obj{}, script{};
  NAPI_CALL(jsrApi->napi_get_reference_value(
      env, reinterpret_cast<napi_ref>(prepared_script), &obj));
  NAPI_CALL(jsrApi->napi_get_named_property(env, obj, "script", &script));
  return jsrApi->napi_run_script(env, script, result);
}

EXTERN_C_END
