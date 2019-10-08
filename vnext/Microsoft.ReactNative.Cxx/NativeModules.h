#pragma once
#include "winrt/Microsoft.ReactNative.Bridge.h"
#include "winrt/Microsoft.ReactNative.h"

#include <type_traits>
#include "ModuleRegistration.h"

// The macro to annotate C++ class to become a ReactNative module.
// Arguments:
// - className (required) - the class name it is attached to
// - moduleName (optional) - the module name visible to JavaScript. Default is
//     the className.
// - eventEmitterName (optional) - the event emitter name use din JavaScript.
//     Default is the moduleName.
#define REACT_MODULE(moduleClass, ...)                          \
  INTERNAL_REACT_MODULE_MACRO_CHOOSER(moduleClass, __VA_ARGS__) \
  (moduleClass, __VA_ARGS__)

// The macro to annotate a method that it must be exported to JavaScript.
// It declares an asynchronous method. To return a value:
// - Return void and have a Callback as a last parameter. The Callback type can
// be any std::function like type. I.e. Func<void(Args...)>.
// - Return void and have two callbacks as last parameters. In JavaScript the
// method returns Promise. To keep it as two callbacks use the
// REACT_ASYNC_METHOD macro.
// - Return non-void value. In JavaScript it is treated as a method with one
// Callback. Return std::pair<Error, Value> to be able to communicate error
// condition.
#define REACT_METHOD(method) REACT_METHOD_JSNAME(method, #method)

// The same as REACT_METHOD but with custom JS name.
#define REACT_METHOD_JSNAME(method, jsName) \
  REACT_METHOD_INTERNAL(method, jsName, , false)

// The same as REACT_METHOD, but two callbacks are exposed as callbacks, and not
// as a Promise.
#define REACT_ASYNC_METHOD(method) REACT_METHOD_ASYNC_JSNAME(method, #method)

// The same as REACT_ASYNC_METHOD but with custom JS name.
#define REACT_ASYNC_METHOD_JSNAME(method, jsName) \
  REACT_METHOD_INTERNAL(method, jsName, , true)

// A method that is called synchronously. It must be used rarely because it may
// cause out-of-order execution when used along with asynchronous methods.
#define REACT_SYNC_METHOD(method) REACT_SYNC_METHOD_JSNAME(method, #method)

// The same as REACT_SYNC_METHOD but with custom JS name.
#define REACT_SYNC_METHOD_JSNAME(method, jsName) \
  REACT_METHOD_INTERNAL(method, jsName, Sync, false)

// A method where we can define constants.
// Constant definition relies on a TLS context that is setup when object is
// created.
#define REACT_CONST_METHOD(method) \
  REACT_METHOD_INTERNAL(method, "", Const, false)

// Internal implementation details.
// Registration of a method relies on a TLS context that is setup when object is
// created. The advantage is that we do zero work during static initialization.
// The disadvantage is that we require to have one bool field per registration.
#define REACT_METHOD_INTERNAL(method, jsName, type, isAsync)       \
  bool REACT_reg##method{                                          \
      ::Microsoft::ReactNative::Module##type##MethodInfo<decltype( \
          &std::remove_pointer_t<decltype(this)>::method)>::       \
          Register(                                                \
              this,                                                \
              jsName,                                              \
              &std::remove_pointer_t<decltype(this)>::method,      \
              isAsync)};

#define REACT_CONSTANT(field) REACT_CONSTANT_JSNAME(field, #field)

#define REACT_CONSTANT_JSNAME(field, jsName)                   \
  bool REACT_reg##field{                                       \
      ::Microsoft::ReactNative::ModuleConstFieldInfo<decltype( \
          &std::remove_pointer_t<decltype(this)>::field)>::    \
          Register(                                            \
              this, jsName, &std::remove_pointer_t<decltype(this)>::field)};

#define REACT_EVENT(field) REACT_EVENT_JSNAME(field, #field)

