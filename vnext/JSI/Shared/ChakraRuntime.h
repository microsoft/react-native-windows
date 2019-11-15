// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "ChakraObjectRef.h"
#include "ScriptStore.h"

#include "jsi/jsi.h"

#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace Microsoft::JSI {

struct ChakraRuntimeArgs;

/**
 * @brief Functionailties of jsi::Runtime that can be implemented by APIs
 * available to both Chakra and ChakraCore.
 *
 * @remarks Since Chakra and ChakraCore has similar but slightly differnt APIs,
 * we use a class hierarchy to enable code reuse. ChakraRuntime contains the
 * functionailties of jsi::Runtime that can be implemented by APIs available to
 * both Chakra and ChakraCore. Functionalities that need to or should be
 * implemented using Chakra or ChakraCore specific APIs are implemented by
 * ChakraRtRuntime and ChakraCoreRuntime, respectively.
 */
class ChakraRuntime : public facebook::jsi::Runtime {
 public:
  ChakraRuntime(std::shared_ptr<ChakraRuntimeArgs> &&args) noexcept;

  /**
   * @remarks We need to load a small bootstraping bundle through
   * EvaluateJavaScriptSimple during initialization to implement HostObjects.
   * Since EvaluateJavaScriptSimple is virtual we cannot use it within the
   * constructor itself. Hence we have a separate Initialize() method.
   */
  void Initialize();

  ~ChakraRuntime() noexcept;

#pragma region Functions_inherited_from_Runtime

  /**
   * @param buffer UTF-8 encoded JavaScript source code.
   *
   * @remarks This function will try to use cached bytecode if it exists.
   */
  facebook::jsi::Value evaluateJavaScript(
      const std::shared_ptr<const facebook::jsi::Buffer> &buffer,
      const std::string &sourceUrl) override;

  /**
   * @returns A std::shared_ptr<const facebook::jsi::PreparedJavaScript>
   * pointing to a ChakraPreparedJavaScript.
   *
   * @remarks The return value might reference JavaScript objects. Hence it must
   * only be used while the ChakraRuntime that created it is still alive.
   */
  std::shared_ptr<const facebook::jsi::PreparedJavaScript> prepareJavaScript(
      const std::shared_ptr<const facebook::jsi::Buffer> &buffer,
      std::string sourceUrl) override;

  facebook::jsi::Object global() override;

  bool isInspectable() override;

  /**
   * Note: We use the default instrumentation() implementation that returns an
   * Instrumentation instance which returns no metrics.
   */

 protected:
  /**
   * @remarks Despite the name "clone" suggesting a deep copy, a return value of
   * thess functions points to a new heap allocated ChakraPointerValue whose
   * memeber ChakraObjectRef refers to the same JavaScript object as the member
   * ChakraObjectRef of *pointerValue. This behavior is consistent with that of
   * HermesRuntime and JSCRuntime. Also, Like all ChakraPointerValues, the
   * return value must only be used as an argument to the constructor of
   * jsi::Pointer or one of its derived classes.
   */
  /**@{*/
  PointerValue *cloneSymbol(const PointerValue *pointerValue) override;
  PointerValue *cloneString(const PointerValue *pointerValue) override;
  PointerValue *cloneObject(const PointerValue *pointerValue) override;
  PointerValue *clonePropNameID(const PointerValue *pointerValue) override;
  /**@}*/

  facebook::jsi::PropNameID createPropNameIDFromAscii(const char *str, size_t length) override;
  facebook::jsi::PropNameID createPropNameIDFromUtf8(const uint8_t *utf8, size_t length) override;
  facebook::jsi::PropNameID createPropNameIDFromString(const facebook::jsi::String &str) override;
  std::string utf8(const facebook::jsi::PropNameID &id) override;
  bool compare(const facebook::jsi::PropNameID &lhs, const facebook::jsi::PropNameID &rhs) override;

  std::string symbolToString(const facebook::jsi::Symbol &s) override;

  /**
   * @remarks Despite its name, createPropNameIDFromAscii is the same function
   * as createStringFromUtf8.
   */
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

