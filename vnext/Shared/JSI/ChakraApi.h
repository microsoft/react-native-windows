// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#ifdef CHAKRACORE
#include "ChakraCore.h"
#else
#ifndef USE_EDGEMODE_JSRT
#define USE_EDGEMODE_JSRT
#endif
#include <jsrt.h>
#endif

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

// We use macros to report errors.
// Macros provide more flexibility to show assert and provide failure context.

// Check condition and crash process if it fails.
#define ChakraVerifyElseCrash(condition, message)          \
  do {                                                     \
    if (!(condition)) {                                    \
      assert(false && "Failed: " #condition && (message)); \
      *((int *)0) = 1;                                     \
    }                                                      \
  } while (false)

// Throw native exception
#define ChakraThrow(message) ThrowNativeException(message);

// Check condition and throw native exception if it fails.
#define ChakraVerifyElseThrow(condition, message) \
  do {                                            \
    if (!(condition)) {                           \
      ThrowNativeException(message);              \
    }                                             \
  } while (false)

// Evaluate expression and throw JS exception if it fails.
#define ChakraVerifyJsErrorElseThrow(expression) \
  do {                                           \
    JsErrorCode temp_error_code_ = (expression); \
    if (temp_error_code_ != JsNoError) {         \
      ThrowJsException(temp_error_code_);        \
    }                                            \
  } while (false)

namespace Microsoft::JSI {

/**
 * @brief A wrapper for Chakra APIs.
 *
 * The ChakraApi class wraps up the Chakra API functions in a way that:
 * - functions throw exceptions instead of returning error code (derived class can define the exception types);
 * - standard library types are used when possible to simplify usage.
 *
 * The order of the functions below follows the corresponding functions order in the chakrart.h
 * Currently we only wrap up functions that are needed to implement the JSI API.
 */
struct ChakraApi {
  /**
   * @brief A smart pointer for JsRefs.
   *
   * JsRefs are references to objects owned by the garbage collector and include
   * JsContextRef, JsValueRef, and JsPropertyIdRef, etc. JsRefHolder ensures
   * that JsAddRef and JsRelease are called to handle the JsRef lifetime.
   */
  struct JsRefHolder final {
    JsRefHolder(std::nullptr_t = nullptr) noexcept {}
    explicit JsRefHolder(JsRef ref) noexcept;

    JsRefHolder(JsRefHolder const &other) noexcept;
    JsRefHolder(JsRefHolder &&other) noexcept;

    JsRefHolder &operator=(JsRefHolder const &other) noexcept;
    JsRefHolder &operator=(JsRefHolder &&other) noexcept;

    ~JsRefHolder() noexcept;

    operator JsRef() const noexcept {
      return m_ref;
    }

   private:
    JsRef m_ref{JS_INVALID_REFERENCE};
  };

  /**
   * @brief Interface that helps override the exception being thrown.
   *
   * The ChakraApi uses the ExceptionThrowerHolder to retrieve the ExceptionThrower
   * instance from the current thread.
   */
  struct IExceptionThrower {
    /**
     * @brief Throws an exception based on the errorCore.
     *
     * The base implementation throws the generic std::exception.
     * The method can be overridden in derived class to provide more specific exceptions.
     */
    [[noreturn]] virtual void ThrowJsExceptionOverride(JsErrorCode errorCode, JsValueRef jsError) = 0;

    /**
     * @brief Throws an exception with the provided errorMessage.
     *
     * The base implementation throws the generic std::exception.
     * The method can be overridden in derived class to provide more specific exceptions.
     */
    [[noreturn]] virtual void ThrowNativeExceptionOverride(char const *errorMessage) = 0;
  };

  /**
   * @brief A RAII class to hold IExceptionThrower instance in the thread local variable.
   */
  struct ExceptionThrowerHolder final {
    ExceptionThrowerHolder(IExceptionThrower *exceptionThrower) noexcept
        : m_previous{std::exchange(tls_exceptionThrower, exceptionThrower)} {}

    ~ExceptionThrowerHolder() noexcept {
      tls_exceptionThrower = m_previous;
    }

    static IExceptionThrower *Get() noexcept {
      return tls_exceptionThrower;
    }

   private:
    static thread_local IExceptionThrower *tls_exceptionThrower;
    IExceptionThrower *const m_previous;
  };

  /**
   * @brief Throws JavaScript exception with provided errorCode.
   */
  [[noreturn]] static void ThrowJsException(JsErrorCode errorCode);

  /**
   * @brief Throws native exception with provided message.
   */
  [[noreturn]] static void ThrowNativeException(char const *errorMessage);

  /**
   * @brief Creates a new runtime.
   */
  static JsRuntimeHandle CreateRuntime(JsRuntimeAttributes attributes, JsThreadServiceCallback threadService);

  /**
   * @brief Disposes a runtime.
   */
  static void DisposeRuntime(JsRuntimeHandle runtime);

  /**
   * @brief Adds a reference to a garbage collected object.
   */
  static uint32_t AddRef(JsRef ref);

  /**
   * @brief Releases a reference to a garbage collected object.
   */
  static uint32_t Release(JsRef ref);

  /**
   * @brief Creates a script context for running scripts.
   */
  static JsContextRef CreateContext(JsRuntimeHandle runtime);

  /**
   * @brief Gets the current script context on the thread..
   */
  JsContextRef GetCurrentContext();

  /**
   * @brief Sets the current script context on the thread.
   */
  void SetCurrentContext(JsContextRef context);

  /**
   * @brief Gets the property ID associated with the name.
   */
  static JsPropertyIdRef GetPropertyIdFromName(wchar_t const *name);

  /**
   * @brief Gets the property ID associated with the name.
   */
  static JsPropertyIdRef GetPropertyIdFromName(std::string_view name);

  /**
   * @brief Gets the property ID associated with the string.
   */
  static JsPropertyIdRef GetPropertyIdFromString(JsValueRef value);

  /**
   * @brief Gets the name associated with the property ID.
   */
  static wchar_t const *GetPropertyNameFromId(JsPropertyIdRef propertyId);

  /**
   * @brief Gets the string associated with the property ID.
   */
  static JsValueRef GetPropertyStringFromId(JsPropertyIdRef propertyId);

  /**
   * @brief Gets the symbol associated with the property ID.
   */
  static JsValueRef GetSymbolFromPropertyId(JsPropertyIdRef propertyId);

  /**
   * @brief Gets the type of property.
   */
  static JsPropertyIdType GetPropertyIdType(JsPropertyIdRef propertyId);

  /**
   * @brief Gets the property ID associated with the symbol.
   */
  static JsPropertyIdRef GetPropertyIdFromSymbol(JsValueRef symbol);

  /**
   * @brief Creates symbol and gets the property ID associated with the symbol.
   */
  static JsPropertyIdRef GetPropertyIdFromSymbol(std::wstring_view symbolDescription);

  /**
   * @brief Creates a JavaScript symbol.
   */
  static JsValueRef CreateSymbol(JsValueRef symbolDescription);

  /**
   * @brief Creates a JavaScript symbol.
   */
  static JsValueRef CreateSymbol(std::wstring_view symbolDescription);

  /**
   * @brief Gets the value of \c undefined in the current script context.
   */
  static JsValueRef GetUndefinedValue();

  /**
   * @brief Gets the value of \c null in the current script context.
   */
  static JsValueRef GetNullValue();

  /**
   * @brief Creates a Boolean value from a \c bool value.
   */
  static JsValueRef BoolToBoolean(bool value);

  /**
   * @brief Retrieves the \c bool value of a Boolean value.
   */
  static bool BooleanToBool(JsValueRef value);

  /**
   * @brief Gets the JavaScript type of a JsValueRef.
   */
  static JsValueType GetValueType(JsValueRef value);

  /**
   * @brief Creates a number value from a \c double value.
   */
  static JsValueRef DoubleToNumber(double value);

  /**
   * @brief Creates a number value from an \c int value.
   */
  static JsValueRef IntToNumber(int32_t value);

  /**
   * @brief Retrieves the \c double value of a number value.
   */
  static double NumberToDouble(JsValueRef value);

  /**
   * @brief Retrieves the \c int value of a number value.
   */
  static int32_t NumberToInt(JsValueRef value);

  /**
   * @brief Creates a string value from a string pointer represented as std::wstring_view.
   */
  static JsValueRef PointerToString(std::wstring_view value);

  /**
   * @brief Creates a string value from a string pointer represented as std::string_view.
   */
  static JsValueRef PointerToString(std::string_view value);

  /**
   * @brief Retrieves the string pointer of a string value.
   *
   * This function retrieves the string pointer of a string value. It will fail with
   * an exception with \c JsErrorInvalidArgument if the type of the value is not string. The lifetime
   * of the string returned will be the same as the lifetime of the value it came from, however
   * the string pointer is not considered a reference to the value (and so will not keep it
   *from being collected).
   */
  static std::wstring_view StringToPointer(JsValueRef string);

  /**
   * @brief Converts the string to the utf8-encoded std::string.
   */
  static std::string StringToStdString(JsValueRef string);

  /**
   * @brief Converts the value to string using standard JavaScript semantics.
   */
  static JsValueRef ConvertValueToString(JsValueRef value);

  /**
   * @brief Gets the global object in the current script context.
   */
  static JsValueRef GetGlobalObject();

  /**
   * @brief Creates a new object.
   */
  static JsValueRef CreateObject();

  /**
   * @brief Creates a new object that stores some external data.
   */
  static JsValueRef CreateExternalObject(void *data, JsFinalizeCallback finalizeCallback);

  /**
   * @brief Creates a new object that stores external data as an std::unique_ptr.
   */
  template <typename T>
  static JsValueRef CreateExternalObject(std::unique_ptr<T> &&data) {
    JsValueRef object = CreateExternalObject(data.get(), [](void *dataToDestroy) {
      // We wrap dataToDestroy in a unique_ptr to avoid calling delete explicitly.
      std::unique_ptr<T> wrapper{static_cast<T *>(dataToDestroy)};
    });

    // We only call data.release() after the CreateExternalObject succeeds.
    // Otherwise, when CreateExternalObject fails and an exception is thrown,
    // the memory that data used to own will be leaked.
    data.release();
    return object;
  }

  /**
   * @brief Returns the prototype of an object.
   */
  static JsValueRef GetPrototype(JsValueRef object);

  /**
   * @brief Performs JavaScript "instanceof" operator test.
   */
  static bool InstanceOf(JsValueRef object, JsValueRef constructor);

  /**
   * @brief Gets an object's property.
   */
  static JsValueRef GetProperty(JsValueRef object, JsPropertyIdRef propertyId);

  /**
   * @brief Gets the list of all properties on the object.
   */
  static JsValueRef GetOwnPropertyNames(JsValueRef object);

  /**
   * @brief Puts an object's property.
   */
  static void SetProperty(JsValueRef object, JsPropertyIdRef propertyId, JsValueRef value);

  /**
   * @brief Determines whether an object has a property.
   */
  static bool HasProperty(JsValueRef object, JsPropertyIdRef propertyId);

  /**
   * @brief Defines a new object's own property from a property descriptor.
   */
  static bool DefineProperty(JsValueRef object, JsPropertyIdRef propertyId, JsValueRef propertyDescriptor);

  /**
   * @brief Retrieve the value at the specified index of an object.
   */
  static JsValueRef GetIndexedProperty(JsValueRef object, int32_t index);

  /**
   * @brief Set the value at the specified index of an object.
   */
  static void SetIndexedProperty(JsValueRef object, int32_t index, JsValueRef value);

  /**
   * @brief Compare two JavaScript values for strict equality.
   */
  static bool StrictEquals(JsValueRef object1, JsValueRef object2);

  /**
   * @brief Retrieves the data from an external object.
   */
  static void *GetExternalData(JsValueRef object);

  /**
   * @brief Creates a JavaScript array object.
   */
  static JsValueRef CreateArray(size_t length);

  /**
   * @brief Creates a JavaScript ArrayBuffer object.
   */
  static JsValueRef CreateArrayBuffer(size_t byteLength);

  /**
   * @brief A span of values that can be used to pass arguments to function.
   *
   * For C++20 we should consider to replace it with std::span.
   */
  template <typename T>
  struct Span final {
    constexpr Span(std::initializer_list<T> il) noexcept : m_data{const_cast<T *>(il.begin())}, m_size{il.size()} {}
    constexpr Span(T *data, size_t size) noexcept : m_data{data}, m_size{size} {}

    [[nodiscard]] constexpr T *begin() const noexcept {
      return m_data;
    }

    [[nodiscard]] constexpr T *end() const noexcept {
      return m_data + m_size;
    }

    [[nodiscard]] constexpr size_t size() const noexcept {
      return m_size;
    }

   private:
    T *const m_data;
    size_t const m_size;
  };

  /**
   * @brief Obtains the underlying memory storage used by an \c ArrayBuffer.
   */
  Span<std::byte> GetArrayBufferStorage(JsValueRef arrayBuffer);

  /**
   * @brief Invokes a function.
   */
  static JsValueRef CallFunction(JsValueRef function, Span<JsValueRef> args);

  /**
   * @brief Invokes a function as a constructor.
   */
  static JsValueRef ConstructObject(JsValueRef function, Span<JsValueRef> args);

  /**
   * @brief Creates a new JavaScript function with name.
   */
  static JsValueRef CreateNamedFunction(JsValueRef name, JsNativeFunction nativeFunction, void *callbackState);

  /**
   * @brief  Sets the runtime of the current context to an exception state.
   *
   * It returns \c false in case if the current context is already in an exception state.
   */
  static bool SetException(JsValueRef error) noexcept;

  /**
   * @brief  Sets the runtime of the current context to an exception state.
   */
  static bool SetException(std::string_view message) noexcept;

  /**
   * @brief  Sets the runtime of the current context to an exception state.
   */
  static bool SetException(std::wstring_view message) noexcept;

 private:
};

} // namespace Microsoft::JSI