#define REACT_EVENT_JSNAME(field, jsName)                      \
  bool REACT_reg##field{                                       \
      ::Microsoft::ReactNative::ModuleEventFieldInfo<decltype( \
          &std::remove_pointer_t<decltype(this)>::field)>::    \
          Register(                                            \
              this, jsName, &std::remove_pointer_t<decltype(this)>::field)};

#define REACT_REGISTER_METHOD(method) \
  REACT_REGISTER_METHOD_JSNAME(method, #method)

// The same as REACT_METHOD but with custom JS name.
#define REACT_REGISTER_METHOD_JSNAME(method, jsName) \
  REACT_REGISTER_METHOD_INTERNAL(method, jsName, , false)

// The same as REACT_METHOD, but two callbacks are exposed as callbacks, and not
// as a Promise.
#define REACT_REGISTER_ASYNC_METHOD(method) \
  REACT_REGISTER_METHOD_ASYNC_JSNAME(method, #method)

// The same as REACT_ASYNC_METHOD but with custom JS name.
#define REACT_REGISTER_ASYNC_METHOD_JSNAME(method, jsName) \
  REACT_REGISTER_METHOD_INTERNAL(method, jsName, , true)

// A method that is called synchronously. It must be used rarely because it may
// cause out-of-order execution when used along with asynchronous methods.
#define REACT_REGISTER_SYNC_METHOD(method) \
  REACT_REGISTER_SYNC_METHOD_JSNAME(method, #method)

// The same as REACT_SYNC_METHOD but with custom JS name.
#define REACT_REGISTER_SYNC_METHOD_JSNAME(method, jsName) \
  REACT_REGISTER_METHOD_INTERNAL(method, jsName, Sync, false)

// The same as REACT_SYNC_METHOD but with custom JS name.
#define REACT_REGISTER_CONST_METHOD(method) \
  REACT_REGISTER_METHOD_INTERNAL(method, "", Const, false)

#define REACT_REGISTER_METHOD_INTERNAL(method, jsName, type, isAsync) \
  (void)::Microsoft::ReactNative::Module##type##MethodInfo<decltype(  \
      &std::remove_pointer_t<decltype(this)>::method)>::              \
      Register(                                                       \
          this,                                                       \
          jsName,                                                     \
          &std::remove_pointer_t<decltype(this)>::method,             \
          isAsync);

#define REACT_REGISTER_CONSTANT(field) \
  REACT_REGISTER_CONSTANT_JSNAME(field, #field)

#define REACT_REGISTER_CONSTANT_JSNAME(field, jsName)      \
  ::Microsoft::ReactNative::ModuleConstFieldInfo<decltype( \
      &std::remove_pointer_t<decltype(this)>::field)>::    \
      Register(this, jsName, &std::remove_pointer_t<decltype(this)>::field);

// A method where we can define constants.
// Constant definition relies on a TLS context that is setup when object is
// created.
#define REACT_CONSTANT_VALUE(jsName, value) \
  ::Microsoft::ReactNative::ModuleConstantInfo::Register(this, jsName, value);

#define REACT_REGISTER_EVENT(field) REACT_REGISTER_EVENT_JSNAME(field, #field)

#define REACT_REGISTER_EVENT_JSNAME(field, jsName)               \
  (void)::Microsoft::ReactNative::ModuleEventFieldInfo<decltype( \
      &std::remove_pointer_t<decltype(this)>::field)>::          \
      Register(this, jsName, &std::remove_pointer_t<decltype(this)>::field);

//==============================================================================
// IJSValueReader helpers
//==============================================================================
namespace Microsoft::ReactNative {

template <class T>
inline bool ReadValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueReader const &reader,
    T &value) noexcept {
  static_assert(
      sizeof(std::decay_t<T>) == 0, "Implement ReadValue for the T type");
}

inline bool ReadValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueReader const &reader,
    bool &value) noexcept {
  return reader.TryGetBoolen(value);
}

inline bool ReadValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueReader const &reader,
    int8_t &value) noexcept {
  int64_t result{};
  return reader.TryGetInt64(result) &&
      (value = static_cast<int8_t>(result), true);
}

inline bool ReadValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueReader const &reader,
    int16_t &value) noexcept {
  int64_t result{};
  return reader.TryGetInt64(result) &&
      (value = static_cast<int16_t>(result), true);
}

