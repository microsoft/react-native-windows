// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "JSBundle.h"
#include "JSExceptionInfo.h"
#include "PropertyBag.h"
#include "dispatchQueue/dispatchQueue.h"
#include "errorCode/errorCode.h"
#include "future/future.h"

namespace Mso::React {

// Forward declarations
struct IReactInstance;
struct IReactRootView;
struct IReactViewHost;
struct ReactOptions;

enum class LogLevel : int32_t {
  Trace = 0,
  Info = 1,
  Warning = 2,
  Error = 3,
  Fatal = 4,
};

using OnErrorCallback = Mso::Functor<void(const Mso::ErrorCode &)>;
using OnLoggingCallback = Mso::Functor<void(LogLevel logLevel, const char *message)>;
using OnReactInstanceCreatedCallback = Mso::Functor<void(IReactInstance &)>;
using OnReactInstanceLoadedCallback = Mso::Functor<void(IReactInstance &, const Mso::ErrorCode &)>;
using OnReactInstanceDestroyedCallback = Mso::Functor<void(IReactInstance &)>;

//! Returns default OnError handler.
LIBLET_PUBLICAPI OnErrorCallback GetDefaultOnErrorHandler() noexcept;

//! PropertyBag named property for JavaScript dispatch queue.
constexpr const Mso::JSHost::NamedProperty<Mso::IDispatchQueueService> JSDispatchQueueProperty{"JSDispatchQueue"};

//! PropertyBag named property for Native dispatch queue.
constexpr const Mso::JSHost::NamedProperty<Mso::IDispatchQueueService> NativeDispatchQueueProperty{
    "NativeDispatchQueue"};

//! Every individual breaking behavior change on the platform should have its own quirk.  This way if
//! there is a breaking change that is a significant amount of work for an SDX to fix, they can maintain that
//! behavior without having to stop getting other new behaviors.
//! We can also add additional quirks based on what quirk version the SDX is running at.  (Maybe we need to do
//! something different for an SDX three versions behind, vs one version behind)
//! Quirks can be shared by the SDX platform, or host specific.
//! Quirks should be registered by calling RegisterQuirk.  Then they can be set on ReactOptions.
//! Generally a host specific version config should be used to set the quirks on ReactOptions, rather
//! then setting them manually.
using SDXQuirk = uint32_t;

/**An Office wrapper that extends FB's React Instance and makes it a 1:1 relationship with the bundle,
such that each Office React Instance is capable of loading a single bundle and hosting js in an
isolated JavaScript execution environment.*/
MSO_STRUCT_GUID(IReactInstance, "085D524A-AF3B-4839-8056-E5D0E6FC64BC")
struct IReactInstance : public IUnknown {
  //! Returns the name of the bundle file this React Instance has loaded.
  virtual const std::string &JsBundleName() const noexcept = 0;

  //! Returns the SDX base path from where this React Instance has loaded.
  virtual std::string SDXBasePath() const noexcept = 0;

  //! Returns a flag that indicates some properties of the existing instance has been changed (like using web debugger),
  //! and this instance should not be cached and served anymore.
  virtual bool NeedsReload() const noexcept = 0;

  //! Marks this instance as needing reload, due to a fundamental property change (like useWebDebugger).
  //! After the instance is marked, this instance will not be served through GetOrCreateReactInstance API anymore.
  virtual void SetAsNeedsReload() noexcept = 0;

  //! Returns if a quirk is set on this instance or not.
  virtual bool GetQuirk(SDXQuirk quirk) const noexcept = 0;

  //! Returns ReactOptions associated with the IReactInstance
  //! The ReactOptions are meant to immutable and give to IReactInstance at its creation.
  virtual const ReactOptions &Options() const noexcept = 0;
};

//! An isolated UI control in the UI tree that is not owned (parented) by another React View.
MSO_STRUCT_GUID(IReactRootView, "FF0FF21D-C760-48C2-98B9-E62DDAFE2BBF")
struct IReactRootView : public IUnknown {
  //! Returns the ReactInstance this rootView is part of.
  virtual Mso::CntPtr<IReactInstance> GetReactInstance() const noexcept = 0;

  //! Returns the name of the component. This name refers to the name of the component in the bundle
  //! that is loaded by the React Instance.
  virtual const std::string &JsComponentName() const noexcept = 0;

  //! Start the JS component and starts rendering it.
  virtual void Start() noexcept = 0;

