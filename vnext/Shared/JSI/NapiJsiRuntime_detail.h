// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

// JSI
#include <js_native_ext_api.h>
#include <jsi/jsi.h>

// Standard Library
#include <string_view>

#ifdef __cpp_lib_span
#include std::span;
#endif // __cpp_lib_span

// We use macros to report errors.
// Macros provide more flexibility to show assert and provide failure context.

// Check condition and crash process if it fails.
#define CHECK_ELSE_CRASH(condition, message)               \
  do {                                                     \
    if (!(condition)) {                                    \
      assert(false && "Failed: " #condition && (message)); \
      *((int *)0) = 1;                                     \
    }                                                      \
  } while (false)

// Check condition and throw native exception if it fails.
#define CHECK_ELSE_THROW(condition, message) \
  do {                                       \
    if (!(condition)) {                      \
      ThrowNativeException(message);         \
    }                                        \
  } while (false)

// Check NAPI result and and throw JS exception if it is not napi_ok.
#define CHECK_NAPI(expression)                      \
  do {                                              \
    napi_status temp_error_code_ = (expression);    \
    if (temp_error_code_ != napi_status::napi_ok) { \
      ThrowJsException(temp_error_code_);           \
    }                                               \
  } while (false)

// Check NAPI result and and crash if it is not napi_ok.
#define CHECK_NAPI_ELSE_CRASH(expression)              \
  do {                                                 \
    napi_status temp_error_code_ = (expression);       \
    if (temp_error_code_ != napi_status::napi_ok) {    \
      CHECK_ELSE_CRASH(false, "Failed: " #expression); \
    }                                                  \
  } while (false)

namespace Microsoft::JSI {

namespace {

#ifdef __cpp_lib_span
using std::span;
#else
/**
 * @brief A span of values that can be used to pass arguments to a function.
 *
 * This should be replaced with std::span once C++ 2020 is supported.
 */
template <typename T>
struct span {
  constexpr span(std::initializer_list<T> il) noexcept : m_data{const_cast<T *>(il.begin())}, m_size{il.size()} {}
  constexpr span(T *data, size_t size) noexcept : m_data{data}, m_size{size} {}

  [[nodiscard]] constexpr T *data() const noexcept {
    return m_data;
  }

  [[nodiscard]] constexpr size_t size() const noexcept {
    return m_size;
  }

  [[nodiscard]] constexpr T *begin() const noexcept {
    return m_data;
  }

  [[nodiscard]] constexpr T *end() const noexcept {
    return *(m_data + m_size);
  }

  const T &operator[](size_t index) const noexcept {
    return *(m_data + index);
  }

 private:
  T *m_data;
  size_t m_size;
};
#endif // __cpp_lib_span

// Implementation of N-API JSI Runtime
struct NapiJsiRuntime : facebook::jsi::Runtime {
  NapiJsiRuntime(napi_env env) noexcept;

#pragma region facebook::jsi::Runtime

  facebook::jsi::Value evaluateJavaScript(
      const std::shared_ptr<const facebook::jsi::Buffer> &buffer,
      const std::string &sourceURL) override;

  std::shared_ptr<const facebook::jsi::PreparedJavaScript> prepareJavaScript(
      const std::shared_ptr<const facebook::jsi::Buffer> &buffer,
      std::string sourceURL) override;

  facebook::jsi::Value evaluatePreparedJavaScript(
      const std::shared_ptr<const facebook::jsi::PreparedJavaScript> &js) override;

  bool drainMicrotasks(int maxMicrotasksHint = -1) override;

  facebook::jsi::Object global() override;

  std::string description() override;

  bool isInspectable() override;

  // We use the default instrumentation() implementation that returns an
  // Instrumentation instance which returns no metrics.

 protected:
  PointerValue *cloneSymbol(const PointerValue *pointerValue) override;
  PointerValue *cloneString(const PointerValue *pointerValue) override;
  PointerValue *cloneObject(const PointerValue *pointerValue) override;
  PointerValue *clonePropNameID(const PointerValue *pointerValue) override;

  facebook::jsi::PropNameID createPropNameIDFromAscii(const char *str, size_t length) override;
  facebook::jsi::PropNameID createPropNameIDFromUtf8(const uint8_t *utf8, size_t length) override;
  facebook::jsi::PropNameID createPropNameIDFromString(const facebook::jsi::String &str) override;
  std::string utf8(const facebook::jsi::PropNameID &id) override;
  bool compare(const facebook::jsi::PropNameID &lhs, const facebook::jsi::PropNameID &rhs) override;

  std::string symbolToString(const facebook::jsi::Symbol &s) override;

  facebook::jsi::String createStringFromAscii(const char *str, size_t length) override;
  facebook::jsi::String createStringFromUtf8(const uint8_t *utf8, size_t length) override;
  std::string utf8(const facebook::jsi::String &string) override;

  facebook::jsi::Object createObject() override;
  facebook::jsi::Object createObject(std::shared_ptr<facebook::jsi::HostObject> ho) override;
  std::shared_ptr<facebook::jsi::HostObject> getHostObject(const facebook::jsi::Object &) override;
  facebook::jsi::HostFunctionType &getHostFunction(const facebook::jsi::Function &) override;

  facebook::jsi::Value getProperty(const facebook::jsi::Object &obj, const facebook::jsi::PropNameID &name) override;
  facebook::jsi::Value getProperty(const facebook::jsi::Object &obj, const facebook::jsi::String &name) override;
  bool hasProperty(const facebook::jsi::Object &obj, const facebook::jsi::PropNameID &name) override;
  bool hasProperty(const facebook::jsi::Object &obj, const facebook::jsi::String &name) override;
  void setPropertyValue(
      facebook::jsi::Object &obj,
      const facebook::jsi::PropNameID &name,
      const facebook::jsi::Value &value) override;
  void setPropertyValue(
      facebook::jsi::Object &obj,
      const facebook::jsi::String &name,
      const facebook::jsi::Value &value) override;

  bool isArray(const facebook::jsi::Object &obj) const override;
  bool isArrayBuffer(const facebook::jsi::Object &obj) const override;
  bool isFunction(const facebook::jsi::Object &obj) const override;
  bool isHostObject(const facebook::jsi::Object &obj) const override;
  bool isHostFunction(const facebook::jsi::Function &func) const override;

  // Returns the names of all enumerable properties of an object.
  // This corresponds to the properties iterated through by the JavaScript for..in loop.
  facebook::jsi::Array getPropertyNames(const facebook::jsi::Object &obj) override;

  facebook::jsi::WeakObject createWeakObject(const facebook::jsi::Object &obj) override;
  facebook::jsi::Value lockWeakObject(facebook::jsi::WeakObject &weakObj) override;

  facebook::jsi::Array createArray(size_t length) override;
  size_t size(const facebook::jsi::Array &arr) override;
  size_t size(const facebook::jsi::ArrayBuffer &arrBuf) override;

  // The lifetime of the buffer returned is the same as the lifetime of the ArrayBuffer.
  // The returned buffer pointer does not count as a reference to ArrayBuffer for the purpose of garbage collection.
  uint8_t *data(const facebook::jsi::ArrayBuffer &arrBuff) override;
  facebook::jsi::Value getValueAtIndex(const facebook::jsi::Array &arr, size_t index) override;
  void setValueAtIndexImpl(facebook::jsi::Array &arr, size_t index, const facebook::jsi::Value &value) override;

  facebook::jsi::Function createFunctionFromHostFunction(
      const facebook::jsi::PropNameID &name,
      unsigned int paramCount,
      facebook::jsi::HostFunctionType type) override;
  facebook::jsi::Value call(
      const facebook::jsi::Function &func,
      const facebook::jsi::Value &jsThis,
      const facebook::jsi::Value *args,
      size_t count) override;
  facebook::jsi::Value
  callAsConstructor(const facebook::jsi::Function &func, const facebook::jsi::Value *args, size_t count) override;

  // Corresponds to napi_open_handle_scope and napi_close_handle_scope.
  ScopeState *pushScope() override;
  void popScope(ScopeState *) override;

  bool strictEquals(const facebook::jsi::Symbol &a, const facebook::jsi::Symbol &b) const override;
  bool strictEquals(const facebook::jsi::String &a, const facebook::jsi::String &b) const override;
  bool strictEquals(const facebook::jsi::Object &a, const facebook::jsi::Object &b) const override;

  bool instanceOf(const facebook::jsi::Object &obj, const facebook::jsi::Function &func) override;

#pragma endregion facebook::jsi::facebook::jsi::Runtime
 private:
  // Smart pointer to napi_env
  struct EnvHolder {
    EnvHolder(napi_env env) noexcept;
    ~EnvHolder() noexcept;

    EnvHolder(const EnvHolder &) = delete;
    EnvHolder &operator=(const EnvHolder &) = delete;

    operator napi_env() const noexcept;

   private:
    napi_env m_env{};
  };

  // RAII struct to open and close the environment scope.
  struct EnvScope {
    EnvScope(napi_env env) noexcept;
    ~EnvScope() noexcept;

   private:
    napi_env m_env{};
    napi_ext_env_scope m_envScope{};
  };

  // Sets the variable in the constructor and then restores its value in the destructor.
  template <typename T>
  struct AutoRestore {
    AutoRestore(T *var, T value);
    ~AutoRestore();

   private:
    T *m_var;
    T m_value;
  };

  // NapiRefHolder is a smart pointer to napi_ext_ref.
  struct NapiRefHolder {
    NapiRefHolder(std::nullptr_t = nullptr) noexcept {}
    explicit NapiRefHolder(NapiJsiRuntime *runtime, napi_ext_ref ref) noexcept;
    explicit NapiRefHolder(NapiJsiRuntime *runtime, napi_value value);

    // The class is movable.
    NapiRefHolder(NapiRefHolder &&other) noexcept;
    NapiRefHolder &operator=(NapiRefHolder &&other) noexcept;

    // The class is not copyable.
    NapiRefHolder &operator=(NapiRefHolder const &other) = delete;
    NapiRefHolder(NapiRefHolder const &other) = delete;

    ~NapiRefHolder() noexcept;

    [[nodiscard]] napi_ext_ref CloneRef() const noexcept;
    operator napi_value() const;

    explicit operator bool() const noexcept;

   private:
    NapiJsiRuntime *m_runtime{};
    napi_ext_ref m_ref{};
  };

  // NapiPointerValueView is the base class for NapiPointerValue.
  // It holds either napi_value or napi_ext_ref. It does nothing in its invalidate() method.
  // It is used directly by the JsiValueView, JsiValueViewArgs, and PropNameIDView classes
  // to keep temporary PointerValues on the call stack and avoid extra memory allocations.
  // In these cases, it is assumed that it holds a napi_value instead of napi_ext_ref.
  struct NapiPointerValueView : PointerValue {
    NapiPointerValueView(const NapiJsiRuntime *runtime, void *valueOrRef) noexcept;
    void invalidate() noexcept override;

    NapiPointerValueView(const NapiPointerValueView &) = delete;
    NapiPointerValueView &operator=(const NapiPointerValueView &) = delete;

    virtual napi_value GetValue() const;
    virtual napi_ext_ref GetRef() const;
    const NapiJsiRuntime *GetRuntime() const noexcept;

   private:
    const NapiJsiRuntime *m_runtime; // TODO: Initialize here?
    void *m_valueOrRef; // napi_value or napi_ext_ref
  };

  // NapiPointerValue is used by facebook::jsi::Pointer class and must only be used for this purpose.
  // Every instance of NapiPointerValue should be allocated on the heap and be used as an argument
  // to the constructor of facebook::jsi::Pointer or one of its derived classes.
  // facebook::jsi::Pointer makes sure that the invalidate() method, which frees the heap-allocated NapiPointerValue,
  // is called upon destruction.
  // Since the the constructor of facebook::jsi::Pointer is protected, we usually have to invoke it through
  // facebook::jsi::Runtime::make. The code should look something like:
  //
  //    make<Pointer>(new NapiPointerValue(...));
  //
  // or you can use the helper function MakePointer().
  struct NapiPointerValue final : NapiPointerValueView {
    NapiPointerValue(const NapiJsiRuntime *runtime, napi_ext_ref ref);
    NapiPointerValue(const NapiJsiRuntime *runtime, napi_value value);
    void invalidate() noexcept override;

    napi_value GetValue() const override;
    napi_ext_ref GetRef() const override;

   private:
    // ~NapiPointerValue() must only be invoked by invalidate(). Hence, we make it private.
    ~NapiPointerValue() noexcept override;
  };

  // SmallBuffer keeps InplaceSize elements in place in the class, and uses heap memory for more elements.
  template <typename T, size_t InplaceSize>
  struct SmallBuffer {
    SmallBuffer(size_t size) noexcept;
    T *Data() noexcept;
    size_t Size() const noexcept;

   private:
    size_t m_size{};
    std::array<T, InplaceSize> m_stackData{};
    std::unique_ptr<T[]> m_heapData{};
  };

  // The number of arguments that we keep on stack. We use heap if we have more arguments.
  constexpr static size_t MaxStackArgCount = 8;

  // NapiValueArgs helps optimize passing arguments to NAPI functions.
  // If number of arguments is below or equal to MaxStackArgCount, they are kept on the call stack,
  // otherwise arguments are allocated on the heap.
  struct NapiValueArgs {
    NapiValueArgs(NapiJsiRuntime &runtime, span<const facebook::jsi::Value> args);
    operator span<napi_value>();

   private:
    SmallBuffer<napi_value, MaxStackArgCount> m_args;
  };

  // Helps to use the stack storage for a temporary conversion from napi_value to facebook::jsi::Value.
  // It also helps to avoid a conversion to a relatively expensive napi_ext_ref.
  struct JsiValueView {
    JsiValueView(NapiJsiRuntime *runtime, napi_value jsValue);
    operator const facebook::jsi::Value &() const noexcept;

    using StoreType = std::aligned_storage_t<sizeof(NapiPointerValueView)>;
    static facebook::jsi::Value InitValue(NapiJsiRuntime *runtime, napi_value jsValue, StoreType *store);

   private:
    StoreType m_pointerStore{};
    facebook::jsi::Value m_value{};
  };

  // Helps to use stack storage for passing arguments that must be temporarily converted
  // from napi_value to facebook::jsi::value.
  // It helps to avoid conversion to a relatively expensive napi_ext_ref.
  struct JsiValueViewArgs {
    JsiValueViewArgs(NapiJsiRuntime *runtime, span<napi_value> args) noexcept;
    const facebook::jsi::Value *Data() noexcept;
    size_t Size() const noexcept;

   private:
    SmallBuffer<JsiValueView::StoreType, MaxStackArgCount> m_pointerStore{0};
    SmallBuffer<facebook::jsi::Value, MaxStackArgCount> m_args{0};
  };

  // Helps to use stack storage for a temporary conversion from napi_value to facebook::jsi::PropNameID.
  // It helps to avoid conversions to a relatively expensive napi_ext_ref.
  struct PropNameIDView {
    PropNameIDView(NapiJsiRuntime *runtime, napi_value propertyId) noexcept;
    operator const facebook::jsi::PropNameID &() const noexcept;

    using StoreType = std::aligned_storage_t<sizeof(NapiPointerValueView)>;

   private:
    StoreType m_pointerStore{};
    facebook::jsi::PropNameID const m_propertyId;
  };

  // Wraps up the facebook::jsi::HostFunctionType along with the NapiJsiRuntime.
  struct HostFunctionWrapper {
    HostFunctionWrapper(facebook::jsi::HostFunctionType &&hostFunction, NapiJsiRuntime &runtime);

    // Does not suppor copying.
    HostFunctionWrapper(const HostFunctionWrapper &) = delete;
    HostFunctionWrapper &operator=(const HostFunctionWrapper &) = delete;

    facebook::jsi::HostFunctionType &GetHosFunction() noexcept;
    NapiJsiRuntime &GetRuntime() noexcept;

   private:
    facebook::jsi::HostFunctionType m_hostFunction;
    NapiJsiRuntime &m_runtime;
  };

  // Packs the source buffer and the byte code together.
  struct NapiPreparedJavaScript final : facebook::jsi::PreparedJavaScript {
    NapiPreparedJavaScript(
        std::unique_ptr<const facebook::jsi::Buffer> serializedBuffer,
        const std::shared_ptr<const facebook::jsi::Buffer> &sourceBuffer,
        std::string sourceUrl);

    const facebook::jsi::Buffer &SerializedBuffer() const;
    const facebook::jsi::Buffer &SourceBuffer() const;
    const std::string &SourceUrl() const;

   private:
    std::unique_ptr<const facebook::jsi::Buffer> m_serializedBuffer;
    std::shared_ptr<const facebook::jsi::Buffer> m_sourceBuffer;
    std::string m_sourceUrl;
  };

  // Implements facebook::jsi::Buffer using std::vector<uint8_t>.
  struct VectorBuffer final : facebook::jsi::Buffer {
    VectorBuffer(std::vector<uint8_t> data);
    uint8_t const *data() const override;
    size_t size() const override;

   private:
    std::vector<uint8_t> m_data;
  };

 private: // Error-handling utility methods
  [[noreturn]] void ThrowJsException(napi_status errorCode) const;
  [[noreturn]] void ThrowNativeException(char const *errorMessage) const;
  void RewriteErrorMessage(napi_value jsError) const;
  template <typename TLambda>
  auto RunInMethodContext(char const *methodName, TLambda lambda);
  template <typename TLambda>
  napi_value HandleCallbackExceptions(TLambda lambda) const noexcept;
  bool SetException(napi_value error) const noexcept;
  bool SetException(std::string_view message) const noexcept;

 private: // Shared NAPI call helpers
  napi_value RunScript(napi_value script, const char *sourceUrl);
  std::vector<uint8_t> SerializeScript(napi_value script, const char *sourceUrl);
  napi_value RunSerializedScript(span<const uint8_t> serialized, napi_value source, const char *sourceUrl);
  napi_ext_ref CreateReference(napi_value value) const;
  void ReleaseReference(napi_ext_ref ref) const;
  napi_value GetReferenceValue(napi_ext_ref ref) const;
  napi_valuetype TypeOf(napi_value value) const;
  bool StrictEquals(napi_value left, napi_value right) const;
  napi_value GetUndefined() const;
  napi_value GetNull() const;
  napi_value GetGlobal() const;
  napi_value GetBoolean(bool value) const;
  bool GetValueBool(napi_value value) const;
  napi_value CreateInt32(int32_t value) const;
  napi_value CreateDouble(double value) const;
  double GetValueDouble(napi_value value) const;
  napi_value CreateStringLatin1(std::string_view value) const;
  napi_value CreateStringUtf8(std::string_view value) const;
  napi_value CreateStringUtf8(const uint8_t *data, size_t length) const;
  std::string StringToStdString(napi_value stringValue) const;
  napi_ext_ref GetPropertyIdFromName(std::string_view value) const;
  napi_ext_ref GetPropertyIdFromName(const uint8_t *data, size_t length) const;
  napi_ext_ref GetPropertyIdFromName(napi_value str) const;
  std::string PropertyIdToStdString(napi_value propertyId);
  napi_value CreateSymbol(std::string_view symbolDescription) const;
  std::string SymbolToStdString(napi_value symbolValue);
  napi_value CallFunction(napi_value thisArg, napi_value function, span<napi_value> args = {}) const;
  napi_value ConstructObject(napi_value constructor, span<napi_value> args = {}) const;
  bool InstanceOf(napi_value object, napi_value constructor) const;
  napi_value CreateObject() const;
  bool HasProperty(napi_value object, napi_value propertId) const;
  napi_value GetProperty(napi_value object, napi_value propertyId) const;
  void SetProperty(napi_value object, napi_value propertyId, napi_value value) const;
  void SetProperty(napi_value object, napi_value propertyId, napi_value value, napi_property_attributes attrs) const;
  napi_value CreateArray(size_t length) const;
  void SetElement(napi_value array, uint32_t index, napi_value value) const;
  static napi_value JsiHostFunctionCallback(napi_env env, napi_callback_info info) noexcept;
  napi_value CreateExternalFunction(napi_value name, int32_t paramCount, napi_callback callback, void *callbackData);
  napi_value CreateExternalObject(void *data, napi_finalize finalizeCallback) const;
  template <typename T>
  napi_value CreateExternalObject(std::unique_ptr<T> &&data) const;
  void *GetExternalData(napi_value object) const;
  const std::shared_ptr<facebook::jsi::HostObject> &GetJsiHostObject(napi_value obj);
  napi_value GetHostObjectProxyHandler();
  template <napi_value (NapiJsiRuntime::*trapMethod)(span<napi_value>), size_t argCount>
  void SetProxyTrap(napi_value handler, napi_value propertyName);
  napi_value HostObjectGetTrap(span<napi_value> args);
  napi_value HostObjectSetTrap(span<napi_value> args);
  napi_value HostObjectOwnKeysTrap(span<napi_value> args);
  napi_value HostObjectGetOwnPropertyDescriptorTrap(span<napi_value> args);

 private: // Miscellaneous utility methods
  span<const uint8_t> ToSpan(const facebook::jsi::Buffer &buffer);
  facebook::jsi::Value ToJsiValue(napi_value value) const;
  napi_value GetNapiValue(const facebook::jsi::Value &value) const;
  static NapiPointerValue *CloneNapiPointerValue(const PointerValue *pointerValue);
  static napi_value GetNapiValue(const facebook::jsi::Pointer &p);
  static napi_ext_ref GetNapiRef(const facebook::jsi::Pointer &p);

  template <typename T, typename TValue, std::enable_if_t<std::is_base_of_v<facebook::jsi::Pointer, T>, int> = 0>
  T MakePointer(TValue value) const;

 private: // Fields
  EnvHolder m_env;

  // Property ID cache to improve execution speed.
  struct PropertyId {
    NapiRefHolder Error;
    NapiRefHolder Object;
    NapiRefHolder Proxy;
    NapiRefHolder Symbol;
    NapiRefHolder byteLength;
    NapiRefHolder configurable;
    NapiRefHolder enumerable;
    NapiRefHolder get;
    NapiRefHolder getOwnPropertyDescriptor;
    NapiRefHolder hostFunctionSymbol;
    NapiRefHolder hostObjectSymbol;
    NapiRefHolder length;
    NapiRefHolder message;
    NapiRefHolder ownKeys;
    NapiRefHolder propertyIsEnumerable;
    NapiRefHolder prototype;
    NapiRefHolder set;
    NapiRefHolder toString;
    NapiRefHolder value;
    NapiRefHolder writable;
  } m_propertyId;

  // Cache of commonly used values.
  struct CachedValue final {
    NapiRefHolder Error;
    NapiRefHolder Global;
    NapiRefHolder False;
    NapiRefHolder HostObjectProxyHandler;
    NapiRefHolder Null;
    NapiRefHolder ProxyConstructor;
    NapiRefHolder SymbolToString;
    NapiRefHolder True;
    NapiRefHolder Undefined;
  } m_value;

  bool m_pendingJSError{false};
};
} // namespace

} // namespace Microsoft::JSI