inline bool ReadValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueReader const &reader,
    int32_t &value) noexcept {
  int64_t result{};
  return reader.TryGetInt64(result) &&
      (value = static_cast<int32_t>(result), true);
}

inline bool ReadValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueReader const &reader,
    int64_t &value) noexcept {
  return reader.TryGetInt64(value);
}

inline bool ReadValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueReader const &reader,
    uint8_t &value) noexcept {
  int64_t result{};
  return reader.TryGetInt64(result) &&
      (value = static_cast<uint8_t>(result), true);
}

inline bool ReadValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueReader const &reader,
    uint16_t &value) noexcept {
  int64_t result{};
  return reader.TryGetInt64(result) &&
      (value = static_cast<uint16_t>(result), true);
}

inline bool ReadValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueReader const &reader,
    uint32_t &value) noexcept {
  int64_t result{};
  return reader.TryGetInt64(result) &&
      (value = static_cast<uint32_t>(result), true);
}

inline bool ReadValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueReader const &reader,
    uint64_t &value) noexcept {
  int64_t result{};
  return reader.TryGetInt64(result) &&
      (value = static_cast<uint64_t>(result), true);
}

inline bool ReadValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueReader const &reader,
    double &value) noexcept {
  return reader.TryGetDouble(value);
}

inline bool ReadValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueReader const &reader,
    float &value) noexcept {
  double result{};
  return reader.TryGetDouble(result) &&
      (value = static_cast<float>(result), true);
}

inline bool ReadValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueReader const &reader,
    std::string &value) noexcept {
  winrt::hstring str;
  return reader.TryGetString(str) && (value = winrt::to_string(str), true);
}

inline bool ReadValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueReader const &reader,
    std::wstring &value) noexcept {
  winrt::hstring str;
  return reader.TryGetString(str) &&
      (value = std::wstring{str.data(), str.size()}, true);
}

template <class... TArgs>
inline void ReadArgs(
    winrt::Microsoft::ReactNative::Bridge::IJSValueReader const &reader,
    TArgs &... args) noexcept {
  if constexpr (sizeof...(args) > 0) {
    using winrt::Microsoft::ReactNative::Bridge::JSValueReaderState;
    JSValueReaderState state = reader.ReadNext();
    if (state == JSValueReaderState::ArrayBegin) {
      // To read variadic template arguments in natural order we must use them
      // in an initializer list.
      [[maybe_unused]] int dummy[] = {
          (state = reader.ReadNext(),
           state != JSValueReaderState::ArrayEnd && ReadValue(reader, args),
           0)...};
    }
  }
}

} // namespace Microsoft::ReactNative

//==============================================================================
// IJSValueWriter helpers
//==============================================================================
namespace Microsoft::ReactNative {

template <class T>
inline bool WriteProperty(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    const std::string &name,
    T value) noexcept {
  return writer.WritePropertyName(winrt::to_hstring(name)) &&
      WriteValue(writer, value);
}

template <class T>
inline bool WriteProperty(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    const char *name,
    T value) noexcept {
  return writer.WritePropertyName(winrt::to_hstring(name)) &&
      WriteValue(writer, value);
}

template <class T>
inline bool WriteValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    T value) noexcept {
  static_assert(
      sizeof(std::decay_t<T>) == 0, "Implement WriteValue for the T type");
}

inline bool WriteValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    bool value) noexcept {
  return writer.WriteBoolean(value);
}