  /**
   * @returns The names of all enumerable properties of an object. This
   * corresponds the properties iterated through by the JavaScript for..in loop.
   */
  facebook::jsi::Array getPropertyNames(const facebook::jsi::Object &obj) override;

  facebook::jsi::Array createArray(size_t length) override;
  size_t size(const facebook::jsi::Array &arr) override;
  size_t size(const facebook::jsi::ArrayBuffer &arrBuf) override;

  /**
   * @remarks The lifetime of the buffer returned is the same as the lifetime of
   * the ArrayBuffer. The returned buffer pointer does not count as a reference
   * to the ArrayBuffer for the purpose of garbage collection.
   */
  uint8_t *data(const facebook::jsi::ArrayBuffer &arrBuf) override;

  facebook::jsi::Value getValueAtIndex(const facebook::jsi::Array &arr, size_t index) override;
  void setValueAtIndexImpl(facebook::jsi::Array &arr, size_t index, const facebook::jsi::Value &value) override;

  facebook::jsi::Function createFunctionFromHostFunction(
      const facebook::jsi::PropNameID &name,
      unsigned int paramCount,
      facebook::jsi::HostFunctionType func) override;
  facebook::jsi::Value call(
      const facebook::jsi::Function &func,
      const facebook::jsi::Value &jsThis,
      const facebook::jsi::Value *args,
      size_t count) override;
  facebook::jsi::Value
  callAsConstructor(const facebook::jsi::Function &func, const facebook::jsi::Value *args, size_t count) override;

  ScopeState *pushScope() override;
  void popScope(ScopeState *) override;

  bool strictEquals(const facebook::jsi::Symbol &a, const facebook::jsi::Symbol &b) const override;
  bool strictEquals(const facebook::jsi::String &a, const facebook::jsi::String &b) const override;
  bool strictEquals(const facebook::jsi::Object &a, const facebook::jsi::Object &b) const override;

  bool instanceOf(const facebook::jsi::Object &obj, const facebook::jsi::Function &func) override;

#pragma endregion Functions_inherited_from_Runtime

  /**
   * @remarks This function requires a fully constructed ChakraRuntime and hence
   * must not be used in ChakraRuntime's constructor and destructor.
   */
  void VerifyJsErrorElseThrow(JsErrorCode error);

  inline JsRuntimeHandle GetRuntime() {
    return m_runtime;
  }

  virtual JSRuntimeVersion_t Version() = 0;

  /**
   * @brief A class used to interface with Facebook's jsi::Pointer class.
   *
   * @remarks ChakraPointerValue is needed for working with Facebook's
   * jsi::Pointer class and must only be used for this purpose. Every instance
   * of ChakraPointerValue should be allocated on the heap and be used as an
   * argument to the constructor of jsi::Pointer or one of its derived classes.
   * Pointer makes sure that invalidate(), which frees the heap allocated
   * ChakraPointerValue, is called upon destruction. Since the constructor of
   * jsi::Pointer is protected, we usually have to invoke it through
   * jsi::Runtime::make. The code should look something like:
   *     make<Pointer>(new ChakraPointerValue(...));
   * or you can use the helper function MakePointer(), as defined below.
   */
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
    ChakraPointerValueTemplate(const ChakraPointerValueTemplate &other) noexcept = default;
    ChakraPointerValueTemplate(ChakraPointerValueTemplate &&other) noexcept = default;
    ChakraPointerValueTemplate &operator=(const ChakraPointerValueTemplate &rhs) noexcept = default;
    ChakraPointerValueTemplate &operator=(ChakraPointerValueTemplate &&rhs) noexcept = default;

    inline void invalidate() noexcept override {
      delete this;
    }

    inline const T &GetRef() const noexcept {
      return m_ref;
    }

   private:
    /**
     * @remarks ~ChakraPointerValueTemplate() should only be invoked by
     * invalidate(). Hence we make it private.
     */
    ~ChakraPointerValueTemplate() noexcept = default;