  //! Stop the JS component.
  virtual void Stop() noexcept = 0;
};

//! Settings per each IReactViewHost associated with an IReactHost instance.
struct ReactViewOptions {
  //! Name of a component registered in JavaScript via AppRegistry.registerComponent('ModuleName', () => ModuleName);
  std::string ComponentName;

  //! Set of initial component properties. It is a JSON string.
  std::string InitialProps;
};

struct ReactDevOptions {
  //! When dev mode is enabled, extra developer features are available
  //! such as the dev menu, red box, etc.
  bool IsDevModeEnabled{false};

  //! Enables debugging in the JavaScript engine (if supported).
  bool UseDirectDebugger{false};

  //! For direct debugging, whether to break on the next line of JavaScript that is executed.
  bool DebuggerBreakOnNextLine{false};

  //! For direct debugging, allow specifying the port to use for the debugger.
  uint16_t DebuggerPort{0};

  //! For direct debugging, specifies a name to associate with the JavaScript runtime instance.
  std::string DebuggerRuntimeName;

  //! Enable live reload to load the source bundle from the React Native packager.
  //! When the file is saved, the packager will trigger reloading.
  bool UseLiveReload{false};

  //! Enables debugging using the web debugger. By default, this is Chrome using http://localhost:8081/debugger-ui from
  //! Metro/Haul. Debugging will start as soon as the react native instance is loaded.
  bool UseWebDebugger{false};

  //! URL used for debugging
  std::string DebugHost;

  //! When using web debugging and/or live reload, the source is obtained from the packager.
  //! The source url for the bundle is "http://{HOST}:{PORT}/{PATH}{EXTENSION}?platform=..."
  //! which defaults to:
  //! {HOST} = "localhost",
  //! {PORT} = "8081",
  //! {PATH} = "index.{PLATFORM}" where {PLATFORM} = "ios", "android", "win32", etc.
  //! {EXTENSION} = ".bundle"
  //! Specify a value for a component, or leave empty to use the default.
  std::string SourceBundleHost; // {HOST}
  std::string SourceBundlePort; // {PORT}
  std::string SourceBundlePath; // {PATH}
  std::string SourceBundleExtension; // {EXTENSION}

  //! Module name used for loading the debug bundle.
  //! e.g. The modules name registered in the jsbundle via AppRegistry.registerComponent('ModuleName', () =>
  //! ModuleName);
  std::string SourceBundleModuleName;
};

//! A simple struct that describes the basic properties/needs of an SDX. Whenever a new SDX is
//! getting hosted in React, properties here will be used to construct the SDX.
struct ReactOptions {
  //! Identity of the SDX. Must uniquely describe the SDX across the installed product.
  std::string Identity;

  //! List of names of Cxx modules that all ReactIntances hosting this SDX should be configured with.
  //!
  //! In addition to this list, there are following hardcoded modules.
  //!
  //! Cxx Modules Provided by ReactNativeHost:
  //! RekaBridge: Implemented by Mso::React::RekaNativeModule
  //!
  //! Cxx Modules Provided by ReactNative:
  //! WebSocketModule: Implemented by facebook::react::WebSocketModule
  //! Timing: Implemented by facebook::react::TimingModule
  //! SourceCode: Implemented by facebook::react::SourceCodeModule
  //! ExceptionsManager: Implemented by facebook::react::ExceptionsManagerModule
  std::vector<std::string> CxxModuleNames;

  //! List of names of Java modules that all ReactIntances hosting this SDX should be configured with.
  std::vector<std::string> JavaModuleNames;

  //! (Optional) Name of the registered IDataServiceProvider to use when creating data services
  //! for this SDX.  Providers are registered using RegisterDataServiceProvider().
  //! Or, specify the reserved name DefaultDataServiceProviderName to delegate to
  //! the DataServiceRegistration facility.
  std::string DataServiceProviderName;

  //! Base path of the SDX. The absolute path of the SDX can be constructed from this and the Identity.
  std::string SDXBasePath;

  //! Javascript Bundles
  //! This List includes both Platform and User Javascript Bundles
  //! Bundles are loaded into Javascript engine in the same order
  //! as they are specified in this list.
  std::vector<Mso::CntPtr<IJSBundle>> JSBundles;

