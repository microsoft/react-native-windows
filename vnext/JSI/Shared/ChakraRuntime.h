// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "ChakraRuntimeArgs.h"
#include "jsi/jsi.h"

#ifdef CHAKRACORE
#include "ChakraCore.h"
#else
#ifndef USE_EDGEMODE_JSRT
#define USE_EDGEMODE_JSRT
#endif
#include "jsrt.h"
#endif

#include <cstdint>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace facebook::jsi::chakra {

// An shared_ptr like RAII Wrapper for JsRefs, which are references to objects
// owned by the garbage collector. These include JsContextRef, JsValueRef, and
// JsPropertyIdRef, etc. ChakraObjectRef ensures that JsAddRef and JsRelease
// are called upon initialization and invalidation, respectively. It also allows
// users to implicitly convert it into a JsRef. A ChakraObjectRef must only be
// initialized once and invalidated once.
class ChakraObjectRef {
 public:
  ChakraObjectRef() noexcept;
  explicit ChakraObjectRef(JsRef ref);

  ChakraObjectRef(const ChakraObjectRef &other) noexcept;
  ChakraObjectRef(ChakraObjectRef &&other) noexcept;

  ChakraObjectRef &operator=(const ChakraObjectRef &rhs) noexcept;
  ChakraObjectRef &operator=(ChakraObjectRef &&rhs) noexcept;

  ~ChakraObjectRef() noexcept;

  void initialize(JsRef ref);
  void invalidate();

  inline operator JsRef() const noexcept {
    return m_ref;
  }

 private:
  enum class State { Uninitialized, Initialized, Invalidated };

  JsRef m_ref;
  State m_state;
};

// TODO (yicyao):
// Currently we assume that a ChakraRuntime will be created and exclusively used
// on a single thread. We ought to explore whether we can make ChakraRuntime
// thread safe.
class ChakraRuntime : public Runtime {
 public:
  ChakraRuntime(ChakraRuntimeArgs &&args) noexcept;
  ~ChakraRuntime() noexcept;

  //============================================================================
  // Below are functions inherited from Runtime.
  // Note that the performance characteristics detailed by the comments in jsi.h
  // is for HermesRuntime and might not apply to ChakraRuntime.
  // TODO (yicyao): Do some performance measurements for ChakraRuntime.

  // Buffer must contain UTF-8 encoded JavaScript source code.
  Value evaluateJavaScript(
      const std::shared_ptr<const Buffer> &buffer,
      const std::string &sourceURL) override;

  std::shared_ptr<const PreparedJavaScript> prepareJavaScript(
      const std::shared_ptr<const Buffer> &buffer,
      std::string sourceURL) override;

  Value evaluatePreparedJavaScript(
      const std::shared_ptr<const PreparedJavaScript> &js) override;

  Object global() override;

  std::string description() override;

  bool isInspectable() override;

  // We use the default instrumentation() implementation that returns an
  // Instrumentation instance which returns no metrics.

 protected:
  // Despite the name "clone" suggesting a deep copy, a return value of these
  // functions points to a new heap allocated ChakraPointerValue whose memeber
  // ChakraObjectRef refers to the same JavaScript object as the member
  // ChakraObjectRef of *pv. This behavior is consistent with that of
  // HermesRuntime and JSCRuntime. Also, Like all ChakraPointerValues, the
  // return value must only be used as an argument to the constructor of
  // jsi::Pointer or one of its derived classes.
  PointerValue *cloneSymbol(const PointerValue *pv) override;
  PointerValue *cloneString(const PointerValue *pv) override;
  PointerValue *cloneObject(const PointerValue *pv) override;
  PointerValue *clonePropNameID(const PointerValue *pv) override;

  PropNameID createPropNameIDFromAscii(const char *str, size_t length) override;
  PropNameID createPropNameIDFromUtf8(const uint8_t *utf8, size_t length)
      override;
  PropNameID createPropNameIDFromString(const String &str) override;
  std::string utf8(const PropNameID &str) override;
  bool compare(const PropNameID &lhs, const PropNameID &rhs) override;

  std::string symbolToString(const Symbol &s) override;

