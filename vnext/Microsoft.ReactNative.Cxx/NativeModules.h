// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "winrt/Microsoft.ReactNative.h"

#include <type_traits>
#include "JSValueReader.h"
#include "JSValueWriter.h"
#include "ModuleMemberRegistration.h"
#include "ModuleRegistration.h"
#include "ReactPromise.h"

// The macro to annotate a C++ class as a ReactNative module.
//
// Arguments:
// - moduleClass (required) - the class name the macro is attached to.
// - moduleName (optional) - the module name visible to JavaScript. Default is
//     the moduleClass name.
// - eventEmitterName (optional) - the event emitter name used in JavaScript.
//     Default is the RCTDeviceEventEmitter.
#define REACT_MODULE(/* moduleClass, [opt] moduleName, [opt] eventEmitterName */...) \
  INTERNAL_REACT_MODULE(__VA_ARGS__)(__VA_ARGS__)

// REACT_METHOD annotates a method to export to JavaScript.
// It declares an asynchronous method. To return a value:
// - Return void and have a Callback as a last parameter. The Callback type can
// be any std::function like type. I.e. Func<void(Args...)>.
// - Return void and have two callbacks as last parameters. One is used to return
// value and another an error.
// - Return void and have a ReactPromise as a last parameter. In JavaScript the
// method returns Promise.
// - Return non-void value. In JavaScript it is treated as a method with one
// Callback. Return std::pair<Error, Value> to be able to communicate error
// condition.
//
// Arguments:
// - method (required) - the method name the macro is attached to.
// - methodName (optional) - the method name visible to JavaScript. Default is
//     the method name.
#define REACT_METHOD(/* method, [opt] methodName */...) INTERNAL_REACT_METHOD(__VA_ARGS__)(, __VA_ARGS__)

// A method that is called synchronously. It must be used rarely because it may
// cause out-of-order execution when used along with asynchronous methods.
#define REACT_SYNC_METHOD(/* method, [opt] methodName */...) INTERNAL_REACT_METHOD(__VA_ARGS__)(Sync, __VA_ARGS__)

// A method where we can define constants.
#define REACT_CONSTANT_PROVIDER(method) INTERNAL_REACT_METHOD_3_ARGS(Const, method, L"")

// Use with a field to provide a constant
#define REACT_CONSTANT(/* field, [opt] constantName */...) INTERNAL_REACT_CONSTANT(__VA_ARGS__)(__VA_ARGS__)

// Use with a field for events
#define REACT_EVENT(/* field, [opt] eventName */...) INTERNAL_REACT_EVENT(__VA_ARGS__)(__VA_ARGS__)

namespace winrt::Microsoft::ReactNative {

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
  constexpr static size_t Value = IsCallback<std::remove_const_t<std::remove_reference_t<TArg>>>::value ? 1 : 0;
};

template <class TArg0, class TArg1, class... TArgs>
struct GetCallbackCount<std::tuple<TArg0, TArg1, TArgs...>> {
  using TupleType = std::tuple<TArg0, TArg1, TArgs...>;
  constexpr static size_t TupleSize = std::tuple_size_v<TupleType>;
  constexpr static size_t Value =
      (IsCallback<std::remove_const_t<std::remove_reference_t<std::tuple_element_t<TupleSize - 2, TupleType>>>>::value
           ? 1
           : 0) +
      (IsCallback<std::remove_const_t<std::remove_reference_t<std::tuple_element_t<TupleSize - 1, TupleType>>>>::value
           ? 1
           : 0);
};

template <class T>
struct IsPromise : std::false_type {};
template <class T>
struct IsPromise<ReactPromise<T>> : std::true_type {};

} // namespace Internal

//==============================================================================
// Module registration helpers
//==============================================================================

template <class TFunc>
struct ModuleMethodInfo;

// Instance asynchronous method
template <class TModule, class TResult, class... TArgs>
struct ModuleMethodInfo<TResult (TModule::*)(TArgs...) noexcept> {
  constexpr static bool HasPromise() noexcept {
    if constexpr (sizeof...(TArgs) > 0) {
      return Internal::IsPromise<std::remove_const_t<std::remove_reference_t<
          std::tuple_element_t<sizeof...(TArgs) - 1, std::tuple<std::remove_reference_t<TArgs>...>>>>>::value;
    }
    return false;
  }

