// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#define NAPI_EXPERIMENTAL

#include "NodeApiJsiRuntime.h"

// Standard Library
#include <string_view>
#include <unordered_set>

#pragma region Macros

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

#pragma endregion Macros

#pragma region Declarations

#ifdef __cpp_lib_span
#include <span>
#endif // __cpp_lib_span

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
  std::string utf8(const facebook::jsi::String &str) override;

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
    const NapiJsiRuntime *m_runtime;
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

    facebook::jsi::HostFunctionType &GetHostFunction() noexcept;
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
  bool HasProperty(napi_value object, napi_value propertyId) const;
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

#pragma endregion Declarations

using namespace facebook::jsi;
using namespace std::string_view_literals;

using std::shared_ptr;
using std::string;
using std::string_view;
using std::unique_ptr;
using std::vector;

namespace Microsoft::JSI {

namespace {
constexpr char s_description[] = "NapiJsiRuntime";

constexpr char s_refHolderError[] = "Error";
constexpr char s_refHolderObject[] = "Object";
constexpr char s_refHolderProxy[] = "Proxy";
constexpr char s_refHolderSymbol[] = "Symbol";
constexpr char s_refHolderByteLength[] = "byteLength";
constexpr char s_refHolderConfigurable[] = "configurable";
constexpr char s_refHolderEnumerable[] = "enumerable";
constexpr char s_refHolderGet[] = "get";
constexpr char s_refHolderGetOwnPropertyDescriptor[] = "getOwnPropertyDescriptor";
constexpr char s_refHolderHostFunctionSymbol[] = "hostFunctionSymbol";
constexpr char s_refHolderHostObjectSymbol[] = "hostObjectSymbol";
constexpr char s_refHolderLength[] = "length";
constexpr char s_refHolderMessage[] = "message";
constexpr char s_refHolderOwnKeys[] = "ownKeys";
constexpr char s_refHolderPropertyIsEnumerable[] = "propertyIsEnumerable";
constexpr char s_refHolderPrototype[] = "prototype";
constexpr char s_refHolderSet[] = "set";
constexpr char s_refHolderToString[] = "toString";
constexpr char s_refHolderValue[] = "value";
constexpr char s_refHolderWritable[] = "writable";

#pragma region NapiJsiRuntime

NapiJsiRuntime::NapiJsiRuntime(napi_env env) noexcept : m_env{env} {
  EnvScope scope{m_env};
  m_propertyId.Error = NapiRefHolder{this, GetPropertyIdFromName(s_refHolderError)};
  m_propertyId.Object = NapiRefHolder{this, GetPropertyIdFromName(s_refHolderObject)};
  m_propertyId.Proxy = NapiRefHolder{this, GetPropertyIdFromName(s_refHolderProxy)};
  m_propertyId.Symbol = NapiRefHolder{this, GetPropertyIdFromName(s_refHolderSymbol)};
  m_propertyId.byteLength = NapiRefHolder{this, GetPropertyIdFromName(s_refHolderByteLength)};
  m_propertyId.configurable = NapiRefHolder{this, GetPropertyIdFromName(s_refHolderConfigurable)};
  m_propertyId.enumerable = NapiRefHolder{this, GetPropertyIdFromName(s_refHolderEnumerable)};
  m_propertyId.get = NapiRefHolder{this, GetPropertyIdFromName(s_refHolderGet)};
  m_propertyId.getOwnPropertyDescriptor =
      NapiRefHolder{this, GetPropertyIdFromName(s_refHolderGetOwnPropertyDescriptor)};
  m_propertyId.hostFunctionSymbol = NapiRefHolder{this, CreateSymbol(s_refHolderHostFunctionSymbol)};
  m_propertyId.hostObjectSymbol = NapiRefHolder{this, CreateSymbol(s_refHolderHostObjectSymbol)};
  m_propertyId.length = NapiRefHolder{this, GetPropertyIdFromName(s_refHolderLength)};
  m_propertyId.message = NapiRefHolder{this, GetPropertyIdFromName(s_refHolderMessage)};
  m_propertyId.ownKeys = NapiRefHolder{this, GetPropertyIdFromName(s_refHolderOwnKeys)};
  m_propertyId.propertyIsEnumerable = NapiRefHolder{this, GetPropertyIdFromName(s_refHolderPropertyIsEnumerable)};
  m_propertyId.prototype = NapiRefHolder{this, GetPropertyIdFromName(s_refHolderPrototype)};
  m_propertyId.set = NapiRefHolder{this, GetPropertyIdFromName(s_refHolderSet)};
  m_propertyId.toString = NapiRefHolder{this, GetPropertyIdFromName(s_refHolderToString)};
  m_propertyId.value = NapiRefHolder{this, GetPropertyIdFromName(s_refHolderValue)};
  m_propertyId.writable = NapiRefHolder{this, GetPropertyIdFromName(s_refHolderWritable)};

  m_value.Undefined = NapiRefHolder{this, GetUndefined()};
  m_value.Null = NapiRefHolder{this, GetNull()};
  m_value.True = NapiRefHolder{this, GetBoolean(true)};
  m_value.False = NapiRefHolder{this, GetBoolean(false)};
  m_value.Global = NapiRefHolder{this, GetGlobal()};
  m_value.Error = NapiRefHolder{this, GetProperty(m_value.Global, m_propertyId.Error)};
}

Value NapiJsiRuntime::evaluateJavaScript(const shared_ptr<const Buffer> &buffer, const string &sourceUrl) {
  EnvScope envScope{m_env};
  napi_value script = CreateStringUtf8(buffer->data(), buffer->size());
  napi_value result = RunScript(script, sourceUrl.c_str());

  return ToJsiValue(result);
}

shared_ptr<const PreparedJavaScript> NapiJsiRuntime::prepareJavaScript(
    const shared_ptr<const Buffer> &sourceBuffer,
    string sourceUrl) {
  EnvScope scope{m_env};
  napi_value source = CreateStringUtf8(sourceBuffer->data(), sourceBuffer->size());
  vector<uint8_t> serialized = SerializeScript(source, sourceUrl.c_str());
  unique_ptr<const Buffer> serializedBuffer{new VectorBuffer{std::move(serialized)}};

  return std::make_shared<NapiPreparedJavaScript>(std::move(serializedBuffer), sourceBuffer, std::move(sourceUrl));
}

Value NapiJsiRuntime::evaluatePreparedJavaScript(const shared_ptr<const PreparedJavaScript> &preparedJs) {
  EnvScope scope{m_env};
  auto preparedScript = static_cast<const NapiPreparedJavaScript *>(preparedJs.get());
  napi_value source = CreateStringUtf8(preparedScript->SourceBuffer().data(), preparedScript->SourceBuffer().size());
  napi_value result =
      RunSerializedScript(ToSpan(preparedScript->SerializedBuffer()), source, preparedScript->SourceUrl().c_str());

  return ToJsiValue(result);
}

bool NapiJsiRuntime::drainMicrotasks(int /*maxMicrotasksHint*/) {
  return true;
}

Object NapiJsiRuntime::global() {
  EnvScope scope{m_env};

  return MakePointer<Object>(m_value.Global.CloneRef());
}

string NapiJsiRuntime::description() {
  return s_description;
}

bool NapiJsiRuntime::isInspectable() {
  return false;
}

Runtime::PointerValue *NapiJsiRuntime::cloneSymbol(const Runtime::PointerValue *pointerValue) {
  EnvScope scope{m_env};

  return CloneNapiPointerValue(pointerValue);
}

Runtime::PointerValue *NapiJsiRuntime::cloneString(const Runtime::PointerValue *pointerValue) {
  EnvScope scope{m_env};

  return CloneNapiPointerValue(pointerValue);
}

Runtime::PointerValue *NapiJsiRuntime::cloneObject(const Runtime::PointerValue *pointerValue) {
  EnvScope scope{m_env};

  return CloneNapiPointerValue(pointerValue);
}

Runtime::PointerValue *NapiJsiRuntime::clonePropNameID(const Runtime::PointerValue *pointerValue) {
  EnvScope scope{m_env};

  return CloneNapiPointerValue(pointerValue);
}

PropNameID NapiJsiRuntime::createPropNameIDFromAscii(const char *str, size_t length) {
  EnvScope scope{m_env};
  napi_value napiStr = CreateStringLatin1({str, length});
  napi_ext_ref uniqueStr = GetPropertyIdFromName(napiStr);

  return MakePointer<PropNameID>(uniqueStr);
}

PropNameID NapiJsiRuntime::createPropNameIDFromUtf8(const uint8_t *utf8, size_t length) {
  EnvScope scope{m_env};
  napi_ext_ref uniqueStr = GetPropertyIdFromName(utf8, length);

  return MakePointer<PropNameID>(uniqueStr);
}

PropNameID NapiJsiRuntime::createPropNameIDFromString(const String &str) {
  EnvScope scope{m_env};
  napi_ext_ref uniqueStr = GetPropertyIdFromName(GetNapiValue(str));

  return MakePointer<PropNameID>(uniqueStr);
}

string NapiJsiRuntime::utf8(const PropNameID &id) {
  EnvScope scope{m_env};

  return PropertyIdToStdString(GetNapiValue(id));
}

bool NapiJsiRuntime::compare(const PropNameID &lhs, const PropNameID &rhs) {
  EnvScope scope{m_env};

  return StrictEquals(GetNapiValue(lhs), GetNapiValue(rhs));
}

string NapiJsiRuntime::symbolToString(const Symbol &s) {
  EnvScope scope{m_env};
  if (!m_value.SymbolToString) {
    napi_value symbolCtor = GetProperty(m_value.Global, m_propertyId.Symbol);
    napi_value symbolPrototype = GetProperty(symbolCtor, m_propertyId.prototype);
    m_value.SymbolToString = NapiRefHolder{this, GetProperty(symbolPrototype, m_propertyId.toString)};
  }
  napi_value jsString = CallFunction(GetNapiValue(s), m_value.SymbolToString, {});

  return StringToStdString(jsString);
}

String NapiJsiRuntime::createStringFromAscii(const char *str, size_t length) {
  EnvScope scope{m_env};

  return MakePointer<String>(CreateStringLatin1({str, length}));
}

String NapiJsiRuntime::createStringFromUtf8(const uint8_t *str, size_t length) {
  EnvScope scope{m_env};

  return MakePointer<String>(CreateStringUtf8(str, length));
}

string NapiJsiRuntime::utf8(const String &str) {
  EnvScope scope{m_env};

  return StringToStdString(GetNapiValue(str));
}

Object NapiJsiRuntime::createObject() {
  EnvScope scope{m_env};

  return MakePointer<Object>(CreateObject());
}

Object NapiJsiRuntime::createObject(shared_ptr<HostObject> hostObject) {
  // The hostObjectHolder keeps the hostObject as external data.
  // Then, the hostObjectHolder is wrapped up by a Proxy object to provide access
  // to the hostObject's get, set and getPropertyNames methods.
  // There is a special symbol property ID, 'hostObjectSymbol', used to access the hostObjectWrapper from the Proxy.
  EnvScope scope{m_env};
  napi_value hostObjectHolder = CreateExternalObject(std::make_unique<shared_ptr<HostObject>>(std::move(hostObject)));
  napi_value obj = CreateObject();
  SetProperty(obj, m_propertyId.hostObjectSymbol, hostObjectHolder);
  if (!m_value.ProxyConstructor) {
    m_value.ProxyConstructor = NapiRefHolder{this, GetProperty(m_value.Global, m_propertyId.Proxy)};
  }
  napi_value proxy = ConstructObject(m_value.ProxyConstructor, {obj, GetHostObjectProxyHandler()});

  return MakePointer<Object>(proxy);
}

shared_ptr<HostObject> NapiJsiRuntime::getHostObject(const Object &obj) {
  EnvScope scope{m_env};

  return GetJsiHostObject(GetNapiValue(obj));
}

HostFunctionType &NapiJsiRuntime::getHostFunction(const Function &func) {
  EnvScope scope{m_env};
  napi_value hostFunctionHolder = GetProperty(GetNapiValue(func), m_propertyId.hostFunctionSymbol);
  if (TypeOf(hostFunctionHolder) == napi_valuetype::napi_external) {
    return static_cast<HostFunctionWrapper *>(GetExternalData(hostFunctionHolder))->GetHostFunction();
  } else {
    throw JSINativeException("getHostFunction() can only be called with HostFunction.");
  }
}

Value NapiJsiRuntime::getProperty(const Object &obj, const PropNameID &name) {
  EnvScope scope{m_env};

  return ToJsiValue(GetProperty(GetNapiValue(obj), GetNapiValue(name)));
}

Value NapiJsiRuntime::getProperty(const Object &obj, const String &name) {
  EnvScope scope{m_env};

  return ToJsiValue(GetProperty(GetNapiValue(obj), GetNapiValue(name)));
}

bool NapiJsiRuntime::hasProperty(const Object &obj, const PropNameID &name) {
  EnvScope scope{m_env};

  return HasProperty(GetNapiValue(obj), GetNapiValue(name));
}

bool NapiJsiRuntime::hasProperty(const Object &obj, const String &name) {
  EnvScope scope{m_env};

  return HasProperty(GetNapiValue(obj), GetNapiValue(name));
}

void NapiJsiRuntime::setPropertyValue(Object &obj, const PropNameID &name, const Value &value) {
  EnvScope scope{m_env};

  SetProperty(GetNapiValue(obj), GetNapiValue(name), GetNapiValue(value));
}

void NapiJsiRuntime::setPropertyValue(Object &obj, const String &name, const Value &value) {
  EnvScope scope{m_env};

  SetProperty(GetNapiValue(obj), GetNapiValue(name), GetNapiValue(value));
}

bool NapiJsiRuntime::isArray(const Object &obj) const {
  EnvScope scope{m_env};
  bool result{};
  CHECK_NAPI(napi_is_array(m_env, GetNapiValue(obj), &result));

  return result;
}

bool NapiJsiRuntime::isArrayBuffer(const Object &obj) const {
  EnvScope scope{m_env};
  bool result{};
  CHECK_NAPI(napi_is_arraybuffer(m_env, GetNapiValue(obj), &result));

  return result;
}

bool NapiJsiRuntime::isFunction(const Object &obj) const {
  EnvScope scope{m_env};

  return TypeOf(GetNapiValue(obj)) == napi_valuetype::napi_function;
}

bool NapiJsiRuntime::isHostObject(const Object &obj) const {
  EnvScope scope{m_env};
  napi_value hostObjectHolder = GetProperty(GetNapiValue(obj), m_propertyId.hostObjectSymbol);

  if (TypeOf(hostObjectHolder) == napi_valuetype::napi_external) {
    return GetExternalData(hostObjectHolder) != nullptr;
  } else {
    return false;
  }
}

bool NapiJsiRuntime::isHostFunction(const Function &func) const {
  EnvScope scope{m_env};
  napi_value hostFunctionHolder = GetProperty(GetNapiValue(func), m_propertyId.hostFunctionSymbol);

  if (TypeOf(hostFunctionHolder) == napi_valuetype::napi_external) {
    return GetExternalData(hostFunctionHolder) != nullptr;
  } else {
    return false;
  }
}

Array NapiJsiRuntime::getPropertyNames(const Object &obj) {
  EnvScope scope{m_env};
  napi_value properties;
  CHECK_NAPI(napi_get_all_property_names(
      m_env,
      GetNapiValue(obj),
      napi_key_collection_mode::napi_key_include_prototypes,
      napi_key_filter(napi_key_enumerable | napi_key_skip_symbols),
      napi_key_numbers_to_strings,
      &properties));

  return MakePointer<Object>(properties).asArray(*this);
}

WeakObject NapiJsiRuntime::createWeakObject(const Object &obj) {
  EnvScope scope{m_env};
  napi_ext_ref weakRef{};
  CHECK_NAPI(napi_ext_create_weak_reference(m_env, GetNapiValue(obj), &weakRef));

  return MakePointer<WeakObject>(weakRef);
}

Value NapiJsiRuntime::lockWeakObject(WeakObject &weakObject) {
  EnvScope scope{m_env};
  napi_value value = GetNapiValue(weakObject);

  if (value) {
    return ToJsiValue(value);
  } else {
    return Value::undefined();
  }
}

Array NapiJsiRuntime::createArray(size_t length) {
  EnvScope scope{m_env};
  napi_value result{};
  CHECK_NAPI(napi_create_array_with_length(m_env, length, &result));

  return MakePointer<Object>(result).asArray(*this);
}

size_t NapiJsiRuntime::size(const Array &arr) {
  EnvScope scope{m_env};
  size_t result{};
  CHECK_NAPI(napi_get_array_length(m_env, GetNapiValue(arr), reinterpret_cast<uint32_t *>(&result)));

  return result;
}

size_t NapiJsiRuntime::size(const ArrayBuffer &arrBuf) {
  EnvScope scope{m_env};
  size_t result{};
  CHECK_NAPI(napi_get_arraybuffer_info(m_env, GetNapiValue(arrBuf), nullptr, &result));

  return result;
}

uint8_t *NapiJsiRuntime::data(const ArrayBuffer &arrBuf) {
  EnvScope scope{m_env};
  uint8_t *result{};
  CHECK_NAPI(napi_get_arraybuffer_info(m_env, GetNapiValue(arrBuf), reinterpret_cast<void **>(&result), nullptr));

  return result;
}

Value NapiJsiRuntime::getValueAtIndex(const Array &arr, size_t index) {
  EnvScope scope{m_env};
  napi_value result{};
  CHECK_NAPI(napi_get_element(m_env, GetNapiValue(arr), static_cast<int32_t>(index), &result));

  return ToJsiValue(result);
}

void NapiJsiRuntime::setValueAtIndexImpl(Array &arr, size_t index, const Value &value) {
  EnvScope scope{m_env};

  SetElement(GetNapiValue(arr), static_cast<uint32_t>(index), GetNapiValue(value));
}

Function
NapiJsiRuntime::createFunctionFromHostFunction(const PropNameID &name, unsigned int paramCount, HostFunctionType func) {
  EnvScope scope{m_env};
  auto hostFunctionWrapper = std::make_unique<HostFunctionWrapper>(std::move(func), *this);
  napi_value function = CreateExternalFunction(
      GetNapiValue(name), static_cast<int32_t>(paramCount), JsiHostFunctionCallback, hostFunctionWrapper.get());

  const napi_value hostFunctionHolder = CreateExternalObject(std::move(hostFunctionWrapper));
  SetProperty(function, m_propertyId.hostFunctionSymbol, hostFunctionHolder, napi_property_attributes::napi_default);

  return MakePointer<Object>(function).getFunction(*this);
}

Value NapiJsiRuntime::call(const Function &func, const Value &jsThis, const Value *args, size_t count) {
  EnvScope scope{m_env};

  return ToJsiValue(
      CallFunction(GetNapiValue(jsThis), GetNapiValue(func), NapiValueArgs(*this, span<const Value>(args, count))));
}

Value NapiJsiRuntime::callAsConstructor(const Function &func, const Value *args, size_t count) {
  EnvScope scope{m_env};

  return ToJsiValue(ConstructObject(GetNapiValue(func), NapiValueArgs(*this, span<Value const>(args, count))));
}

Runtime::ScopeState *NapiJsiRuntime::pushScope() {
  EnvScope scope{m_env};
  napi_handle_scope result{};
  CHECK_NAPI(napi_open_handle_scope(m_env, &result));

  return reinterpret_cast<Runtime::ScopeState *>(result);
}

void NapiJsiRuntime::popScope(Runtime::ScopeState *state) {
  EnvScope scope{m_env};

  CHECK_NAPI(napi_close_handle_scope(m_env, reinterpret_cast<napi_handle_scope>(state)));
}

bool NapiJsiRuntime::strictEquals(const Symbol &a, const Symbol &b) const {
  EnvScope scope{m_env};

  return StrictEquals(GetNapiValue(a), GetNapiValue(b));
}

bool NapiJsiRuntime::strictEquals(const String &a, const String &b) const {
  EnvScope scope{m_env};

  return StrictEquals(GetNapiValue(a), GetNapiValue(b));
}

bool NapiJsiRuntime::strictEquals(const Object &a, const Object &b) const {
  EnvScope scope{m_env};

  return StrictEquals(GetNapiValue(a), GetNapiValue(b));
}

bool NapiJsiRuntime::instanceOf(const Object &obj, const Function &func) {
  EnvScope scope{m_env};

  return InstanceOf(GetNapiValue(obj), GetNapiValue(func));
}

#pragma region EnvHolder

NapiJsiRuntime::EnvHolder::EnvHolder(napi_env env) noexcept : m_env{env} {}

NapiJsiRuntime::EnvHolder::~EnvHolder() noexcept {
  if (m_env) {
    CHECK_NAPI_ELSE_CRASH(napi_ext_env_unref(m_env));
  }
}

NapiJsiRuntime::EnvHolder::operator napi_env() const noexcept {
  return m_env;
}

#pragma endregion EnvHolder

#pragma region EnvScope

NapiJsiRuntime::EnvScope::EnvScope(napi_env env) noexcept : m_env{env} {
  CHECK_NAPI_ELSE_CRASH(napi_ext_open_env_scope(m_env, &m_envScope));
}

NapiJsiRuntime::EnvScope::~EnvScope() noexcept {
  CHECK_NAPI_ELSE_CRASH(napi_ext_close_env_scope(m_env, m_envScope));
}

#pragma endregion EnvScope

#pragma region AutoRestore

template <typename T>
NapiJsiRuntime::AutoRestore<T>::AutoRestore(T *var, T value) : m_var{var}, m_value{std::exchange(*var, value)} {}

template <typename T>
NapiJsiRuntime::AutoRestore<T>::~AutoRestore() {
  *m_var = m_value;
}

#pragma endregion AutoRestore

#pragma region NapiRefHolder

NapiJsiRuntime::NapiRefHolder::NapiRefHolder(NapiJsiRuntime *runtime, napi_ext_ref ref) noexcept
    : m_runtime{runtime}, m_ref{ref} {}

NapiJsiRuntime::NapiRefHolder::NapiRefHolder(NapiJsiRuntime *runtime, napi_value value)
    : m_runtime{runtime}, m_ref{m_runtime->CreateReference(value)} {}

NapiJsiRuntime::NapiRefHolder::NapiRefHolder(NapiRefHolder &&other) noexcept
    : m_runtime{std::exchange(other.m_runtime, nullptr)}, m_ref{std::exchange(other.m_ref, nullptr)} {}

NapiJsiRuntime::NapiRefHolder &NapiJsiRuntime::NapiRefHolder::operator=(NapiRefHolder &&other) noexcept {
  if (this != &other) {
    NapiRefHolder temp{std::move(*this)};
    std::swap(m_runtime, other.m_runtime);
    std::swap(m_ref, other.m_ref);
  }

  return *this;
}

NapiJsiRuntime::NapiRefHolder::~NapiRefHolder() noexcept {
  if (m_ref) {
    // Clear m_ref before calling ReleaseReference on it to make sure that we always hold a valid m_ref.
    m_runtime->ReleaseReference(std::exchange(m_ref, nullptr));
  }
}

[[nodiscard]] napi_ext_ref NapiJsiRuntime::NapiRefHolder::CloneRef() const noexcept {
  if (m_ref) {
    napi_ext_reference_ref(m_runtime->m_env, m_ref);
  }

  return m_ref;
}

NapiJsiRuntime::NapiRefHolder::operator napi_value() const {
  return m_runtime->GetReferenceValue(m_ref);
}

NapiJsiRuntime::NapiRefHolder::operator bool() const noexcept {
  return m_ref != nullptr;
}

#pragma endregion NapiRefHolder

#pragma region NapiPointerValueView

NapiJsiRuntime::NapiPointerValueView::NapiPointerValueView(NapiJsiRuntime const *runtime, void *valueOrRef) noexcept
    : m_runtime{runtime}, m_valueOrRef{valueOrRef} {}

// Intentionally do nothing.
void NapiJsiRuntime::NapiPointerValueView::invalidate() noexcept {}

napi_value NapiJsiRuntime::NapiPointerValueView::GetValue() const {
  return reinterpret_cast<napi_value>(m_valueOrRef);
}

napi_ext_ref NapiJsiRuntime::NapiPointerValueView::GetRef() const {
  CHECK_ELSE_CRASH(false, "Not implemented");

  return nullptr;
}

const NapiJsiRuntime *NapiJsiRuntime::NapiPointerValueView::GetRuntime() const noexcept {
  return m_runtime;
}

#pragma endregion NapiPointerValueView

#pragma region NapiPointerValue

NapiJsiRuntime::NapiPointerValue::NapiPointerValue(const NapiJsiRuntime *runtime, napi_ext_ref ref)
    : NapiPointerValueView(runtime, ref) {}

NapiJsiRuntime::NapiPointerValue::NapiPointerValue(const NapiJsiRuntime *runtime, napi_value value)
    : NapiPointerValueView(runtime, runtime->CreateReference(value)) {}

NapiJsiRuntime::NapiPointerValue::~NapiPointerValue() noexcept {
  if (napi_ext_ref ref = GetRef()) {
    GetRuntime()->ReleaseReference(ref);
  }
}

void NapiJsiRuntime::NapiPointerValue::invalidate() noexcept {
  delete this;
}

napi_value NapiJsiRuntime::NapiPointerValue::GetValue() const {
  return GetRuntime()->GetReferenceValue(GetRef());
}

napi_ext_ref NapiJsiRuntime::NapiPointerValue::GetRef() const {
  return reinterpret_cast<napi_ext_ref>(NapiPointerValueView::GetValue());
}

#pragma endregion NapiPointerValue

#pragma region SmallBuffer

template <typename T, size_t InplaceSize>
NapiJsiRuntime::SmallBuffer<T, InplaceSize>::SmallBuffer(size_t size) noexcept
    : m_size{size}, m_heapData{m_size > InplaceSize ? std::make_unique<T[]>(m_size) : nullptr} {}

template <typename T, size_t InplaceSize>
T *NapiJsiRuntime::SmallBuffer<T, InplaceSize>::Data() noexcept {
  return m_heapData ? m_heapData.get() : m_stackData.data();
}

template <typename T, size_t InplaceSize>
size_t NapiJsiRuntime::SmallBuffer<T, InplaceSize>::Size() const noexcept {
  return m_size;
}

#pragma endregion SmallBuffer

#pragma region NapiValueArgs

NapiJsiRuntime::NapiValueArgs::NapiValueArgs(NapiJsiRuntime &runtime, span<const Value> args) : m_args{args.size()} {
  napi_value *jsArgs = m_args.Data();
  for (size_t i = 0; i < args.size(); ++i) {
    jsArgs[i] = runtime.GetNapiValue(args[i]);
  }
}

NapiJsiRuntime::NapiValueArgs::operator span<napi_value>() {
  return span<napi_value>{m_args.Data(), m_args.Size()};
}

#pragma endregion NapiValueArgs

#pragma region JsiValueView

NapiJsiRuntime::JsiValueView::JsiValueView(NapiJsiRuntime *runtime, napi_value jsValue)
    : m_value{InitValue(runtime, jsValue, std::addressof(m_pointerStore))} {}

NapiJsiRuntime::JsiValueView::operator const Value &() const noexcept {
  return m_value;
}

/*static*/ Value NapiJsiRuntime::JsiValueView::InitValue(NapiJsiRuntime *runtime, napi_value value, StoreType *store) {
  switch (runtime->TypeOf(value)) {
    case napi_valuetype::napi_undefined:
      return Value::undefined();
    case napi_valuetype::napi_null:
      return Value::null();
    case napi_valuetype::napi_boolean:
      return Value{runtime->GetValueBool(value)};
    case napi_valuetype::napi_number:
      return Value{runtime->GetValueDouble(value)};
    case napi_valuetype::napi_string:
      return make<String>(new (store) NapiPointerValueView{runtime, value});
    case napi_valuetype::napi_symbol:
      return make<Symbol>(new (store) NapiPointerValueView{runtime, value});
    case napi_valuetype::napi_object:
    case napi_valuetype::napi_function:
    case napi_valuetype::napi_external:
    case napi_valuetype::napi_bigint:
      return make<Object>(new (store) NapiPointerValueView{runtime, value});
    default:
      throw JSINativeException("Unexpected value type");
  }
}

#pragma endregion JsiValueView

#pragma region JsiValueViewArgs

NapiJsiRuntime::JsiValueViewArgs::JsiValueViewArgs(NapiJsiRuntime *runtime, span<napi_value> args) noexcept
    : m_pointerStore{args.size()}, m_args{args.size()} {
  JsiValueView::StoreType *pointerStore = m_pointerStore.Data();
  Value *jsiArgs = m_args.Data();
  for (size_t i = 0; i < m_args.Size(); ++i) {
    jsiArgs[i] = JsiValueView::InitValue(runtime, args[i], std::addressof(pointerStore[i]));
  }
}

Value const *NapiJsiRuntime::JsiValueViewArgs::Data() noexcept {
  return m_args.Data();
}

size_t NapiJsiRuntime::JsiValueViewArgs::Size() const noexcept {
  return m_args.Size();
}

#pragma endregion JsiValueViewArgs

#pragma region PropNameIDView

NapiJsiRuntime::PropNameIDView::PropNameIDView(NapiJsiRuntime *runtime, napi_value propertyId) noexcept
    : m_propertyId{make<PropNameID>(new (std::addressof(m_pointerStore)) NapiPointerValueView{runtime, propertyId})} {}

NapiJsiRuntime::PropNameIDView::operator PropNameID const &() const noexcept {
  return m_propertyId;
}

#pragma endregion PropNameIDView

#pragma region HostFunctionWrapper

NapiJsiRuntime::HostFunctionWrapper::HostFunctionWrapper(HostFunctionType &&type, NapiJsiRuntime &runtime)
    : m_hostFunction{std::move(type)}, m_runtime{runtime} {}

HostFunctionType &NapiJsiRuntime::HostFunctionWrapper::GetHostFunction() noexcept {
  return m_hostFunction;
}

NapiJsiRuntime &NapiJsiRuntime::HostFunctionWrapper::GetRuntime() noexcept {
  return m_runtime;
}

#pragma endregion HostFunctionWrapper

#pragma region NapiPreparedJavaScript

NapiJsiRuntime::NapiPreparedJavaScript::NapiPreparedJavaScript(
    unique_ptr<const Buffer> serializedBuffer,
    const shared_ptr<const Buffer> &sourceBuffer,
    string sourceUrl)
    : m_sourceBuffer{sourceBuffer},
      m_serializedBuffer{std::move(serializedBuffer)},
      m_sourceUrl{std::move(sourceUrl)} {}

const Buffer &NapiJsiRuntime::NapiPreparedJavaScript::SerializedBuffer() const {
  return *m_serializedBuffer;
}

const Buffer &NapiJsiRuntime::NapiPreparedJavaScript::SourceBuffer() const {
  return *m_sourceBuffer;
}

const string &NapiJsiRuntime::NapiPreparedJavaScript::SourceUrl() const {
  return m_sourceUrl;
}

#pragma endregion NapiPreparedJavaScript

#pragma region VectorBuffer

NapiJsiRuntime::VectorBuffer::VectorBuffer(vector<uint8_t> data) : m_data(std::move(data)) {}

const uint8_t *NapiJsiRuntime::VectorBuffer::data() const {
  return m_data.data();
}

size_t NapiJsiRuntime::VectorBuffer::size() const {
  return m_data.size();
}

#pragma endregion VectorBuffer

#pragma region error - handling

// Throws facebook::jsi::JSError or facebook::jsi::JSINativeException from NodeApi error.
[[noreturn]] void NapiJsiRuntime::ThrowJsException(napi_status status) const {
  napi_value jsError{};
  CHECK_NAPI_ELSE_CRASH(napi_get_and_clear_last_exception(m_env, &jsError));

  if (!m_pendingJSError && (status == napi_pending_exception || InstanceOf(jsError, m_value.Error))) {
    AutoRestore<bool> setValue(const_cast<bool *>(&m_pendingJSError), true);
    RewriteErrorMessage(jsError);

    throw JSError(*const_cast<NapiJsiRuntime *>(this), ToJsiValue(jsError));
  } else {
    std::ostringstream errorString;
    errorString << "A call to NodeApi returned error code 0x" << std::hex << status << '.';

    throw JSINativeException(errorString.str().c_str());
  }
}

// Throws facebook::jsi::JSINativeException with a message.
[[noreturn]] void NapiJsiRuntime::ThrowNativeException(char const *errorMessage) const {
  throw JSINativeException(errorMessage);
}

// Rewrites error messages to match the JSI unit test expectations.
void NapiJsiRuntime::RewriteErrorMessage(napi_value jsError) const {
  // The code below must work correctly even if the 'message' getter throws.
  // In case when it throws, we ignore that exception.
  napi_value message{};
  napi_status status = napi_get_property(m_env, jsError, m_propertyId.message, &message);
  if (status != napi_ok) {
    // If the 'message' property getter throws, then we clear the exception and ignore it.
    napi_value ignoreJSError{};
    napi_get_and_clear_last_exception(m_env, &ignoreJSError);
  } else if (TypeOf(message) == napi_string) {
    // JSI unit tests expect V8- or JSC-like messages for the stack overflow.
    if (StringToStdString(message) == "Out of stack space") {
      SetProperty(jsError, m_propertyId.message, CreateStringUtf8("RangeError : Maximum call stack size exceeded"sv));
    }
  }
}

// Evaluates lambda and augments exception messages with the method's name.
template <typename TLambda>
auto NapiJsiRuntime::RunInMethodContext(char const *methodName, TLambda lambda) {
  try {
    return lambda();
  } catch (JSError const &) {
    throw; // do not augment the JSError exceptions.
  } catch (std::exception const &ex) {
    ThrowNativeException((string{"Exception in "} + methodName + ": " + ex.what()).c_str());
  } catch (...) {
    ThrowNativeException((string{"Exception in "} + methodName + ": <unknown>").c_str());
  }
}

// Evaluates lambda and converts all exceptions to NAPI errors.
template <typename TLambda>
napi_value NapiJsiRuntime::HandleCallbackExceptions(TLambda lambda) const noexcept {
  try {
    try {
      return lambda();
    } catch (JSError const &jsError) {
      // This block may throw exceptions
      SetException(GetNapiValue(jsError.value()));
    }
  } catch (std::exception const &ex) {
    SetException(ex.what());
  } catch (...) {
    SetException("Unexpected error");
  }

  return m_value.Undefined;
}

// Throws JavaScript exception using NAPI.
bool NapiJsiRuntime::SetException(napi_value error) const noexcept {
  // This method must not throw. We return false in case of error.
  return napi_throw(m_env, error) == napi_status::napi_ok;
}

// Throws JavaScript error exception with the provided message using NAPI.
bool NapiJsiRuntime::SetException(string_view message) const noexcept {
  // This method must not throw. We return false in case of error.
  return napi_throw_error(m_env, "Unknown", message.data()) == napi_status::napi_ok;
}

#pragma endregion error - handling

#pragma region Shared NAPI wrappers

// Runs script with the sourceUrl origin.
napi_value NapiJsiRuntime::RunScript(napi_value script, const char *sourceUrl) {
  napi_value result{};
  CHECK_NAPI(napi_ext_run_script(m_env, script, sourceUrl, &result));

  return result;
}

// Serializes script with the sourceUrl origin.
vector<uint8_t> NapiJsiRuntime::SerializeScript(napi_value script, const char *sourceUrl) {
  vector<uint8_t> result;
  auto getBuffer = [](napi_env /*env*/, uint8_t const *buffer, size_t bufferLength, void *bufferHint) {
    auto data = reinterpret_cast<vector<uint8_t> *>(bufferHint);
    data->assign(buffer, buffer + bufferLength);
  };
  CHECK_NAPI(napi_ext_serialize_script(m_env, script, sourceUrl, getBuffer, &result));

  return result;
}

// Runs serialized scriopt with the provided source and the sourceUrl origin.
napi_value
NapiJsiRuntime::RunSerializedScript(span<const uint8_t> serialized, napi_value source, const char *sourceUrl) {
  napi_value result{};
  CHECK_NAPI(napi_ext_run_serialized_script(m_env, serialized.data(), serialized.size(), source, sourceUrl, &result));

  return result;
}

// Creates a ref counted reference out of the napi_value.
napi_ext_ref NapiJsiRuntime::CreateReference(napi_value value) const {
  napi_ext_ref result{};
  CHECK_NAPI(napi_ext_create_reference(m_env, value, &result));

  return result;
}

// Decrements the reference ref count. It removes the reference if its ref count becomes 0.
// Do not use the provided ref parameter after calling this method.
void NapiJsiRuntime::ReleaseReference(napi_ext_ref ref) const {
  // TODO: [vmoroz] make it safe to be called from another thread per JSI spec.
  CHECK_NAPI(napi_ext_reference_unref(m_env, ref));
}

// Gets the napi_value from the reference.
napi_value NapiJsiRuntime::GetReferenceValue(napi_ext_ref ref) const {
  napi_value result{};
  CHECK_NAPI(napi_ext_get_reference_value(m_env, ref, &result));

  return result;
}

// Gets type of the napi_value.
napi_valuetype NapiJsiRuntime::TypeOf(napi_value value) const {
  napi_valuetype result{};
  CHECK_NAPI(napi_typeof(m_env, value, &result));

  return result;
}

// Returns true if two napi_values are strict equal per JavaScript rules.
bool NapiJsiRuntime::StrictEquals(napi_value left, napi_value right) const {
  bool result{false};
  CHECK_NAPI(napi_strict_equals(m_env, left, right, &result));

  return result;
}

// Gets the napi_value for the JavaScript's undefined value.
napi_value NapiJsiRuntime::GetUndefined() const {
  napi_value result{nullptr};
  CHECK_NAPI(napi_get_undefined(m_env, &result));

  return result;
}

// Gets the napi_value for the JavaScript's null value.
napi_value NapiJsiRuntime::GetNull() const {
  napi_value result{nullptr};
  CHECK_NAPI(napi_get_null(m_env, &result));

  return result;
}

// Gets the napi_value for the JavaScript's global object.
napi_value NapiJsiRuntime::GetGlobal() const {
  napi_value result{nullptr};
  CHECK_NAPI(napi_get_global(m_env, &result));

  return result;
}

// Gets the napi_value for the JavaScript's true and false values.
napi_value NapiJsiRuntime::GetBoolean(bool value) const {
  napi_value result{nullptr};
  CHECK_NAPI(napi_get_boolean(m_env, value, &result));

  return result;
}

// Gets value of the Boolean napi_value.
bool NapiJsiRuntime::GetValueBool(napi_value value) const {
  bool result{nullptr};
  CHECK_NAPI(napi_get_value_bool(m_env, value, &result));

  return result;
}

// Creates napi_value with an int32_t value.
napi_value NapiJsiRuntime::CreateInt32(int32_t value) const {
  napi_value result{};
  CHECK_NAPI(napi_create_int32(m_env, value, &result));

  return result;
}

// Creates napi_value with a double value.
napi_value NapiJsiRuntime::CreateDouble(double value) const {
  napi_value result{};
  CHECK_NAPI(napi_create_double(m_env, value, &result));

  return result;
}

// Gets value of the Double napi_value.
double NapiJsiRuntime::GetValueDouble(napi_value value) const {
  double result{0};
  CHECK_NAPI(napi_get_value_double(m_env, value, &result));

  return result;
}

// Creates a napi_value string from the extended ASCII symbols that correspond to the Latin1 encoding.
// Each character is a byte-sized value from 0 to 255.
napi_value NapiJsiRuntime::CreateStringLatin1(string_view value) const {
  CHECK_ELSE_THROW(value.data(), "Cannot convert a nullptr to a JS string.");
  napi_value result{};
  CHECK_NAPI(napi_create_string_latin1(m_env, value.data(), value.size(), &result));

  return result;
}

// Creates a napi_value string from a UTF-8 string.
napi_value NapiJsiRuntime::CreateStringUtf8(string_view value) const {
  CHECK_ELSE_THROW(value.data(), "Cannot convert a nullptr to a JS string.");
  napi_value result{};
  CHECK_NAPI(napi_create_string_utf8(m_env, value.data(), value.size(), &result));

  return result;
}

// Creates a napi_value string from a UTF-8 string. Use data and length instead of string_view.
napi_value NapiJsiRuntime::CreateStringUtf8(const uint8_t *data, size_t length) const {
  return CreateStringUtf8({reinterpret_cast<const char *>(data), length});
}

// Gets std::string from the napi_value string.
string NapiJsiRuntime::StringToStdString(napi_value stringValue) const {
  string result;
  CHECK_ELSE_THROW(
      TypeOf(stringValue) == napi_valuetype::napi_string,
      "Cannot convert a non JS string NodeApi Value to a std::string.");
  size_t strLength{};
  CHECK_NAPI(napi_get_value_string_utf8(m_env, stringValue, nullptr, 0, &strLength));
  result.assign(strLength, '\0');
  size_t copiedLength{};
  CHECK_NAPI(napi_get_value_string_utf8(m_env, stringValue, &result[0], result.length() + 1, &copiedLength));
  CHECK_ELSE_THROW(result.length() == copiedLength, "Unexpected string length");

  return result;
}

// Gets or creates a unique string value from an UTF-8 string_view.
napi_ext_ref NapiJsiRuntime::GetPropertyIdFromName(string_view value) const {
  napi_ext_ref ref{};
  CHECK_NAPI(napi_ext_get_unique_string_utf8_ref(m_env, value.data(), value.size(), &ref));

  return ref;
}

// Gets or creates a unique string value from an UTF-8 data/length range.
napi_ext_ref NapiJsiRuntime::GetPropertyIdFromName(const uint8_t *data, size_t length) const {
  return GetPropertyIdFromName({reinterpret_cast<const char *>(data), length});
}

// Gets or creates a unique string value from napi_value string.
napi_ext_ref NapiJsiRuntime::GetPropertyIdFromName(napi_value str) const {
  napi_ext_ref ref{};
  CHECK_NAPI(napi_ext_get_unique_string_ref(m_env, str, &ref));

  return ref;
}

// Converts property id value to std::string.
string NapiJsiRuntime::PropertyIdToStdString(napi_value propertyId) {
  if (TypeOf(propertyId) == napi_symbol) {
    return SymbolToStdString(propertyId);
  }

  return StringToStdString(propertyId);
}

// Creates a JavaScript symbol napi_value.
napi_value NapiJsiRuntime::CreateSymbol(string_view symbolDescription) const {
  napi_value result{};
  napi_value description = CreateStringUtf8(symbolDescription);
  CHECK_NAPI(napi_create_symbol(m_env, description, &result));

  return result;
}

// Calls Symbol.toString() and returns it as std::string.
string NapiJsiRuntime::SymbolToStdString(napi_value symbolValue) {
  EnvScope scope{m_env};
  if (!m_value.SymbolToString) {
    napi_value symbolCtor = GetProperty(m_value.Global, m_propertyId.Symbol);
    napi_value symbolPrototype = GetProperty(symbolCtor, m_propertyId.prototype);
    m_value.SymbolToString = NapiRefHolder{this, GetProperty(symbolPrototype, m_propertyId.toString)};
  }
  napi_value jsString = CallFunction(symbolValue, m_value.SymbolToString, {});

  return StringToStdString(jsString);
}

// Calls a JavaScript function.
napi_value NapiJsiRuntime::CallFunction(napi_value thisArg, napi_value function, span<napi_value> args) const {
  napi_value result{};
  CHECK_NAPI(napi_call_function(m_env, thisArg, function, args.size(), args.begin(), &result));

  return result;
}

// Constructs a new JavaScript Object using a constructor function.
napi_value NapiJsiRuntime::ConstructObject(napi_value constructor, span<napi_value> args) const {
  napi_value result{};
  CHECK_NAPI(napi_new_instance(m_env, constructor, args.size(), args.begin(), &result));

  return result;
}

// Returns true if object was constructed using the provided constructor.
bool NapiJsiRuntime::InstanceOf(napi_value object, napi_value constructor) const {
  bool result{false};
  CHECK_NAPI(napi_instanceof(m_env, object, constructor, &result));

  return result;
}

// Creates new JavaScript Object.
napi_value NapiJsiRuntime::CreateObject() const {
  napi_value result{};
  CHECK_NAPI(napi_create_object(m_env, &result));

  return result;
}

// Returns true if the object has a property with the provided property ID.
bool NapiJsiRuntime::HasProperty(napi_value object, napi_value propertyId) const {
  bool result{};
  CHECK_NAPI(napi_has_property(m_env, object, propertyId, &result));

  return result;
}

// Gets object property value.
napi_value NapiJsiRuntime::GetProperty(napi_value object, napi_value propertyId) const {
  napi_value result{};
  CHECK_NAPI(napi_get_property(m_env, object, propertyId, &result));

  return result;
}

// Sets object property value.
void NapiJsiRuntime::SetProperty(napi_value object, napi_value propertyId, napi_value value) const {
  CHECK_NAPI(napi_set_property(m_env, object, propertyId, value));
}

// Sets object property value with the provided property accessibility attributes.
void NapiJsiRuntime::SetProperty(
    napi_value object,
    napi_value propertyId,
    napi_value value,
    napi_property_attributes attrs) const {
  napi_property_descriptor descriptor{};
  descriptor.name = propertyId;
  descriptor.value = value;
  descriptor.attributes = attrs;
  CHECK_NAPI(napi_define_properties(m_env, object, 1, &descriptor));
}

// Creates a new JavaScript Array with the provided length.
napi_value NapiJsiRuntime::CreateArray(size_t length) const {
  napi_value result{};
  CHECK_NAPI(napi_create_array_with_length(m_env, length, &result));

  return result;
}

// Sets array element.
void NapiJsiRuntime::SetElement(napi_value array, uint32_t index, napi_value value) const {
  CHECK_NAPI(napi_set_element(m_env, array, index, value));
}

// The NAPI external function callback used for the JSI host function implementation.
/*static*/ napi_value NapiJsiRuntime::JsiHostFunctionCallback(napi_env env, napi_callback_info info) noexcept {
  HostFunctionWrapper *hostFuncWrapper{};
  size_t argc{};
  CHECK_NAPI_ELSE_CRASH(
      napi_get_cb_info(env, info, &argc, nullptr, nullptr, reinterpret_cast<void **>(&hostFuncWrapper)));
  CHECK_ELSE_CRASH(hostFuncWrapper, "Cannot find the host function");
  NapiJsiRuntime &runtime = hostFuncWrapper->GetRuntime();

  return runtime.HandleCallbackExceptions([&env, &info, &argc, &runtime, &hostFuncWrapper]() {
    SmallBuffer<napi_value, MaxStackArgCount> napiArgs(argc);
    napi_value thisArg{};
    CHECK_NAPI_ELSE_CRASH(napi_get_cb_info(env, info, &argc, napiArgs.Data(), &thisArg, nullptr));
    CHECK_ELSE_CRASH(napiArgs.Size() == argc, "Wrong argument count");
    const JsiValueView jsiThisArg{&runtime, thisArg};
    JsiValueViewArgs jsiArgs(&runtime, span<napi_value>(napiArgs.Data(), napiArgs.Size()));

    const HostFunctionType &hostFunc = hostFuncWrapper->GetHostFunction();
    return runtime.RunInMethodContext("HostFunction", [&hostFunc, &runtime, &jsiThisArg, &jsiArgs]() {
      return runtime.GetNapiValue(hostFunc(runtime, jsiThisArg, jsiArgs.Data(), jsiArgs.Size()));
    });
  });
}

// Creates an external function.
napi_value NapiJsiRuntime::CreateExternalFunction(
    napi_value name,
    int32_t paramCount,
    napi_callback callback,
    void *callbackData) {
  string funcName = StringToStdString(name);
  napi_value function{};
  CHECK_NAPI(napi_create_function(m_env, funcName.data(), funcName.length(), callback, callbackData, &function));
  SetProperty(function, m_propertyId.length, CreateInt32(paramCount), napi_property_attributes::napi_default);

  return function;
}

// Creates an object that wraps up external data.
napi_value NapiJsiRuntime::CreateExternalObject(void *data, napi_finalize finalizeCallback) const {
  napi_value result{};
  CHECK_NAPI(napi_create_external(m_env, data, finalizeCallback, nullptr, &result));

  return result;
}

// Wraps up std::unique_ptr as an external object.
template <typename T>
napi_value NapiJsiRuntime::CreateExternalObject(unique_ptr<T> &&data) const {
  napi_value object =
      CreateExternalObject(data.get(), [](napi_env /*env*/, void *dataToDestroy, void * /*finalizerHint*/) {
        // We wrap dataToDestroy in a unique_ptr to avoid calling delete explicitly.
        if (std::is_array<T>::value) {
          delete[] static_cast<T *>(dataToDestroy);
        } else {
          delete static_cast<T *>(dataToDestroy);
        }
      });

  // We only call data.release() after the CreateExternalObject succeeds.
  // Otherwise, when CreateExternalObject fails and an exception is thrown,
  // the memory that data used to own will be leaked.
  data.release();

  return object;
}

// Gets external data wrapped by an external object.
void *NapiJsiRuntime::GetExternalData(napi_value object) const {
  void *result{};
  CHECK_NAPI(napi_get_value_external(m_env, object, &result));

  return result;
}

// Gets JSI host object wrapped into a napi_value object.
const shared_ptr<HostObject> &NapiJsiRuntime::GetJsiHostObject(napi_value obj) {
  const napi_value hostObjectHolder = GetProperty(obj, m_propertyId.hostObjectSymbol);

  if (TypeOf(hostObjectHolder) == napi_valuetype::napi_external) {
    if (void *data = GetExternalData(hostObjectHolder)) {
      return *static_cast<shared_ptr<HostObject> *>(data);
    }
  }

  throw JSINativeException("Cannot get HostObjects.");
}

// Gets cached or creates Proxy handler to implement the JSI host object.
napi_value NapiJsiRuntime::GetHostObjectProxyHandler() {
  if (!m_value.HostObjectProxyHandler) {
    const napi_value handler = CreateObject();
    SetProxyTrap<&NapiJsiRuntime::HostObjectGetTrap, 3>(handler, m_propertyId.get);
    SetProxyTrap<&NapiJsiRuntime::HostObjectSetTrap, 4>(handler, m_propertyId.set);
    SetProxyTrap<&NapiJsiRuntime::HostObjectOwnKeysTrap, 1>(handler, m_propertyId.ownKeys);
    SetProxyTrap<&NapiJsiRuntime::HostObjectGetOwnPropertyDescriptorTrap, 2>(
        handler, m_propertyId.getOwnPropertyDescriptor);
    m_value.HostObjectProxyHandler = NapiRefHolder{this, handler};
  }

  return m_value.HostObjectProxyHandler;
}

// Sets Proxy trap method as a pointer to NapiJsiRuntime instance method.
template <napi_value (NapiJsiRuntime::*trapMethod)(span<napi_value>), size_t argCount>
void NapiJsiRuntime::SetProxyTrap(napi_value handler, napi_value propertyName) {
  auto proxyTrap = [](napi_env env, napi_callback_info info) noexcept {
    NapiJsiRuntime *runtime{};
    napi_value args[argCount]{};
    size_t actualArgCount{argCount};
    CHECK_NAPI_ELSE_CRASH(
        napi_get_cb_info(env, info, &actualArgCount, args, nullptr, reinterpret_cast<void **>(&runtime)));
    CHECK_ELSE_CRASH(actualArgCount == argCount, "proxy trap requires argCount arguments.");

    return runtime->HandleCallbackExceptions(
        [&runtime, &args]() { return (runtime->*trapMethod)(span<napi_value>(args, argCount)); });
  };

  SetProperty(handler, propertyName, CreateExternalFunction(propertyName, argCount, proxyTrap, this));
}

// The host object Proxy 'get' trap implementation.
napi_value NapiJsiRuntime::HostObjectGetTrap(span<napi_value> args) {
  // args[0] - the Proxy target object.
  // args[1] - the name of the property to set.
  // args[2] - the Proxy object (unused).
  napi_value propertyName = args[1];
  if (TypeOf(propertyName) == napi_symbol && StrictEquals(propertyName, m_propertyId.hostObjectSymbol)) {
    // The special property to retrieve the target object.
    return GetProperty(args[0], m_propertyId.hostObjectSymbol);
  }
  const auto &hostObject = GetJsiHostObject(args[0]);
  PropNameIDView propertyId{this, propertyName};

  return RunInMethodContext("HostObject::get", [&hostObject, &propertyId, this]() {
    return GetNapiValue(hostObject->get(*this, propertyId));
  });
}

// The host object Proxy 'set' trap implementation.
napi_value NapiJsiRuntime::HostObjectSetTrap(span<napi_value> args) {
  // args[0] - the Proxy target object.
  // args[1] - the name of the property to set.
  // args[2] - the new value of the property to set.
  // args[3] - the Proxy object (unused).
  const auto &hostObject = GetJsiHostObject(args[0]);
  PropNameIDView propertyId{this, args[1]};
  JsiValueView value{this, args[2]};
  RunInMethodContext(
      "HostObject::set", [&hostObject, &propertyId, &value, this]() { hostObject->set(*this, propertyId, value); });

  return static_cast<napi_value>(m_value.Undefined);
}

// The host object Proxy 'ownKeys' trap implementation.
napi_value NapiJsiRuntime::HostObjectOwnKeysTrap(span<napi_value> args) {
  // args[0] - the Proxy target object.
  const auto &hostObject = GetJsiHostObject(args[0]);

  vector<PropNameID> ownKeys = RunInMethodContext(
      "HostObject::getPropertyNames", [&hostObject, this]() { return hostObject->getPropertyNames(*this); });

  std::unordered_set<napi_ext_ref> dedupedOwnKeys{};
  dedupedOwnKeys.reserve(ownKeys.size());
  for (PropNameID const &key : ownKeys) {
    dedupedOwnKeys.insert(GetNapiRef(key));
  }

  napi_value ownKeyArray = CreateArray(dedupedOwnKeys.size());
  uint32_t index = 0;
  for (napi_ext_ref key : dedupedOwnKeys) {
    SetElement(ownKeyArray, index, GetReferenceValue(key));
    ++index;
  }

  return ownKeyArray;
}

// The host object Proxy 'getOwnPropertyDescriptor' trap implementation.
napi_value NapiJsiRuntime::HostObjectGetOwnPropertyDescriptorTrap(span<napi_value> args) {
  // args[0] - the Proxy target object.
  // args[1] - the property
  const auto &hostObject = GetJsiHostObject(args[0]);
  PropNameIDView propertyId{this, args[1]};

  return RunInMethodContext("HostObject::getOwnPropertyDescriptor", [&hostObject, &propertyId, this]() {
    auto getPropDescriptor = [](napi_value name, napi_value value) {
      return napi_property_descriptor{
          nullptr, name, nullptr, nullptr, nullptr, value, napi_default_jsproperty, nullptr};
    };
    napi_property_descriptor properties[]{
        getPropDescriptor(m_propertyId.value, GetNapiValue(hostObject->get(*this, propertyId))),
        getPropDescriptor(m_propertyId.writable, m_value.True),
        getPropDescriptor(m_propertyId.enumerable, m_value.True),
        getPropDescriptor(m_propertyId.configurable, m_value.True)};
    napi_value descriptor = CreateObject();
    CHECK_NAPI(napi_define_properties(m_env, descriptor, std::size(properties), properties));

    return descriptor;
  });
}

#pragma endregion Shared NAPI wrappers

#pragma region Miscellaneous utility methods

// Converts facebook::jsi::Bufer to span.
span<const uint8_t> NapiJsiRuntime::ToSpan(const Buffer &buffer) {
  return span<const uint8_t>(buffer.data(), buffer.size());
}

// Creates facebook::jsi::Value from napi_value.
Value NapiJsiRuntime::ToJsiValue(napi_value value) const {
  switch (TypeOf(value)) {
    case napi_valuetype::napi_undefined:
      return Value::undefined();
    case napi_valuetype::napi_null:
      return Value::null();
    case napi_valuetype::napi_boolean:
      return Value{GetValueBool(value)};
    case napi_valuetype::napi_number:
      return Value{GetValueDouble(value)};
    case napi_valuetype::napi_string:
      return Value{MakePointer<String>(value)};
    case napi_valuetype::napi_symbol:
      return Value{MakePointer<Symbol>(value)};
    case napi_valuetype::napi_object:
    case napi_valuetype::napi_function:
    case napi_valuetype::napi_external:
    case napi_valuetype::napi_bigint:
      return Value{MakePointer<Object>(value)};
    default:
      throw JSINativeException("Unexpected value type");
  }
}

// Gets napi_value from facebook::jsi::Value.
napi_value NapiJsiRuntime::GetNapiValue(const Value &value) const {
  if (value.isUndefined()) {
    return m_value.Undefined;
  } else if (value.isNull()) {
    return m_value.Null;
  } else if (value.isBool()) {
    return value.getBool() ? m_value.True : m_value.False;
  } else if (value.isNumber()) {
    return CreateDouble(value.getNumber());
  } else if (value.isSymbol()) {
    return GetNapiValue(value.getSymbol(*const_cast<NapiJsiRuntime *>(this)));
  } else if (value.isString()) {
    return GetNapiValue(value.getString(*const_cast<NapiJsiRuntime *>(this)));
  } else if (value.isObject()) {
    return GetNapiValue(value.getObject(*const_cast<NapiJsiRuntime *>(this)));
  } else {
    throw JSINativeException("Unexpected jsi::Value type");
  }
}

// Clones NapiPointerValue.
/*static*/ NapiJsiRuntime::NapiPointerValue *NapiJsiRuntime::CloneNapiPointerValue(const PointerValue *pointerValue) {
  auto napiPointerValue = static_cast<const NapiPointerValueView *>(pointerValue);

  return new NapiPointerValue(napiPointerValue->GetRuntime(), napiPointerValue->GetValue());
}

// Gets napi_value from facebook::jsi::Pointer.
/*static*/ napi_value NapiJsiRuntime::GetNapiValue(const Pointer &p) {
  return static_cast<const NapiPointerValueView *>(getPointerValue(p))->GetValue();
}

// Gets napi_ext_ref from facebook::jsi::Pointer.
/*static*/ napi_ext_ref NapiJsiRuntime::GetNapiRef(const Pointer &p) {
  return static_cast<const NapiPointerValueView *>(getPointerValue(p))->GetRef();
}

// Makes new value derived from the facebook::jsi::Pointer type.
template <typename T, typename TValue, std::enable_if_t<std::is_base_of_v<facebook::jsi::Pointer, T>, int>>
T NapiJsiRuntime::MakePointer(TValue value) const {
  return make<T>(new NapiPointerValue(this, value));
}

#pragma endregion Miscellaneous utility methods

#pragma endregion NapiJsiRuntime
} // namespace

unique_ptr<Runtime> __cdecl MakeNodeApiJsiRuntime(napi_env env) noexcept {
  return std::make_unique<NapiJsiRuntime>(env);
}

} // namespace Microsoft::JSI