  String createStringFromAscii(const char *str, size_t length) override;
  String createStringFromUtf8(const uint8_t *utf8, size_t length) override;
  std::string utf8(const String &str) override;

  Object createObject() override;
  Object createObject(std::shared_ptr<HostObject> ho) override;
  std::shared_ptr<HostObject> getHostObject(const Object &) override;
  HostFunctionType &getHostFunction(const Function &) override;

  Value getProperty(const Object &obj, const PropNameID &name) override;
  Value getProperty(const Object &obj, const String &name) override;
  bool hasProperty(const Object &obj, const PropNameID &name) override;
  bool hasProperty(const Object &obj, const String &name) override;
  void setPropertyValue(Object &obj, const PropNameID &name, const Value &value)
      override;
  void setPropertyValue(Object &obj, const String &name, const Value &value)
      override;

  bool isArray(const Object &obj) const override;
  bool isArrayBuffer(const Object &obj) const override;
  bool isFunction(const Object &obj) const override;
  bool isHostObject(const Object &obj) const override;
  bool isHostFunction(const Function &func) const override;
  // Returns the names of all enumerable properties of an object. This
  // corresponds the properties iterated through by the JavaScript for..in loop.
  Array getPropertyNames(const Object &obj) override;

  WeakObject createWeakObject(const Object &obj) override;
  Value lockWeakObject(const WeakObject &weakObj) override;

  Array createArray(size_t length) override;
  size_t size(const Array &arr) override;
  size_t size(const ArrayBuffer &arrBuf) override;
  // The lifetime of the buffer returned is the same as the lifetime of the
  // ArrayBuffer. The returned buffer pointer does not count as a reference to
  // the ArrayBuffer for the purpose of garbage collection.
  uint8_t *data(const ArrayBuffer &arrBuf) override;
  Value getValueAtIndex(const Array &arr, size_t index) override;
  void setValueAtIndexImpl(Array &arr, size_t index, const Value &value)
      override;

  Function createFunctionFromHostFunction(
      const PropNameID &name,
      unsigned int paramCount,
      HostFunctionType func) override;
  Value call(
      const Function &func,
      const Value &jsThis,
      const Value *args,
      size_t count) override;
  Value callAsConstructor(const Function &func, const Value *args, size_t count)
      override;

  // For now, pushing a scope does nothing, and popping a scope forces the
  // JavaScript garbage collector to run.
  ScopeState *pushScope() override;
  void popScope(ScopeState *) override;

  bool strictEquals(const Symbol &a, const Symbol &b) const override;
  bool strictEquals(const String &a, const String &b) const override;
  bool strictEquals(const Object &a, const Object &b) const override;

  bool instanceOf(const Object &o, const Function &f) override;

  // Above are functions inherited from Runtime.
  //============================================================================

  // This function may need to construct a JsError, and since JsError's
  // constructor takes in a Runtime&, not a const Runtime&, this function cannot
  // be const.
  void throwUponJsError(
      JsErrorCode error,
      const char *const chakraApiName = nullptr);

  // These three functions only performs shallow copies.
  ChakraObjectRef toChakraObjectRef(const Value &value);
  std::vector<ChakraObjectRef> toChakraObjectRefs(
      const Value *value,
      size_t count);
  Value toJsiValue(ChakraObjectRef &&ref);

  // ChakraPointerValue is needed for working with Facebook's jsi::Pointer class
  // and must only be used for this purpose. Every instance of
  // ChakraPointerValue should be allocated on the heap and be used as an
  // argument to the constructor of jsi::Pointer or one of its derived classes.
  // Pointer makes sure that invalidate(), which frees the heap allocated
  // ChakraPointerValue, is called upon destruction. Since the constructor of
  // jsi::Pointer is protected, we usually have to invoke it through
  // jsi::Runtime::make. The code should look something like:
  //
  //     make<Pointer>(new ChakraPointerValue(...));//
  //
  template <typename T>
  struct ChakraPointerValueTemplate : PointerValue {
   public:
    ChakraPointerValueTemplate(const T &ref) noexcept : m_ref{ref} {
      static_assert(
          std::is_same<T, ChakraObjectRef>::value ||
              // Since only ChakraCore offers the JsWeakRef type alias, we
              // cannot use it here; so void* is the best alternative we can use
              // here.
              std::is_same<T, void *>::value,
          "ChakraPointerValueTemplate should only be instantiated for "
          "ChakraObjectRef and JsWeakRef.");
    }