inline bool WriteValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    int8_t value) noexcept {
  return writer.WriteInt64(value);
}

inline bool WriteValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    int16_t value) noexcept {
  return writer.WriteInt64(value);
}

inline bool WriteValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    int32_t value) noexcept {
  return writer.WriteInt64(value);
}

inline bool WriteValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    int64_t value) noexcept {
  return writer.WriteInt64(value);
}

inline bool WriteValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    uint8_t value) noexcept {
  return writer.WriteInt64(static_cast<int64_t>(value));
}

inline bool WriteValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    uint16_t value) noexcept {
  return writer.WriteInt64(static_cast<int64_t>(value));
}

inline bool WriteValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    uint32_t value) noexcept {
  return writer.WriteInt64(static_cast<int64_t>(value));
}

inline bool WriteValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    uint64_t value) noexcept {
  return writer.WriteInt64(static_cast<int64_t>(value));
}

inline bool WriteValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    double value) noexcept {
  return writer.WriteDouble(value);
}

inline bool WriteValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    float value) noexcept {
  return writer.WriteDouble(value);
}

inline bool WriteValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    const std::string &value) noexcept {
  return writer.WriteString(winrt::to_hstring(value));
}

inline bool WriteValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    const char *value) noexcept {
  return writer.WriteString(winrt::to_hstring(value));
}

inline bool WriteValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    const std::wstring &value) noexcept {
  return writer.WriteString(value);
}

inline bool WriteValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    const wchar_t *value) noexcept {
  return writer.WriteString(value);
}

template <class... TArgs>
inline void WriteArgs(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    const TArgs &... args) noexcept {
  writer.WriteArrayBegin();

  if constexpr (sizeof...(args) > 0) {
    // To write variadic template arguments in natural order we must use them in
    // an initializer list.
    [[maybe_unused]] int dummy[] = {(WriteValue(writer, args), 0)...};
  }

  writer.WriteArrayEnd();
}

} // namespace Microsoft::ReactNative

namespace Microsoft::ReactNative {

namespace Internal {

// Checks if provided type has a callback-like signature TFunc<void(TArgs...)
template <class T>
struct IsCallback : std::false_type {};
template <template <class...> class TFunc, class... TArgs>
struct IsCallback<TFunc<void(TArgs...)>> : std::true_type {};
#if defined(__cpp_noexcept_function_type) || (_HAS_NOEXCEPT_FUNCTION_TYPES == 1)
template <template <class...> class TFunc, class... TArgs>
struct IsCallback<TFunc<void(TArgs...) noexcept>> : std::true_type {};
#endif

// Finds how many callback the function has
template <class TArgTuple>
struct GetCallbackCount;

template <>
struct GetCallbackCount<std::tuple<>> {
  constexpr static size_t Value = 0;
};

template <class TArg>
struct GetCallbackCount<std::tuple<TArg>> {
  constexpr static size_t Value = IsCallback<TArg>::value ? 1 : 0;
};

template <class TArg0, class TArg1, class... TArgs>
struct GetCallbackCount<std::tuple<TArg0, TArg1, TArgs...>> {
  using TupleType = std::tuple<TArg0, TArg1, TArgs...>;
  constexpr static size_t TupleSize = std::tuple_size_v<TupleType>;
  constexpr static size_t Value =
      (IsCallback<std::tuple_element_t<TupleSize - 2, TupleType>>::value ? 1
                                                                         : 0) +
      (IsCallback<std::tuple_element_t<TupleSize - 1, TupleType>>::value ? 1
                                                                         : 0);
};

template <class T>
struct ThreadLocalHolder {
  ThreadLocalHolder(T *value) noexcept : m_savedValue{tl_value} {
    tl_value = value;
  }

  ~ThreadLocalHolder() noexcept {
    tl_value = m_savedValue;
  }