    T m_ref;
  };

  using ChakraPointerValue = ChakraPointerValueTemplate<ChakraObjectRef>;

  template <typename T>
  inline T MakePointer(JsValueRef ref) {
    return MakePointer<T>(ChakraObjectRef(ref));
  }

  template <typename T>
  inline T MakePointer(ChakraObjectRef &&ref) {
    static_assert(
        std::is_base_of<facebook::jsi::Pointer, T>::value,
        "MakePointer should only be instantiated for classes derived from "
        "facebook::jsi::Pointer.");
    return make<T>(new ChakraPointerValue(std::move(ref)));
  }

  /**
   * @param pointerValue A pointer to a ChakraPointerValue.
   */
  inline static ChakraPointerValue *CloneChakraPointerValue(const PointerValue *pointerValue) {
    return new ChakraPointerValue(*(static_cast<const ChakraPointerValue *>(pointerValue)));
  }

  /**
   * @param p A jsi::Pointer holding a ChakraPointerValue.
   */
  inline static const ChakraObjectRef &GetChakraObjectRef(const facebook::jsi::Pointer &p) {
    return static_cast<const ChakraPointerValue *>(getPointerValue(p))->GetRef();
  }

  /**
   * @remarks These three functions only performs shallow copies.
   */
  /**@{*/
  facebook::jsi::Value ToJsiValue(ChakraObjectRef &&ref);
  ChakraObjectRef ToChakraObjectRef(const facebook::jsi::Value &value);
  std::vector<ChakraObjectRef> ToChakraObjectRefs(const facebook::jsi::Value *value, size_t count);
  /**@}*/

  /**
   * @remarks Since the function
   *     Object::getProperty(Runtime& runtime, const char* name)
   * causes mulitple copies of name, we do not want to use it when implementing
   * ChakraRuntime methods. This function does the same thing as
   * Object::getProperty, but without the extra overhead. This function is
   * declared as const so that it can be used when implementing
   * isHostFunction and isHostObject.
   */
  inline facebook::jsi::Value GetObjectProperty(const facebook::jsi::Object &obj, const char *const name) const {
    // We have to use a const_cast here because ToJsiValue cannnot be marked as
    // const.
    return const_cast<ChakraRuntime *>(this)->ToJsiValue(GetProperty(GetChakraObjectRef(obj), name));
  }

 private:
  /**
   * @remarks This function does not check for existing cached bytecode. It
   * loads the source code contained in buffer directly.
   */
  virtual facebook::jsi::Value EvaluateJavaScriptSimple(
      const std::shared_ptr<const facebook::jsi::Buffer> &buffer,
      const std::string &sourceUrl) = 0;

  virtual std::shared_ptr<const facebook::jsi::Buffer> SerializeScript(
      const std::shared_ptr<const facebook::jsi::Buffer> &buffer) = 0;

  void SetUpMemoryTracker();

  virtual void StartDebugging() = 0;

  virtual void InitializeAdditionalFeatures(){};

  static JsValueRef CALLBACK HostFunctionCall(
      JsValueRef callee,
      bool isConstructCall,
      JsValueRef *argumentsIncThis,
      unsigned short argumentCountIncThis,
      void *callbackState);

  /**
   * @param runtime A reference to a ChakraRuntime.
   */
  /**@{*/
  static facebook::jsi::Value HostObjectGetTrap(
      Runtime &runtime,
      const facebook::jsi::Value & /*thisVal*/,
      const facebook::jsi::Value *args,
      size_t count);
  static facebook::jsi::Value HostObjectSetTrap(
      Runtime &runtime,
      const facebook::jsi::Value & /*thisVal*/,
      const facebook::jsi::Value *args,
      size_t count);
  static facebook::jsi::Value HostObjectOwnKeysTrap(
      Runtime &runtime,
      const facebook::jsi::Value & /*thisVal*/,
      const facebook::jsi::Value *args,
      size_t count);
  /**@}*/

  facebook::jsi::Object createHostObjectProxyHandler() noexcept;

 private:
  std::shared_ptr<ChakraRuntimeArgs> m_args;
  JsRuntimeHandle m_runtime{JS_INVALID_RUNTIME_HANDLE};
  ChakraObjectRef m_context;
};

} // namespace Microsoft::JSI