  //! ReactNative Infrastructure Error
  //! Error types include:
  //! * Any call to Javascript function after Global Exception has been raised
  //! * Any WebServer error when DeveloperSettings.UseWebDebugger is true
  //! Note: Default callback generates ShipAssert.
  OnErrorCallback OnError{GetDefaultOnErrorHandler()};

  //! The callback is called when JavaScript code logs a message through the console.log class of APIs.
  //! Note: this is currently only used in Win32 (Chakra Executor)
  OnLoggingCallback OnLogging;

  //! Javascript Function Exception
  //! Callback called when an Exception is thrown inside JavaScript function,
  //! except the ones that happen during initialization.
  //! Javascript Exceptions which happen during initialization go through OnError callback.
  OnJSExceptionCallback OnJSException;

  //! Path used for localhost loading.
  //! e.g. for "index.ios" the RN runtime will create the url "http://localhost:8081/index.ios.bundle?platform=..."
  //! If not set then localhost loading is not attempted.
  std::string LocalHostPath; // DEPRECATED: use DeveloperSettings.SourceBundlePath

  //! Module name used for localhost loading.
  //! e.g. The modules name registered in the jsbundle via AppRegistry.registerComponent('ModuleName', () =>
  //! ModuleName);
  std::string ModuleName; // DEPRECATED: use DeveloperSettings.SourceBundleModuleName

  //! Enable dev mode.
  //! development tools such as the debug executors, dev menu, red box, etc. are available
  bool IsDevModeEnabled{false}; // DEPRECATED: use DeveloperSettings.IsDevModeEnabled

  //! Turns on remote debugging.  Remote debugging will start as soon
  //! as the react native instance is loaded.
  bool UseWebDebugger{false}; // DEPRECATED: use DeveloperSettings.UseWebDebugger

  //! Flag to suggest sdx owner's preference on enabling Bytecode caching in Javascript Engine for corresponding SDX.
  bool EnableBytecode{true};

  //! Flag controlling whether the JavaScript engine uses JIT compilation.
  bool EnableJITCompilation{true};

  //! Enable function nativePerformanceNow.
  //! Method nativePerformanceNow() returns high resolution time info.
  //! It is not safe to expose to Custom Function. Add this flag so we can turn it off for Custom Function.
  bool EnableNativePerformanceNow{true};

  //! This lets us have a mechanism to keep existing SDX behavior in places where we want to change the behavior
  std::unordered_set<SDXQuirk> Quirks;

  ReactDevOptions DeveloperSettings = {};

  //! Additional properties associated with the ReactOptions.
  Mso::JSHost::PropertyBag Properties;

  //! Adds registered JS bundle to JSBundles.
  LIBLET_PUBLICAPI ReactOptions &AddRegisteredJSBundle(std::string_view jsBundleId) noexcept;

  //! Adds JS bundle based on a file name to JSBundles.
  LIBLET_PUBLICAPI ReactOptions &AddFileJSBundle(std::string_view jsBundleId, std::string_view fileName) noexcept;

  //! The callback is called when IReactInstance is created, and JS bundles are started to load.
  //! It is called from the native queue.
  OnReactInstanceCreatedCallback OnInstanceCreated;

  //! The callback is called when IReactInstance finished loading JS bundles.
  //! If there were errors, then the provided Mso::ErrorCode is not empty.
  //! The callback is always called regardless if JSBundles were loaded or not.
  //! This callback is called from the native queue.
  //!
  //! Error types include:
  //! * Javascript Syntax Error
  //! * Global Javascript Exception Thrown
  //!    Including Javascript Exceptions during initialization
  OnReactInstanceLoadedCallback OnInstanceLoaded;

  //! The callback is called when IReactInstance is destroyed and must not be used anymore.
  //! It is called from the native queue.
  OnReactInstanceDestroyedCallback OnInstanceDestroyed;
};

//! IReactHost manages a RactNative instance.
MSO_STRUCT_GUID(IReactHost, "2a2474ff-264f-449a-9852-0463e6ac6bbf")
struct DECLSPEC_NOVTABLE IReactHost : IUnknown {
  //! Returns a copy of react options.
  virtual ReactOptions Options() const noexcept = 0;

  //! Returns current ReactInstance. The result can be null if instance is unloaded.
  virtual Mso::CntPtr<IReactInstance> Instance() const noexcept = 0;

  //! A native sequential queue associated with IReactHost.
  //! All internal operations are done using this queue to synchronize internal state.
  virtual Mso::DispatchQueue const &NativeQueue() const noexcept = 0;