  static T *Get() noexcept {
    return tl_value;
  }

 private:
  T *m_savedValue{nullptr};
  static thread_local T *tl_value;
};

template <class T>
/*static*/ thread_local T *ThreadLocalHolder<T>::tl_value{nullptr};

} // namespace Internal

using CurrentNativeModuleBuilder = Internal::ThreadLocalHolder<
    const winrt::Microsoft::ReactNative::Bridge::INativeModuleBuilder>;

//==============================================================================
// Module registration helpers
//==============================================================================

template <class TModule>
inline winrt::Microsoft::ReactNative::Bridge::ReactModuleProvider
MakeModuleProvider() noexcept {
  using winrt::Microsoft::ReactNative::Bridge::INativeModuleBuilder;

  return [module = std::shared_ptr<TModule>{nullptr}](
      INativeModuleBuilder const &moduleBuilder) mutable noexcept {
    CurrentNativeModuleBuilder currentModuleBuilder{&moduleBuilder};
    module = std::make_shared<TModule>();
    return winrt::Windows::Foundation::IInspectable{nullptr};
  };
}

template <class TFunc>
struct ModuleMethodInfo;

template <class TModule, class... TArgs>
struct ModuleMethodInfo<void (TModule::*)(TArgs...) noexcept> {
  constexpr static size_t CallbackCount = Internal::GetCallbackCount<
      std::tuple<std::remove_reference_t<TArgs>...>>::Value;
  using ModuleType = TModule;
  using MethodType = void (TModule::*)(TArgs...) noexcept;
  using IndexSequence =
      std::make_index_sequence<sizeof...(TArgs) - CallbackCount>;

  using IJSValueReader = winrt::Microsoft::ReactNative::Bridge::IJSValueReader;
  using IJSValueWriter = winrt::Microsoft::ReactNative::Bridge::IJSValueWriter;
  using MethodDelegate = winrt::Microsoft::ReactNative::Bridge::MethodDelegate;
  using MethodResultCallback =
      winrt::Microsoft::ReactNative::Bridge::MethodResultCallback;

  template <class>
  struct Invoker;

  template <size_t... I>
  struct Invoker<std::index_sequence<I...>> {
    // Fire and forget method
    static MethodDelegate GetFunc(
        ModuleType *module,
        MethodType method,
        std::integral_constant<size_t, 0>) noexcept {
      return [ module, method ](
          const IJSValueReader &argReader,
          const IJSValueWriter & /*argWriter*/,
          const MethodResultCallback &,
          const MethodResultCallback &) mutable noexcept {
        std::tuple<std::remove_reference_t<TArgs>...> typedArgs{};
        ReadArgs(argReader, std::get<I>(typedArgs)...);
        (module->*method)(std::get<I>(std::move(typedArgs))...);
      };
    }

    template <class T>
    struct CallbackCreator;

    template <template <class...> class TCallback, class... TArgs>
    struct CallbackCreator<TCallback<void(TArgs...)>> {
      static TCallback<void(TArgs...)> Create(
          const IJSValueWriter &argWriter,
          const MethodResultCallback &callback) noexcept {
        return TCallback([ callback = std::move(callback), argWriter ](
            TArgs... args) noexcept {
          WriteArgs(argWriter, std::move(args)...);
          callback(argWriter);
        });
      }
    };

    template <class T, class = void>
    struct RejectCallbackCreator;

    template <template <class...> class TCallback, class TArg>
    struct RejectCallbackCreator<
        TCallback<void(TArg)>,
        std::enable_if_t<
            std::is_assignable_v<std::string, TArg> ||
            std::is_assignable_v<std::wstring, TArg>>> {
      static TCallback<void(TArg)> Create(
          const IJSValueWriter &argWriter,
          const MethodResultCallback &callback) noexcept {
        return TCallback(
            [ callback = std::move(callback), argWriter ](TArg arg) noexcept {
              argWriter.WriteArrayBegin();
              argWriter.WriteObjectBegin();
              argWriter.WritePropertyName(L"message");
              WriteValue(argWriter, arg);
              argWriter.WriteObjectEnd();
              argWriter.WriteArrayEnd();
              callback(argWriter);
            });
      }
    };