    ChakraPointerValueTemplate(T &&ref) noexcept : m_ref{std::move(ref)} {}

    // Declaring ~ChakraPointerValueTemplate() private prevents the compiler
    // from implicitly generating the following functions, so we have to tell
    // the compiler to do so.
    ChakraPointerValueTemplate(
        const ChakraPointerValueTemplate &other) noexcept = default;
    ChakraPointerValueTemplate(ChakraPointerValueTemplate &&other) noexcept =
        default;
    ChakraPointerValueTemplate &operator=(
        const ChakraPointerValueTemplate &rhs) noexcept = default;
    ChakraPointerValueTemplate &operator=(
        ChakraPointerValueTemplate &&rhs) noexcept = default;

    inline void invalidate() noexcept override {
      delete this;
    }

    inline const T &getRef() const noexcept {
      return m_ref;
    }

   private:
    // ~ChakraPointerValueTemplate() should only be invoked by invalidate().
    // Hence we make it private.
    ~ChakraPointerValueTemplate() noexcept = default;

    T m_ref;
  };

  using ChakraPointerValue = ChakraPointerValueTemplate<ChakraObjectRef>;

  // The pointer passed to this function must point to a ChakraPointerValue.
  inline static ChakraPointerValue *cloneChakraPointerValue(
      const PointerValue *pv) {
    return new ChakraPointerValue(
        *(static_cast<const ChakraPointerValue *>(pv)));
  }

  // The jsi::Pointer passed to this function must hold a ChakraPointerValue.
  inline static const ChakraObjectRef &getChakraObjectRef(const Pointer &p) {
    return static_cast<const ChakraPointerValue *>(getPointerValue(p))
        ->getRef();
  }

  //============================================================================
  // These functions are helpers for working with Chakra's APIs. Some of these
  // functions are virtual to allow ChakraCoreRuntime to override them and take
  // advantage of ChakraCore specific APIs.

  // We must mark these two functions as const, as they are used in other const
  // member functions such as isArray and strictEquals. Hence we cannot use
  // throwUponJsError when implementing these functions.
  JsValueType getValueType(const ChakraObjectRef &value) const;
  bool areStrictlyEqual(const ChakraObjectRef &lhs, const ChakraObjectRef &rhs)
      const;

  // The returned wstring is UTF-16 encoded. This function copies the JS string
  // buffer into the returned std::wstring.
  std::wstring toStdWstring(const ChakraObjectRef jsString);

  // Returns a ChakraObjectRef managing a JavaScript string from a UTF-8 encoded
  // cstring. utf8 is copied to JS engine owned memory.
  virtual ChakraObjectRef toJsString(const char *utf8, size_t length);

  // Returns a ChakraObjectRef managing the return value of the JavaScript
  // .toString function.
  ChakraObjectRef toJsString(const ChakraObjectRef &ref);

  // This function should only be used on JsPropertyIdRefs of type
  // JsPropertyIdTypeString. The returned buffer is valid as long as the runtime
  // is alive and cannot be used once the runtime has been disposed.
  const wchar_t *getPropertyName(JsPropertyIdRef id);

  // This function should only be used on PropertyIdRefs of type
  // JsPropertyIdTypeSymbol.
  ChakraObjectRef getPropertySymbol(JsPropertyIdRef id);

  ChakraObjectRef getPropertyId(const wchar_t *const name);
  JsPropertyIdType getPropertyIdType(JsPropertyIdRef id);
  bool comparePropertyIds(JsPropertyIdRef lhs, JsPropertyIdRef rhs);

  //============================================================================

 private:
  Value evaluateJavaScriptImpl(
      const wchar_t *scriptUtf16,
      const wchar_t *urlUtf16);

  ChakraRuntimeArgs m_args;
  JsRuntimeHandle m_runtime;
  ChakraObjectRef m_context;

}; // class ChakraRuntime

} // namespace facebook::jsi::chakra