  //! Reloads the ReactNative instance and associated ReactViews.
  virtual Mso::Future<void> ReloadInstance() noexcept = 0;

  //! Updates ReactOptions associated with the IReactHost and reloads the ReactNative instance.
  virtual Mso::Future<void> ReloadInstanceWithOptions(ReactOptions &&options) noexcept = 0;

  //! Unloads the ReactNative instance and associated ReactViews.
  virtual Mso::Future<void> UnloadInstance() noexcept = 0;

  //! Creates a new instance of IReactViewHost.
  //! The IReactViewHost is added to the list of view hosts only after a IReactViewInstance is attached to it.
  virtual Mso::CntPtr<IReactViewHost> MakeViewHost(ReactViewOptions &&options) noexcept = 0;

  //! Returns a snapshot of IReactViewHost instances associated with the IReactHost.
  virtual Mso::Future<std::vector<Mso::CntPtr<IReactViewHost>>> GetViewHostList() noexcept = 0;
};

//! Each platform has a ReactViewInstance where it hosts the React UI tree.
//! This ReactViewInstance implements the IReactViewInstance interface.
//! When the ReactViewInstance is created, it is given a IReactViewHost instance.
//! The IReactViewInstance should attach itself to the IReactViewHost instance at creation time, and then
//! detach from it when destroyed.
//! After the IReactViewInstance is attached to the IReactViewHost, the IReactViewHost is attached to its
//! IReactHost. The IReactViewHost is detached from IReactHost after when the IReactViewInstance is detached.
//! The IReactViewHost has a set of Options associated with it that are used by IReactViewInstance to create
//! the React UI tree.
//! The IReactViewHost is responsible for calling IReactViewInstance Reload() and Unload() methods to
//! reload and unload the UI tree.
//! The IReactViewHost also provides an access to IReactHost associated with it.

//! This interface is to be implemented by a platform specific ReactViewInstance that hosts React UI tree.
MSO_STRUCT_GUID(IReactViewInstance, "29e04f14-9fc9-4dd7-a543-e59db0d57bd2")
struct IReactViewInstance : IUnknown {
  //! Reloads React UI tree. We pass reactInstance and viewOptions that essentially represent
  //! a snapshot of current options.
  virtual Mso::Future<void> Reload(
      Mso::CntPtr<IReactInstance> &&reactInstance,
      ReactViewOptions &&viewOptions) noexcept = 0;

  //! Unloads React UI tree.
  virtual Mso::Future<void> Unload() noexcept = 0;
};

//! A host object for ReactView.
//! Each IReactViewHost has association with its parent IReactHost.
//! IReactHost has a weak reference to IReactViewHost when the IReactViewHost is bound to IReactViewInstance.
//! Typically platform-specific implementation of IReactViewInstance is created using
//! IReactViewHost instance options, and then attaches itself to the IReactViewHost.
//! When IReactViewInstance is destroyed, it detaches itself from the IReactViewHost.
MSO_STRUCT_GUID(IReactViewHost, "cc9f0c95-ad43-43af-a834-0ef9adc411c4")
struct IReactViewHost : IUnknown {
  //! Returns a copy of current react view options.
  virtual ReactViewOptions Options() const noexcept = 0;

  //! Returns IReactHost associated with this IReactViewHost.
  virtual IReactHost &ReactHost() const noexcept = 0;

  //! Reloads the IReactViewInstance if it is attached.
  virtual Mso::Future<void> ReloadViewInstance() noexcept = 0;

  //! Reloads IReactViewInstance if it is attached with a new set of options.
  virtual Mso::Future<void> ReloadViewInstanceWithOptions(ReactViewOptions &&options) noexcept = 0;

  //! Unloads the attached IReactViewInstance.
  virtual Mso::Future<void> UnloadViewInstance() noexcept = 0;

  //! Attaches IReactViewInstance to the IReactViewHost.
  virtual Mso::Future<void> AttachViewInstance(IReactViewInstance &viewInstance) noexcept = 0;

  //! Detaches IReactViewInstance from the IReactViewHost.
  virtual Mso::Future<void> DetachViewInstance() noexcept = 0;
};

LIBLET_PUBLICAPI Mso::CntPtr<IReactHost> MakeReactHost() noexcept;

} // namespace Mso::React