    template <template <class...> class TCallback>
    struct RejectCallbackCreator<TCallback<void()>, void>
        : CallbackCreator<TCallback<void()>> {};

    template <template <class...> class TCallback, class TArg>
    struct RejectCallbackCreator<
        TCallback<void(TArg)>,
        std::enable_if_t<
            !std::is_assignable_v<std::string, TArg> &&
            !std::is_assignable_v<std::wstring, TArg>>>
        : CallbackCreator<TCallback<void(TArg)>> {};

    template <
        template <class...>
        class TCallback,
        class TArg0,
        class TArg1,
        class... TArgs>
    struct RejectCallbackCreator<TCallback<void(TArg0, TArg1, TArgs...)>, void>
        : CallbackCreator<TCallback<void(TArg0, TArg1, TArgs...)>> {};

    // Method with one callback
    static MethodDelegate GetFunc(
        ModuleType *module,
        MethodType method,
        std::integral_constant<size_t, 1>) noexcept {
      return [ module, method ](
          const IJSValueReader &argReader,
          const IJSValueWriter &argWriter,
          const MethodResultCallback &callback,
          const MethodResultCallback &) mutable noexcept {
        using ArgTuple = std::tuple<std::remove_reference_t<TArgs>...>;
        ArgTuple typedArgs{};
        ReadArgs(argReader, std::get<I>(typedArgs)...);
        (module->*method)(
            std::get<I>(std::move(typedArgs))...,
            CallbackCreator<
                std::tuple_element_t<sizeof...(TArgs) - 1, ArgTuple>>::
                Create(argWriter, std::move(callback)));
      };
    }

    // Method with two callbacks
    static MethodDelegate GetFunc(
        ModuleType *module,
        MethodType method,
        std::integral_constant<size_t, 2>) noexcept {
      return [ module, method ](
          const IJSValueReader &argReader,
          const IJSValueWriter &argWriter,
          const MethodResultCallback &callback1,
          const MethodResultCallback &callback2) mutable noexcept {
        using ArgTuple = std::tuple<std::remove_reference_t<TArgs>...>;
        ArgTuple typedArgs{};
        ReadArgs(argReader, std::get<I>(typedArgs)...);
        (module->*method)(
            std::get<I>(std::move(typedArgs))...,
            CallbackCreator<
                std::tuple_element_t<sizeof...(TArgs) - 2, ArgTuple>>::
                Create(argWriter, std::move(callback1)),
            RejectCallbackCreator<
                std::tuple_element_t<sizeof...(TArgs) - 1, ArgTuple>>::
                Create(argWriter, std::move(callback2)));
      };
    }
  };

  static bool Register(
      void *module,
      const char *jsName,
      MethodType method,
      [[maybe_unused]] bool isAsync) noexcept {
    using winrt::Microsoft::ReactNative::Bridge::INativeModuleBuilder;
    using winrt::Microsoft::ReactNative::Bridge::MethodReturnType;

    if (auto m = static_cast<ModuleType *>(module)) {
      MethodReturnType returnType{MethodReturnType::Void};
      if constexpr (CallbackCount == 1) {
        returnType = MethodReturnType::Callback;
      } else if constexpr (CallbackCount == 2) {
        if (isAsync) {
          returnType = MethodReturnType::TwoCallbacks;
        } else {
          returnType = MethodReturnType::Promise;
        }
      }

      MethodDelegate methodDelegate = Invoker<IndexSequence>::GetFunc(
          m, method, std::integral_constant<size_t, CallbackCount>{});
      CurrentNativeModuleBuilder::Get()->AddMethod(
          winrt::to_hstring(jsName), returnType, methodDelegate);
    }
    return false;
  }
};

template <class TModule, class TResult, class... TArgs>
struct ModuleMethodInfo<TResult (TModule::*)(TArgs...) noexcept> {
  using ModuleType = TModule;
  using MethodType = TResult (TModule::*)(TArgs...) noexcept;
  using IndexSequence = std::make_index_sequence<sizeof...(TArgs)>;