  constexpr static size_t CallbackCount =
      Internal::GetCallbackCount<std::tuple<std::remove_reference_t<TArgs>...>>::Value;
  using ModuleType = TModule;
  using MethodType = TResult (TModule::*)(TArgs...) noexcept;
  using IndexSequence = std::make_index_sequence<sizeof...(TArgs) - (HasPromise() ? 1 : CallbackCount)>;

  template <class>
  struct Invoker;

  template <size_t... I>
  struct Invoker<std::index_sequence<I...>> {
    // Fire and forget method
    static MethodDelegate GetFunc(ModuleType *module, MethodType method, std::integral_constant<size_t, 0>) noexcept {
      return [ module, method ](
          IJSValueReader const &argReader,
          IJSValueWriter const & /*argWriter*/,
          MethodResultCallback const &,
          MethodResultCallback const &) mutable noexcept {
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
        return TCallback([ callback = std::move(callback), argWriter ](TArgs... args) noexcept {
          WriteArgs(argWriter, std::move(args)...);
          callback(argWriter);
        });
      }
    };

    template <class T, class TDummy>
    struct RejectCallbackCreator;

    template <template <class...> class TCallback, class TArg>
    struct RejectCallbackCreator<
        TCallback<void(TArg)>,
        std::enable_if_t<std::is_assignable_v<std::string, TArg> || std::is_assignable_v<std::wstring, TArg>>> {
      static TCallback<void(TArg)> Create(
          const IJSValueWriter &argWriter,
          const MethodResultCallback &callback) noexcept {
        return TCallback([ callback = std::move(callback), argWriter ](TArg arg) noexcept {
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
    struct RejectCallbackCreator<TCallback<void()>, void> : CallbackCreator<TCallback<void()>> {};

    template <template <class...> class TCallback, class TArg>
    struct RejectCallbackCreator<
        TCallback<void(TArg)>,
        std::enable_if_t<!std::is_assignable_v<std::string, TArg> && !std::is_assignable_v<std::wstring, TArg>>>
        : CallbackCreator<TCallback<void(TArg)>> {};

    template <template <class...> class TCallback, class TArg0, class TArg1, class... TArgs>
    struct RejectCallbackCreator<TCallback<void(TArg0, TArg1, TArgs...)>, void>
        : CallbackCreator<TCallback<void(TArg0, TArg1, TArgs...)>> {};

    template <class T>
    struct PromiseCreator;

    template <class T>
    struct PromiseCreator<ReactPromise<T>> {
      static ReactPromise<T> Create(
          const IJSValueWriter &argWriter,
          const MethodResultCallback &resolve,
          const MethodResultCallback &reject) noexcept {
        return ReactPromise<T>(argWriter, resolve, reject);
      }
    };

    // Method with one callback
    static MethodDelegate GetFunc(ModuleType *module, MethodType method, std::integral_constant<size_t, 1>) noexcept {
      return [ module, method ](
          IJSValueReader const &argReader,
          IJSValueWriter const &argWriter,
          MethodResultCallback const &callback,
          MethodResultCallback const &) mutable noexcept {
        using ArgTuple = std::tuple<std::remove_reference_t<TArgs>...>;
        ArgTuple typedArgs{};
        ReadArgs(argReader, std::get<I>(typedArgs)...);
        auto cb = CallbackCreator<std::remove_const_t<std::remove_reference_t<
            std::tuple_element_t<sizeof...(TArgs) - 1, ArgTuple>>>>::Create(argWriter, callback);
        (module->*method)(std::get<I>(std::move(typedArgs))..., std::move(cb));
      };
    }

    // Method with two callbacks
    static MethodDelegate GetFunc(ModuleType *module, MethodType method, std::integral_constant<size_t, 2>) noexcept {
      return [ module, method ](
          IJSValueReader const &argReader,
          IJSValueWriter const &argWriter,
          MethodResultCallback const &resolve,
          MethodResultCallback const &reject) mutable noexcept {
        using ArgTuple = std::tuple<std::remove_reference_t<TArgs>...>;
        ArgTuple typedArgs{};
        ReadArgs(argReader, std::get<I>(typedArgs)...);
        auto resolveCallback = CallbackCreator<std::remove_const_t<
            std::remove_reference_t<std::tuple_element_t<sizeof...(TArgs) - 2, ArgTuple>>>>::Create(argWriter, resolve);
        auto rejectCallback = RejectCallbackCreator<
            std::remove_const_t<std::remove_reference_t<std::tuple_element_t<sizeof...(TArgs) - 1, ArgTuple>>>,
            void>::Create(argWriter, reject);
        (module->*method)(std::get<I>(std::move(typedArgs))..., std::move(resolveCallback), std::move(rejectCallback));
      };
    }

    // Method with Promise
    static MethodDelegate GetFunc(ModuleType *module, MethodType method, std::integral_constant<size_t, 3>) noexcept {
      return [ module, method ](
          IJSValueReader const &argReader,
          IJSValueWriter const &argWriter,
          MethodResultCallback const &resolve,
          MethodResultCallback const &reject) mutable noexcept {
        using AllArgsTuple = std::tuple<std::remove_reference_t<TArgs>...>;
        using ArgsTuple = std::tuple<std::tuple_element_t<I, AllArgsTuple>...>;
        using PromiseArg = std::remove_const_t<std::tuple_element_t<sizeof...(TArgs) - 1, AllArgsTuple>>;
        ArgsTuple typedArgs{};
        ReadArgs(argReader, std::get<I>(typedArgs)...);
        auto promise = PromiseCreator<PromiseArg>::Create(argWriter, resolve, reject);
        (module->*method)(std::get<I>(std::move(typedArgs))..., promise);
      };
    }

    // Async method with return value
    static MethodDelegate GetFunc(ModuleType *module, MethodType method, std::integral_constant<size_t, 4>) noexcept {
      return [ module, method ](
          IJSValueReader const &argReader,
          IJSValueWriter const &argWriter,
          MethodResultCallback const &callback,
          MethodResultCallback const &) mutable noexcept {
        using ArgTuple = std::tuple<std::remove_reference_t<TArgs>...>;
        ArgTuple typedArgs{};
        ReadArgs(argReader, std::get<I>(typedArgs)...);
        TResult result = (module->*method)(std::get<I>(std::move(typedArgs))...);
        WriteArgs(argWriter, result);
        callback(argWriter);
      };
    }
  };

  static MethodDelegate GetMethodDelegate(void *module, MethodType method, MethodReturnType &returnType) noexcept {
    constexpr bool isVoidResult = std::is_void_v<TResult> || std::is_same_v<TResult, winrt::fire_and_forget>;
    if constexpr (!isVoidResult) {
      returnType = MethodReturnType::Callback;
    } else if constexpr (HasPromise()) {
      returnType = MethodReturnType::Promise;
    } else if constexpr (CallbackCount == 2) {
      returnType = MethodReturnType::TwoCallbacks;
    } else if constexpr (CallbackCount == 1) {
      returnType = MethodReturnType::Callback;
    } else {
      returnType = MethodReturnType::Void;
    }

    constexpr int selector = !isVoidResult ? 4 : HasPromise() ? 3 : CallbackCount;
    return Invoker<IndexSequence>::GetFunc(
        static_cast<ModuleType *>(module), method, std::integral_constant<size_t, selector>{});
  }
};

// Static asynchronous method
template <class TResult, class... TArgs>
struct ModuleMethodInfo<TResult (*)(TArgs...) noexcept> {
  constexpr static bool HasPromise() noexcept {
    if constexpr (sizeof...(TArgs) > 0) {
      return Internal::IsPromise<std::remove_const_t<std::remove_reference_t<
          std::tuple_element_t<sizeof...(TArgs) - 1, std::tuple<std::remove_reference_t<TArgs>...>>>>>::value;
    }
    return false;
  }

  constexpr static size_t CallbackCount =
      Internal::GetCallbackCount<std::tuple<std::remove_reference_t<TArgs>...>>::Value;
  using MethodType = TResult (*)(TArgs...) noexcept;
  using IndexSequence = std::make_index_sequence<sizeof...(TArgs) - (HasPromise() ? 1 : CallbackCount)>;

  template <class>
  struct Invoker;

  template <size_t... I>
  struct Invoker<std::index_sequence<I...>> {
    // Fire and forget method
    static MethodDelegate GetFunc(MethodType method, std::integral_constant<size_t, 0>) noexcept {
      return [method](
          IJSValueReader const &argReader,
          IJSValueWriter const & /*argWriter*/,
          MethodResultCallback const &,
          MethodResultCallback const &) mutable noexcept {
        std::tuple<std::remove_reference_t<TArgs>...> typedArgs{};
        ReadArgs(argReader, std::get<I>(typedArgs)...);
        (*method)(std::get<I>(std::move(typedArgs))...);
      };
    }

    template <class T>
    struct CallbackCreator;

    template <template <class...> class TCallback, class... TArgs>
    struct CallbackCreator<TCallback<void(TArgs...)>> {
      static TCallback<void(TArgs...)> Create(
          const IJSValueWriter &argWriter,
          const MethodResultCallback &callback) noexcept {
        return TCallback([ callback = std::move(callback), argWriter ](TArgs... args) noexcept {
          WriteArgs(argWriter, std::move(args)...);
          callback(argWriter);
        });
      }
    };

    template <class T, class TDummy>
    struct RejectCallbackCreator;

    template <template <class...> class TCallback, class TArg>
    struct RejectCallbackCreator<
        TCallback<void(TArg)>,
        std::enable_if_t<std::is_assignable_v<std::string, TArg> || std::is_assignable_v<std::wstring, TArg>>> {
      static TCallback<void(TArg)> Create(
          const IJSValueWriter &argWriter,
          const MethodResultCallback &callback) noexcept {
        return TCallback([ callback = std::move(callback), argWriter ](TArg arg) noexcept {
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
    struct RejectCallbackCreator<TCallback<void()>, void> : CallbackCreator<TCallback<void()>> {};

    template <template <class...> class TCallback, class TArg>
    struct RejectCallbackCreator<
        TCallback<void(TArg)>,
        std::enable_if_t<!std::is_assignable_v<std::string, TArg> && !std::is_assignable_v<std::wstring, TArg>>>
        : CallbackCreator<TCallback<void(TArg)>> {};

    template <template <class...> class TCallback, class TArg0, class TArg1, class... TArgs>
    struct RejectCallbackCreator<TCallback<void(TArg0, TArg1, TArgs...)>, void>
        : CallbackCreator<TCallback<void(TArg0, TArg1, TArgs...)>> {};

    template <class T>
    struct PromiseCreator;

    template <class T>
    struct PromiseCreator<ReactPromise<T>> {
      static ReactPromise<T> Create(
          const IJSValueWriter &argWriter,
          const MethodResultCallback &resolve,
          const MethodResultCallback &reject) noexcept {
        return ReactPromise<T>(argWriter, resolve, reject);
      }
    };

    // Method with one callback
    static MethodDelegate GetFunc(MethodType method, std::integral_constant<size_t, 1>) noexcept {
      return [method](
          IJSValueReader const &argReader,
          IJSValueWriter const &argWriter,
          MethodResultCallback const &callback,
          MethodResultCallback const &) mutable noexcept {
        using ArgTuple = std::tuple<std::remove_reference_t<TArgs>...>;
        ArgTuple typedArgs{};
        ReadArgs(argReader, std::get<I>(typedArgs)...);
        auto cb = CallbackCreator<std::remove_const_t<std::remove_reference_t<
            std::tuple_element_t<sizeof...(TArgs) - 1, ArgTuple>>>>::Create(argWriter, callback);
        (*method)(std::get<I>(std::move(typedArgs))..., std::move(cb));
      };
    }

    // Method with two callbacks
    static MethodDelegate GetFunc(MethodType method, std::integral_constant<size_t, 2>) noexcept {
      return [method](
          IJSValueReader const &argReader,
          IJSValueWriter const &argWriter,
          MethodResultCallback const &resolve,
          MethodResultCallback const &reject) mutable noexcept {
        using ArgTuple = std::tuple<std::remove_reference_t<TArgs>...>;
        ArgTuple typedArgs{};
        ReadArgs(argReader, std::get<I>(typedArgs)...);
        auto resolveCallback = CallbackCreator<std::remove_const_t<
            std::remove_reference_t<std::tuple_element_t<sizeof...(TArgs) - 2, ArgTuple>>>>::Create(argWriter, resolve);
        auto rejectCallback = RejectCallbackCreator<
            std::remove_const_t<std::remove_reference_t<std::tuple_element_t<sizeof...(TArgs) - 1, ArgTuple>>>,
            void>::Create(argWriter, reject);
        (*method)(std::get<I>(std::move(typedArgs))..., std::move(resolveCallback), std::move(rejectCallback));
      };
    }

    // Method with Promise
    static MethodDelegate GetFunc(MethodType method, std::integral_constant<size_t, 3>) noexcept {
      return [method](
          IJSValueReader const &argReader,
          IJSValueWriter const &argWriter,
          MethodResultCallback const &resolve,
          MethodResultCallback const &reject) mutable noexcept {
        using AllArgsTuple = std::tuple<std::remove_reference_t<TArgs>...>;
        using ArgsTuple = std::tuple<std::tuple_element_t<I, AllArgsTuple>...>;
        using PromiseArg = std::remove_const_t<std::tuple_element_t<sizeof...(TArgs) - 1, AllArgsTuple>>;
        ArgsTuple typedArgs{};
        ReadArgs(argReader, std::get<I>(typedArgs)...);
        auto promise = PromiseCreator<PromiseArg>::Create(argWriter, resolve, reject);
        (*method)(std::get<I>(std::move(typedArgs))..., std::move(promise));
      };
    }

    // Async method with return value
    static MethodDelegate GetFunc(MethodType method, std::integral_constant<size_t, 4>) noexcept {
      return [method](
          IJSValueReader const &argReader,
          IJSValueWriter const &argWriter,
          MethodResultCallback const &callback,
          MethodResultCallback const &) mutable noexcept {
        using ArgTuple = std::tuple<std::remove_reference_t<TArgs>...>;
        ArgTuple typedArgs{};
        ReadArgs(argReader, std::get<I>(typedArgs)...);
        TResult result = (*method)(std::get<I>(typedArgs)...);
        WriteArgs(argWriter, result);
        callback(argWriter);
      };
    }
  };

  static MethodDelegate GetMethodDelegate(void * /*module*/, MethodType method, MethodReturnType &returnType) noexcept {
    constexpr bool isVoidResult = std::is_void_v<TResult> || std::is_same_v<TResult, winrt::fire_and_forget>;
    if constexpr (!isVoidResult) {
      returnType = MethodReturnType::Callback;
    } else if constexpr (HasPromise()) {
      returnType = MethodReturnType::Promise;
    } else if constexpr (CallbackCount == 2) {
      returnType = MethodReturnType::TwoCallbacks;
    } else if constexpr (CallbackCount == 1) {
      returnType = MethodReturnType::Callback;
    } else {
      returnType = MethodReturnType::Void;
    }

    constexpr int selector = !isVoidResult ? 4 : HasPromise() ? 3 : CallbackCount;
    return Invoker<IndexSequence>::GetFunc(method, std::integral_constant<size_t, selector>{});
  }
};

template <class TFunc>
struct ModuleSyncMethodInfo;

// Instance synchronous method
template <class TModule, class TResult, class... TArgs>
struct ModuleSyncMethodInfo<TResult (TModule::*)(TArgs...) noexcept> {
  using ModuleType = TModule;
  using MethodType = TResult (TModule::*)(TArgs...) noexcept;
  using IndexSequence = std::make_index_sequence<sizeof...(TArgs)>;

  template <class>
  struct Invoker;

  template <size_t... I>
  struct Invoker<std::index_sequence<I...>> {
    static SyncMethodDelegate GetFunc(ModuleType *module, MethodType method) noexcept {
      return [ module, method ](IJSValueReader const &argReader, IJSValueWriter const &argWriter) mutable noexcept {
        using ArgTuple = std::tuple<std::remove_reference_t<TArgs>...>;
        ArgTuple typedArgs{};
        ReadArgs(argReader, std::get<I>(typedArgs)...);
        TResult result = (module->*method)(std::get<I>(std::move(typedArgs))...);
        WriteArgs(argWriter, result);
      };
    }
  };

  static SyncMethodDelegate GetMethodDelegate(void *module, MethodType method) noexcept {
    return Invoker<IndexSequence>::GetFunc(static_cast<ModuleType *>(module), method);
  }
};

// Static synchronous method
template <class TResult, class... TArgs>
struct ModuleSyncMethodInfo<TResult (*)(TArgs...) noexcept> {
  using MethodType = TResult (*)(TArgs...) noexcept;
  using IndexSequence = std::make_index_sequence<sizeof...(TArgs)>;

  template <class>
  struct Invoker;

  template <size_t... I>
  struct Invoker<std::index_sequence<I...>> {
    static SyncMethodDelegate GetFunc(MethodType method) noexcept {
      return [method](IJSValueReader const &argReader, IJSValueWriter const &argWriter) mutable noexcept {
        using ArgTuple = std::tuple<std::remove_reference_t<TArgs>...>;
        ArgTuple typedArgs{};
        ReadArgs(argReader, std::get<I>(typedArgs)...);
        TResult result = (*method)(std::get<I>(std::move(typedArgs))...);
        WriteArgs(argWriter, result);
      };
    }
  };

  static SyncMethodDelegate GetMethodDelegate(void * /*module*/, MethodType method) noexcept {
    return Invoker<IndexSequence>::GetFunc(method);
  }
};

template <class TField>
struct ModuleConstFieldInfo;

template <class TModule, class TValue>
struct ModuleConstFieldInfo<TValue TModule::*> {
  using ModuleType = TModule;
  using FieldType = TValue TModule::*;

  static ConstantProvider GetConstantProvider(void *module, wchar_t const *name, FieldType field) noexcept {
    return [ module = static_cast<ModuleType *>(module), name = std::wstring{name}, field ](
        IJSValueWriter const &argWriter) mutable noexcept {
      WriteProperty(argWriter, name, module->*field);
    };
  }
};

template <class TValue>
struct ModuleConstFieldInfo<TValue *> {
  using FieldType = TValue *;

  static ConstantProvider GetConstantProvider(void * /*module*/, wchar_t const *name, FieldType field) noexcept {
    return [ name = std::wstring{name}, field ](IJSValueWriter const &argWriter) mutable noexcept {
      WriteProperty(argWriter, name, *field);
    };
  }
};

struct ReactConstantProvider {
  ReactConstantProvider(IJSValueWriter const &writer) noexcept : m_writer{writer} {}

  template <class T>
  void Add(const wchar_t *name, const T &value) noexcept {
    WriteProperty(m_writer, name, value);
  }

 private:
  IJSValueWriter m_writer;
};

template <class TMethod>
struct ModuleConstantInfo;

template <class TModule>
struct ModuleConstantInfo<void (TModule::*)(ReactConstantProvider &) noexcept> {
  using ModuleType = TModule;
  using MethodType = void (TModule::*)(ReactConstantProvider &) noexcept;

  static ConstantProvider GetConstantProvider(void *module, MethodType method) noexcept {
    return [ module = static_cast<ModuleType *>(module), method ](IJSValueWriter const &argWriter) mutable noexcept {
      ReactConstantProvider constantProvider{argWriter};
      (module->*method)(constantProvider);
    };
  }
};

template <>
struct ModuleConstantInfo<void (*)(ReactConstantProvider &) noexcept> {
  using MethodType = void (*)(ReactConstantProvider &) noexcept;

  static ConstantProvider GetConstantProvider(void * /*module*/, MethodType method) noexcept {
    return [method](IJSValueWriter const &argWriter) mutable noexcept {
      ReactConstantProvider constantProvider{argWriter};
      (*method)(constantProvider);
    };
  }
};

template <class TField>
struct ModuleEventFieldInfo;

template <class TModule, template <class> class TFunc, class TArg>
struct ModuleEventFieldInfo<TFunc<void(TArg)> TModule::*> {
  using ModuleType = TModule;
  using EventType = TFunc<void(TArg)>;
  using FieldType = EventType TModule::*;

  static ReactEventHandlerSetter GetEventHandlerSetter(void *module, FieldType field) noexcept {
    return [ module = static_cast<ModuleType *>(module), field ](const ReactEventHandler &eventHandler) noexcept {
      module->*field = [eventHandler](TArg arg) noexcept {
        eventHandler([&](const IJSValueWriter &argWriter) noexcept { WriteValue(argWriter, arg); });
      };
    };
  }
};

struct ReactModuleBuilder {
  ReactModuleBuilder(void *module, IReactModuleBuilder const &moduleBuilder) noexcept
      : m_module{module}, m_moduleBuilder{moduleBuilder} {}

  template <class TModule, int I>
  void RegisterModule(const wchar_t * /*moduleName*/, const wchar_t *eventEmitterName, ReactMemberId<I>) noexcept {
    m_moduleBuilder.SetEventEmitterName(eventEmitterName);
    RegisterMembers<TModule, I>();
  }

  template <class TClass, int I>
  auto HasRegisterMember(ReactModuleBuilder &builder, ReactMemberId<I> id)
      -> decltype(TClass::template RegisterMember<TClass>(builder, id), std::true_type{});
  template <class TClass>
  auto HasRegisterMember(...) -> std::false_type;

  template <class TModule, int I>
  void RegisterMembers() noexcept {
    if constexpr (decltype(HasRegisterMember<TModule>(*this, ReactMemberId<I + 1>{}))::value) {
      TModule::template RegisterMember<TModule>(*this, ReactMemberId<I + 1>{});
      RegisterMembers<TModule, I + 1>();
    }
  }

  template <class TClass, class TMethod>
  void RegisterMethod(TMethod method, wchar_t const *name) noexcept {
    MethodReturnType returnType;
    auto methodDelegate = ModuleMethodInfo<TMethod>::GetMethodDelegate(m_module, method, /*out*/ returnType);
    m_moduleBuilder.AddMethod(name, returnType, methodDelegate);
  }

  template <class TClass, class TMethod>
  void RegisterSyncMethod(TMethod method, wchar_t const *name) noexcept {
    auto syncMethodDelegate = ModuleSyncMethodInfo<TMethod>::GetMethodDelegate(m_module, method);
    m_moduleBuilder.AddSyncMethod(name, syncMethodDelegate);
  }

  template <class TClass, class TMethod>
  void RegisterConstMethod(TMethod method, wchar_t const * /*name*/) noexcept {
    auto constantProvider = ModuleConstantInfo<TMethod>::GetConstantProvider(m_module, method);
    m_moduleBuilder.AddConstantProvider(constantProvider);
  }

  template <class TClass, class TField>
  void RegisterConstant(TField field, wchar_t const *name) noexcept {
    auto constantProvider = ModuleConstFieldInfo<TField>::GetConstantProvider(m_module, name, field);
    m_moduleBuilder.AddConstantProvider(constantProvider);
  }

  template <class TClass, class TField>
  void RegisterEvent(TField field, wchar_t const *name) noexcept {
    auto eventHandlerSetter = ModuleEventFieldInfo<TField>::GetEventHandlerSetter(m_module, field);
    m_moduleBuilder.AddEventHandlerSetter(name, eventHandlerSetter);
  }

 private:
  void *m_module;
  IReactModuleBuilder m_moduleBuilder;
};

template <class T>
struct BoxedValue : implements<BoxedValue<T>, IBoxedValue> {
  BoxedValue() noexcept {}

  int64_t GetPtr() noexcept {
    return reinterpret_cast<int64_t>(&m_value);
  }

  static T &GetImpl(IBoxedValue &module) noexcept;

 private:
  T m_value{};
};

template <class T>
inline T &BoxedValue<T>::GetImpl(IBoxedValue &module) noexcept {
  return *reinterpret_cast<T *>(module.GetPtr());
}

template <class TModule>
inline ReactModuleProvider MakeModuleProvider() noexcept {
  return [](IReactModuleBuilder const &moduleBuilder) noexcept {
    auto moduleObject = make<BoxedValue<TModule>>();
    auto module = &BoxedValue<TModule>::GetImpl(moduleObject);
    ReactModuleBuilder builder{module, moduleBuilder};
    RegisterModule(builder, module);
    return moduleObject;
  };
}

} // namespace winrt::Microsoft::ReactNative