  using IJSValueReader = winrt::Microsoft::ReactNative::Bridge::IJSValueReader;
  using IJSValueWriter = winrt::Microsoft::ReactNative::Bridge::IJSValueWriter;
  using MethodDelegate = winrt::Microsoft::ReactNative::Bridge::MethodDelegate;
  using MethodResultCallback =
      winrt::Microsoft::ReactNative::Bridge::MethodResultCallback;

  template <class>
  struct Invoker;

  template <size_t... I>
  struct Invoker<std::index_sequence<I...>> {
    // Async method with return value
    static MethodDelegate GetFunc(
        ModuleType *module,
        MethodType method) noexcept {
      return [ module, method ](
          const IJSValueReader &argReader,
          const IJSValueWriter &argWriter,
          const MethodResultCallback &callback,
          const MethodResultCallback &) mutable noexcept {
        using ArgTuple = std::tuple<std::remove_reference_t<TArgs>...>;
        ArgTuple typedArgs{};
        ReadArgs(argReader, std::get<I>(typedArgs)...);
        TResult result =
            (module->*method)(std::get<I>(std::move(typedArgs))...);
        WriteArgs(argWriter, std::move(result));
        callback(argWriter);
      };
    }
  };

  static bool Register(
      void *module,
      const char *jsName,
      MethodType method,
      bool /*isAsync*/) noexcept {
    using winrt::Microsoft::ReactNative::Bridge::INativeModuleBuilder;
    using winrt::Microsoft::ReactNative::Bridge::MethodReturnType;

    if (auto m = static_cast<ModuleType *>(module)) {
      MethodDelegate methodDelegate =
          Invoker<IndexSequence>::GetFunc(m, method);
      CurrentNativeModuleBuilder::Get()->AddMethod(
          winrt::to_hstring(jsName),
          MethodReturnType::Callback,
          methodDelegate);
    }
    return false;
  }
};

template <class TFunc>
struct ModuleSyncMethodInfo;

template <class TModule, class TResult, class... TArgs>
struct ModuleSyncMethodInfo<TResult (TModule::*)(TArgs...) noexcept> {
  using ModuleType = TModule;
  using MethodType = TResult (TModule::*)(TArgs...) noexcept;
  using IndexSequence = std::make_index_sequence<sizeof...(TArgs)>;

  using IJSValueReader = winrt::Microsoft::ReactNative::Bridge::IJSValueReader;
  using IJSValueWriter = winrt::Microsoft::ReactNative::Bridge::IJSValueWriter;
  using SyncMethodDelegate =
      winrt::Microsoft::ReactNative::Bridge::SyncMethodDelegate;

  template <class>
  struct Invoker;

  template <size_t... I>
  struct Invoker<std::index_sequence<I...>> {
    static SyncMethodDelegate GetFunc(
        ModuleType *module,
        MethodType method) noexcept {
      return [ module, method ](
          IJSValueReader const &argReader,
          IJSValueWriter const &argWriter) mutable noexcept {
        using ArgTuple = std::tuple<std::remove_reference_t<TArgs>...>;
        ArgTuple typedArgs{};
        ReadArgs(argReader, std::get<I>(typedArgs)...);
        TResult result =
            (module->*method)(std::get<I>(std::move(typedArgs))...);
        WriteArgs(argWriter, std::move(result));
      };
    }
  };

  static bool Register(
      void *module,
      const char *jsName,
      MethodType method,
      bool /*isAsync*/) noexcept {
    using winrt::Microsoft::ReactNative::Bridge::INativeModuleBuilder;

    if (auto m = static_cast<ModuleType *>(module)) {
      SyncMethodDelegate syncMethodDelegate =
          Invoker<IndexSequence>::GetFunc(m, method);
      CurrentNativeModuleBuilder::Get()->AddSyncMethod(
          winrt::to_hstring(jsName), syncMethodDelegate);
    }
    return false;
  }
};

template <class TFunc>
struct ModuleConstMethodInfo;

template <class TModule>
struct ModuleConstMethodInfo<void (TModule::*)(
    const winrt::Microsoft::ReactNative::Bridge::IJSValueWriter &) noexcept> {
  using ModuleType = TModule;
  using IJSValueWriter = winrt::Microsoft::ReactNative::Bridge::IJSValueWriter;
  using MethodType = void (TModule::*)(const IJSValueWriter &) noexcept;

  static bool Register(
      void *module,
      const char * /*jsName*/,
      MethodType method,
      bool /*isAsync*/) noexcept {
    using winrt::Microsoft::ReactNative::Bridge::INativeModuleBuilder;

    if (auto m = static_cast<ModuleType *>(module)) {
      CurrentNativeModuleBuilder::Get()->AddConstantWriter(
          [ m, method ](const IJSValueWriter &argWriter) mutable noexcept {
            (m->*method)(argWriter);
          });
    }
    return false;
  }
};

template <class TFunc>
struct ModuleConstFieldInfo;

template <class TModule, class TValue>
struct ModuleConstFieldInfo<TValue TModule::*> {
  using ModuleType = TModule;
  using FieldType = TValue TModule::*;

  static bool
  Register(void *module, const char *jsName, FieldType field) noexcept {
    using winrt::Microsoft::ReactNative::Bridge::INativeModuleBuilder;
    using winrt::Microsoft::ReactNative::Bridge::IJSValueWriter;

    if (auto m = static_cast<ModuleType *>(module)) {
      CurrentNativeModuleBuilder::Get()->AddConstantWriter(
          [ m, name = std::string(jsName),
            field ](const IJSValueWriter &argWriter) mutable noexcept {
            ::Microsoft::ReactNative::WriteProperty(argWriter, name, m->*field);
          });
    }
    return false;
  }
};

struct ModuleConstantInfo {
  template <class TValue>
  static bool
  Register(void *module, const char *jsName, TValue &&value) noexcept {
    using winrt::Microsoft::ReactNative::Bridge::IJSValueWriter;

    if (module) {
      CurrentNativeModuleBuilder::Get()->AddConstantWriter(
          [ name = std::string(jsName), value = std::forward<TValue>(value) ](
              const IJSValueWriter &argWriter) mutable noexcept {
            ::Microsoft::ReactNative::WriteProperty(argWriter, name, value);
          });
    }

    return false;
  }
};

template <class TField>
struct ModuleEventFieldInfo;

template <class TModule, template <class> class TFunc, class TArg>
struct ModuleEventFieldInfo<TFunc<void(TArg)> TModule::*> {
  using ModuleType = TModule;
  using EventType = TFunc<void(TArg)>;
  using FieldType = EventType TModule::*;

  static bool
  Register(TModule *module, const char *jsName, FieldType field) noexcept {
    using winrt::Microsoft::ReactNative::Bridge::RaiseEvent;
    using winrt::Microsoft::ReactNative::Bridge::IJSValueWriter;

    CurrentNativeModuleBuilder::Get()->AddEventRegister(
        winrt::to_hstring(jsName),
        [ module, field ](const RaiseEvent &raiseEvent) noexcept {
          module->*field = [raiseEvent](TArg arg) noexcept {
            raiseEvent([&](const IJSValueWriter &argWriter) noexcept {
              WriteValue(argWriter, arg);
            });
          };
        });

    return false;
  }
};

} // namespace Microsoft::ReactNative
