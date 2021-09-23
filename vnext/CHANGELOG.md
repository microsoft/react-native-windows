# Change Log - react-native-windows

This log was last generated on Wed, 22 Sep 2021 21:25:30 GMT and should not be manually modified.

<!-- Start content -->

## 0.63.38

Wed, 22 Sep 2021 21:25:30 GMT

### Patches

- Change the definitino of YGUndefined from NAN (contra the comment in YGValue, MSVC does define NAN) to __builtin_nanf("0"), which generates a quiet NaN. The problem with NAN is that corecrt_math.h defines it as: (hpratt@microsoft.com)

## 0.63.37

Thu, 26 Aug 2021 14:12:28 GMT

### Patches

- [0.63] Consume Node-Api V8 JSI Runtime (julio.rocha@microsoft.com)

## 0.63.36

Wed, 30 Jun 2021 23:23:14 GMT

### Patches

- Use V8 by default for desktop (if enabled) (tudor.mihai@microsoft.com)

## 0.63.35

Tue, 22 Jun 2021 22:30:55 GMT

### Patches

- Have script store return nullptr on facebook::jsi::JSINativeException (#8068) (julio.rocha@microsoft.com)

## 0.63.34

Mon, 21 Jun 2021 15:08:21 GMT

### Patches

- [0.63]Fix XAML Visibility for display:none (agnel@microsoft.com)
- [0.63] More cases of *Undefined* are popping up in MsBuild (dannyvv@microsoft.com)

## 0.63.33

Fri, 18 Jun 2021 00:57:20 GMT

### Patches

- V8 package update (tudor.mihai@microsoft.com)

## 0.63.32

Fri, 04 Jun 2021 23:23:19 GMT

### Patches

- Allow propagating WinRTWebSocketResource constructor exceptions (#7892) (julio.rocha@microsoft.com)
- Try to work around new nuget braking change (dannyvv@microsoft.com)
- V8 package update (tudor.mihai@microsoft.com)
- Add JSI.MemoryMappedScriptStore runtime option to control usage of MemoryMappedBuffer (#7943) (julio.rocha@microsoft.com)
- Fix JSValue memory leak in move constructor (vmorozov@microsoft.com)

## 0.63.31

Mon, 31 May 2021 15:07:23 GMT

### Patches

- Update .net uwp version (agnel@microsoft.com)

## 0.63.29

Mon, 17 May 2021 15:09:02 GMT

### Patches

- Use memory-mapping in BasePreparedScriptStoreImpl::tryGetPreparedScript (julio.rocha@microsoft.com)

## 0.63.28

Mon, 10 May 2021 15:06:43 GMT

### Patches

- Update V8 package version (tudor.mihai@microsoft.com)

## 0.63.27

Tue, 13 Apr 2021 02:52:41 GMT

### Patches

- Allow swapping to System Chakra at runtime. (julio.rocha@microsoft.com)

## 0.63.26

Mon, 29 Mar 2021 15:27:29 GMT

### Patches

- Debugging: Adding variables to watch window crashes the process hosting the addin VSO 4853697 (anandrag@microsoft.com)

## 0.63.25

Fri, 26 Mar 2021 22:43:28 GMT

### Patches

- Fix AppStateModule::GetCurrentAppState from returning undefined (namrog84@gmail.com)
- Backporting codegen test fix for build on new agents (agnel@microsoft.com)
- Promote 0.63 to legacy (ngerlem@microsoft.com)

## 0.63.24

Wed, 10 Mar 2021 17:41:09 GMT

### Patches

- [0.63] Fix usage of std::thread in RN's RuntimeExecutor (asklar@microsoft.com)
- undefined (asklar@microsoft.com)

## 0.63.23

Fri, 05 Mar 2021 08:02:05 GMT

### Patches

- Avoid exporting data as that breaks delay loading (ryknuth@microsoft.com)

## 0.63.22

Tue, 16 Feb 2021 22:57:36 GMT

### Patches

- Workaround arm64 compiler bug for internal consumption (30809111+acoates-ms@users.noreply.github.com)

## 0.63.20

Mon, 01 Feb 2021 15:05:10 GMT

### Patches

- Background/Foreground app state handlers should be registered from the xaml thread (asklar@microsoft.com)
- Update V8 package version (tudor.mihai@microsoft.com)

## 0.63.19

Mon, 25 Jan 2021 15:08:23 GMT

### Patches

- Include jest config files in package (30809111+acoates-ms@users.noreply.github.com)
- Add new .net native and UWP runtime versions (asklar@microsoft.com)

## 0.63.18

Mon, 18 Jan 2021 15:07:08 GMT

### Patches

- Backport of bugfix: Dimension never fires change event (Bartosz.Klonowski@callstack.com)
- Reverse order of AccessibilityView in TransferProperties(). (igklemen@microsoft.com)

## 0.63.17

Mon, 11 Jan 2021 15:07:21 GMT

### Patches

- Show HRESULT code on WebSocket errors (#6734) (julio.rocha@microsoft.com)

## 0.63.15

Mon, 21 Dec 2020 15:06:05 GMT

### Patches

- [0.63] Make package config more strict (dannyvv@microsoft.com)

## 0.63.14

Fri, 11 Dec 2020 17:51:41 GMT

### Patches

- Fix Clicks/Taps Being Eaten (ngerlem@microsoft.com)
- Fix destruction of UIManager in UI thread (#6680) (vmorozov@microsoft.com)

## 0.63.13

Tue, 08 Dec 2020 04:33:16 GMT

### Patches

- Remove Sporaddic Assert on Instance Reload from TimingModule (ngerlem@microsoft.com)
- Remove the other TimingModule Assertion (ngerlem@microsoft.com)
- [0.63] Autolinking fixes for C++ / C# interop (jthysell@microsoft.com)

## 0.63.11

Wed, 25 Nov 2020 03:02:04 GMT

### Patches

- [0.63] Make ref assembly of MS.RN.Managed nuget anycpu (dannyvv@microsoft.com)

## 0.63.10

Wed, 18 Nov 2020 01:36:10 GMT

### Patches

- Use HRESULT instead of catching exceptions for WebSocket APIs (#5174) (julio.rocha@microsoft.com)
- Add win32 exports required for internal usage (email not defined)
- CornerRadius only exists on RS5, restore the behavior to gracefully degrade on older OS releases (asklar@microsoft.com)

## 0.63.9

Mon, 16 Nov 2020 15:06:51 GMT

### Patches

- [0.63] Allow NodeJS 14 (asklar@microsoft.com)
- Don't return an error if AsyncStorage is empty (i@mandrigin.ru)

## 0.63.8

Tue, 10 Nov 2020 16:50:27 GMT

### Patches

- Upgrade to prettier 1.19.1 which has support for optional chaining operator (x?.foo) and lint (asklar@microsoft.com)

## 0.63.7

Mon, 09 Nov 2020 17:01:11 GMT

### Patches

- Set Content-Length on the content object and not on the request itself. (i@mandrigin.ru)
- Update Microsoft.ReactNative Nuget Package to support layout convention (#6381) (dannyvv@microsoft.com)

## 0.63.6

Mon, 02 Nov 2020 15:05:04 GMT

### Patches

- Fix crash during instance destruction if there are outstanding animations (30809111+acoates-ms@users.noreply.github.com)
- [Internal] Expose additional createUIManagerModule method (30809111+acoates-ms@users.noreply.github.com)

## 0.63.5

Mon, 12 Oct 2020 15:07:16 GMT

### Patches

- Backport fix for native modules not being re-initialized on reload (#6159) (tn0502@gmail.com)

## 0.63.4

Mon, 05 Oct 2020 15:04:50 GMT

### Patches

- Fix pre-1903 crash when loading certain images (email not defined)

## 0.63.3

Mon, 21 Sep 2020 15:04:59 GMT

### Patches

- Port update folly to 0.63 (asklar@winse.microsoft.com)

## 0.63.2

Mon, 14 Sep 2020 21:58:15 GMT

### Patches

- Deprecate AppTheme by adding yellowbox warning (#5889) (email not defined)
- Fix 5870: Dont check for inheritance, but do explicit type check (dannyvv@microsoft.com)
- Update V8 and Hermes pacakge versions (tudor.mihai@microsoft.com)

## 0.63.1

Mon, 07 Sep 2020 15:05:16 GMT

### Patches

- Add ARM64 Desktop support (#5864) (julio.rocha@microsoft.com)
- Drop shadownodes still in registry in uimanager dtor (email not defined)

## 0.63.0

Mon, 31 Aug 2020 15:05:16 GMT

### Patches

- Promote 0.63 to latest (ngerlem@microsoft.com)

## 0.63.0-preview.6

Fri, 28 Aug 2020 17:58:04 GMT

### Changes

- Fix fast refresh (acoates-ms@noreply.github.com)
- Harden ReactInstanceWin::NativeUIManager to return null when instance is being torn down (acoates-ms@noreply.github.com)

## 0.63.0-preview.5

Fri, 28 Aug 2020 01:35:52 GMT

### Changes

- Fix bugs from acceptsKeyboardFocus to focusable deprecation (ngerlem@microsoft.com)
- Fix C# interop serialization for JsValueObject (dannyvv@microsoft.com)
- Fixed templates so C++ apps can consume C# native modules (jthysell@microsoft.com)

## 0.63.0-preview.4

Mon, 24 Aug 2020 15:05:26 GMT

### Changes

- Update nuspec copyright header to use xml entity as some of the powershell processing we do messed up the encoding (dannyvv@microsoft.com)

## 0.63.0-preview.3

Mon, 17 Aug 2020 15:05:03 GMT

### Changes

- Add Get/SetRuntimeOptionInt. (julio.rocha@microsoft.com)
- Add licene metadata to nuget package for nuget.org compliance (dannyvv@microsoft.com)
- [0.63] Use thread compositor in ReactImage (asklar@winse.microsoft.com)
- [0.63] Fix teardown crash in WebSocketJSExecutor (asklar@winse.microsoft.com)
- [0.63] Fix crash in ViewViewManager (asklar@microsoft.com)
- Restore Previous Behavior Allowing onMouseEnter and onMouseLeave on Touchables (ngerlem@microsoft.com)
- Handle MessageWebSocket constructor exceptions (julio.rocha@microsoft.com)
- Make C# CodeGen the default (dannyvv@microsoft.com)

## 0.63.0-preview.2

Mon, 10 Aug 2020 15:04:54 GMT

### Changes

- Fix label of RedBox reload button (email not defined)
- Fix Crash Using accessibilityValue Without Web Debugging (ngerlem@microsoft.com)
- Integrate 0.63.2 (ngerlem@microsoft.com)
- [0.63] Fix word-wrapping behavior of tooltips (asklar@microsoft.com)

## 0.63.0-preview.1

Fri, 31 Jul 2020 23:32:31 GMT

### Changes

- Promote 0.63 to preview (ngerlem@microsoft.com)

## 0.0.0-canary.134

Fri, 31 Jul 2020 05:05:10 GMT

### Changes

- WinUI 3 islands support (asklar@microsoft.com)

## 0.0.0-canary.133

Thu, 30 Jul 2020 05:04:47 GMT

### Changes

- Share packager connection between instances to same packager (email not defined)

## 0.0.0-canary.132

Wed, 29 Jul 2020 05:04:59 GMT

### Changes

- Update codegen targets for source mode to use property instead of items (dannyvv@microsoft.com)

## 0.0.0-canary.131

Tue, 28 Jul 2020 05:06:13 GMT

### Changes

- Separate local-cli into its own package (ngerlem@microsoft.com)
- Clang tidy, wave 2 (asklar@microsoft.com)
- Deploy apps in CI because we want to ensure we are catching cli deployment issues (asklar@microsoft.com)

## 0.0.0-canary.130

Sun, 26 Jul 2020 05:06:58 GMT

### Changes

- Integrate Up To The Base of React Native 0.63 (ngerlem@microsoft.com)

## 0.0.0-canary.129

Sat, 25 Jul 2020 05:04:34 GMT

### Changes

- clang tidy, wave 1 (asklar@microsoft.com)

## 0.0.0-canary.128

Fri, 24 Jul 2020 05:05:51 GMT

### Changes

- Stop placing Microsoft.ReactNative.Cxx in the Microsoft.ReactNative nuget package (dannyvv@microsoft.com)
- Update V8 package version (tudorm@microsoft.com)

## 0.0.0-canary.127

Thu, 23 Jul 2020 05:04:42 GMT

### Changes

- move legacy tests to end of list (kmelmon@microsoft.com)

## 0.0.0-canary.126

Wed, 22 Jul 2020 05:04:42 GMT

### Changes

- move e2etest pages into rntester (kmelmon@microsoft.com)
- Implement Image tintColor style prop (email not defined)
- move to winui3 preview 2 (asklar@microsoft.com)
- Use compositor in TLS (asklar@microsoft.com)

## 0.0.0-canary.125

Tue, 21 Jul 2020 05:05:04 GMT

### Changes

- Remove ReactWindows-Universal.sln and update publish.yml to use Microsoft.React-native.sln (dannyvv@microsoft.com)
- Display expected answers and default one (sam@elborai.me)
- Update e2e-testing.md (dannyvv@microsoft.com)
- Add windows adk to machine setup script (for build lab) (dannyvv@microsoft.com)

## 0.0.0-canary.124

Sat, 18 Jul 2020 05:04:20 GMT

### Changes

- Fix layout deploy (asklar@microsoft.com)
- Enabeling SourceLink support. (dannyvv@microsoft.com)
- Add fix for msbuild long paths in fixup script (dannyvv@microsoft.com)

## 0.0.0-canary.123

Fri, 17 Jul 2020 05:06:31 GMT

### Changes

- Codesign Microsoft.ReactNative binaries and nuget package (disabled) (jthysell@microsoft.com)

## 0.0.0-canary.122

Thu, 16 Jul 2020 05:04:45 GMT

### Changes

- Implement Image blurRadius (email not defined)
- Add support for `Task` and `Task<T>` async methods for C# native modules. (dannyvv@microsoft.com)
- Add debugging output for image load failures (asklar@microsoft.com)
- Have Desktop int. tests consume DLL. (julio.rocha@microsoft.com)

## 0.0.0-canary.121

Wed, 15 Jul 2020 05:04:55 GMT

### Changes

- Move some templates to shared location. (dannyvv@microsoft.com)

## 0.0.0-canary.120

Tue, 14 Jul 2020 05:05:25 GMT

### Changes

- don't error out on errors from installing dependent framework packages (asklar@microsoft.com)
- Start using xamlroot's compositor when available for islands to work. This enables most of playground win32 for rnw-on-winui3 for a pre-preview 2 build (asklar@microsoft.com)

## 0.0.0-canary.119

Sun, 12 Jul 2020 05:05:04 GMT

### Changes

- Move index file to flow (acoates@microsoft.com)

## 0.0.0-canary.118

Sat, 11 Jul 2020 05:09:38 GMT

### Changes

- Move contents of ReactDirectories.props to Directory.build.props (ngerlem@microsoft.com)
- Define Set/Get FeatureGate (julio.rocha@microsoft.com)
- Create nuget package for Microsoft.ReacNative.Cxx (dannyvv@microsoft.com)
- [Image] Remove resizemode assert (email not defined)
- Automatically Pull In New Files from RNTester and IntegrationTests (ngerlem@microsoft.com)

## 0.0.0-canary.117

Fri, 10 Jul 2020 05:06:06 GMT

### Changes

- Make IWebSocketResource methods noexcept. (julio.rocha@microsoft.com)
- run-windows cannot find built app with default init app (acoates@microsoft.com)
- Create nuget package for Managed CodeGen (dannyvv@microsoft.com)

## 0.0.0-canary.116

Wed, 08 Jul 2020 05:04:59 GMT

### Changes

- Dont try to parseJson unless redbox error starts with { (acoates@microsoft.com)
- Deploy dependencies when registering from layout (asklar@microsoft.com)

## 0.0.0-canary.115

Wed, 08 Jul 2020 01:27:37 GMT

### Changes

- Add a bunch of deps we are using but not declaring (acoates@microsoft.com)
- Fix #4809: Remove the /flow folder from the shipping npm package (dannyvv@microsoft.com)
- Rever to new experimentalNuGetDependency Naming (ngerlem@microsoft.com)

## 0.0.0-canary.114

Tue, 07 Jul 2020 05:05:23 GMT

### Changes

- Enable experimental usage of Microsoft.ReactNative.Managed as a nuget package (dannyvv@microsoft.com)

## 0.0.0-canary.113

Mon, 06 Jul 2020 23:11:46 GMT

### Changes

- Add Support for Binary Overrides to react-native-platform-override (ngerlem@microsoft.com)
- Create a Programmatic API for react-native-platform-override (ngerlem@microsoft.com)

## 0.0.0-canary.112

Fri, 03 Jul 2020 05:05:03 GMT

### Changes

- Enforce null/undefined safety for the CLI (ngerlem@microsoft.com)
- Fix #4810: Remove the /vnext/etc/react-native-windows.api.md file from the shipping npm package (dannyvv@microsoft.com)
- Remove github actions workflow for CI and publish (dannyvv@microsoft.com)
- Include "nuget.exe" as Package Dependency of react-native-windows (ngerlem@microsoft.com)
- Replace Custom Generator Prompt With Inquirer (ngerlem@microsoft.com)
- Cleanup VS Installation Related CLI code (ngerlem@microsoft.com)
- add guard to ensure only one version of XAML is loaded (asklar@microsoft.com)

## 0.0.0-canary.111

Thu, 02 Jul 2020 05:05:06 GMT

### Changes

- Migrate 'AppState' to turbo module (zihanc@microsoft.com)
- Migrate 'DeviceInfo' to turbo module (zihanc@microsoft.com)
- Migrate 'I18nManager' to turbo module (zihanc@microsoft.com)

## 0.0.0-canary.110

Wed, 01 Jul 2020 05:06:19 GMT

### Changes

- Fix compile warnings (dannyvv@microsoft.com)
- Remove delayload dependencies on some winrt apisets, to reduce warning count (dannyvv@microsoft.com)
- Migrate 'DevSettings' to turbo module (zihanc@microsoft.com)
- Migrate 'LogBox' to turbo module (zihanc@microsoft.com)
- Implement REACT_CONSTANT as getConstants sync method for turbo module (zihanc@microsoft.com)
- Fix a few more casing typos (dannyvv@microsoft.com)
- Share eslint and Typescript configs across packages (ngerlem@microsoft.com)
- Migrate the local-cli to TypeScript (ngerlem@microsoft.com)
- Migrate "Alert" to turbo module (zihanc@microsoft.com)

## 0.0.0-canary.109

Tue, 30 Jun 2020 05:06:03 GMT

### Changes

- Call AppPolicyGetWindowingModel dynamically since it's WACK-banned (asklar@microsoft.com)

## 0.0.0-canary.108

Mon, 29 Jun 2020 05:04:57 GMT

### Changes

- Removed references to ReactWindowsCore (julio.rocha@microsoft.com)

## 0.0.0-canary.107

Sun, 28 Jun 2020 05:04:51 GMT

### Changes

- Fix casing typos in NuGet and MSBuild (dannyvv@microsoft.com)
- Remove Include/ReactUWP files (vmorozov@microsoft.com)
- Fix build errors for codegen unittest project when building in VS2019 (dannyvv@microsoft.com)

## 0.0.0-canary.106

Sat, 27 Jun 2020 03:52:38 GMT

### Changes

- Add Enterprise vs2019 support to rnw-dependencies (dannyvv@microsoft.com)

## 0.0.0-canary.104

Fri, 26 Jun 2020 23:59:27 GMT

### Changes

- Upgrade React Native to 3/22 Nightly Build (ngerlem@microsoft.com)

## 0.0.0-canary.103

Fri, 26 Jun 2020 20:17:03 GMT

### Changes

- Add a null check on reload callback (acoates@microsoft.com)
- Unify storage for sourceBundleHost/Port and debugHost (acoates@microsoft.com)
- Remove ReactUWP project folder (vmorozov@microsoft.com)
- Fix empty host getting passed to HMR client when SourceBundleHost not specified (acoates@microsoft.com)

## 0.0.0-canary.102

Thu, 25 Jun 2020 14:32:50 GMT

### Changes

- WinUI3 Preview 1 (asklar@microsoft.com)

## 0.0.0-canary.101

Thu, 25 Jun 2020 00:07:54 GMT

### Changes

- Use Deterministic NuGet Binary (ngerlem@microsoft.com)

## 0.0.0-canary.100

Wed, 24 Jun 2020 00:07:20 GMT

### Changes

- Fix warnings of signed/unsigned comparrison mismatches on x86 builds (dannyvv@microsoft.com)
- update csproj to use versions from winui.props (asklar@microsoft.com)
- Fix JSValueReaderGenerator runtime error for read-only properties (vmorozov@microsoft.com)

## 0.0.0-canary.99

Tue, 23 Jun 2020 00:05:47 GMT

### Changes

- start moving away from config and use a sentinel file instead (asklar@microsoft.com)

## 0.0.0-canary.98

Sat, 20 Jun 2020 00:05:18 GMT

### Changes

- Auto-linking for native modules (source-based) (jthysell@microsoft.com)
- Implement AccessibilityValue from RN 62 (igklemen@microsoft.com)
- Complete BundleUrl should be provided to JSExecutor when using fastrefresh without remote debugging (acoates@microsoft.com)
- turbo modules should be replacable (zihanc@microsoft.com)

## 0.0.0-canary.97

Fri, 19 Jun 2020 00:07:58 GMT

### Changes

- Move the addition of folly.natvis to ReactWin32.nuspec (warleu@microsoft.com)
- Fix building vcxproj files outside of solution (dannyvv@microsoft.com)

## 0.0.0-canary.96

Thu, 18 Jun 2020 00:07:44 GMT

### Changes

- Create Scripts for the Release Process (ngerlem@microsoft.com)
- Run NativeModules 2.0 in JS thread (vmorozov@microsoft.com)
- Update C++/WinRT to version 2.0.200615.7 (vmorozov@microsoft.com)
- version agnostic msbuild log viewer (asklar@microsoft.com)

## 0.0.0-canary.95

Wed, 17 Jun 2020 00:05:39 GMT

### Changes

- Unify DevSupportManager and WebSocketJSExecutor (acoates@microsoft.com)
- Rename "master" packages to "canary" (ngerlem@microsoft.com)
- Don't lint target folder and don't flow check node_modules packages (dannyvv@microsoft.com)

## 0.0.0-master.94

Tue, 16 Jun 2020 00:05:48 GMT

### Changes

-  Upgrade to boost 1.72.0. (julio.rocha@microsoft.com)
- Prevent Switches from Accidentally Becoming Disabled (ngerlem@microsoft.com)
- use default brush for border (kmelmon@microsoft.com)

## 0.0.0-master.93

Sat, 13 Jun 2020 00:09:11 GMT

### Changes

- Implement logbox (acoates@microsoft.com)
- Add Microsoft.ReactNative.Cxx to the nugets. (acoates@microsoft.com)
- Fix file lock issue wiht msbuild /bl flag and the metro bundler (dannyvv@microsoft.com)

## 0.0.0-master.92

Fri, 12 Jun 2020 00:33:44 GMT

### Changes

- Fix support for `--namespace` on react-native-windows init for cpp (dannyvv@microsoft.com)

## 0.0.0-master.91

Thu, 11 Jun 2020 00:05:14 GMT

### Changes

- Add support for redbox's on xamlislands (acoates@microsoft.com)
- Manually set Lib and Include paths for Hermes (julio.rocha@microsoft.com)
- More rnw-dependencies improvements (asklar@microsoft.com)

## 0.0.0-master.90

Wed, 10 Jun 2020 00:05:10 GMT

### Changes

- implement --proxy in CLI (igklemen@microsoft.com)
- Don't crash when using animations in RS3-RS4, just no-op (asklar@microsoft.com)
- Add support for Alert module with Xaml Islands (acoates@microsoft.com)
- fix onPress for nested Text (igklemen@microsoft.com)
- translate PS exception into error code (asklar@microsoft.com)

## 0.0.0-master.89

Tue, 09 Jun 2020 00:06:03 GMT

### Changes

- Use yoga config to align with ios/android (acoates@microsoft.com)

## 0.0.0-master.88

Mon, 08 Jun 2020 00:05:53 GMT

### Changes

- Add Integration and Component tests to CI (vmorozov@microsoft.com)

## 0.0.0-master.87

Sun, 07 Jun 2020 00:05:23 GMT

### Changes

- Remove SharedProject (dannyvv@microsoft.com)
- Update template gen to use mustache (dannyvv@microsoft.com)
- Install MSBuild log viewer (asklar@microsoft.com)
- DevMenu should only show up when DeveloperSupport is on (acoates@microsoft.com)
- Remove legacy ReactInstanceSettings struct (vmorozov@microsoft.com)

## 0.0.0-master.86

Sat, 06 Jun 2020 00:05:20 GMT

### Changes

- Expose turbo module registry API (zihanc@microsoft.com)
- Ensure Image.windows.js is an exact copy of Image.ios.js (ngerlem@microsoft.com)

## 0.0.0-master.85

Fri, 05 Jun 2020 00:06:16 GMT

### Changes

- Adding compile time Code generation for Managed React Native modules for Windows (dannyvv@microsoft.com)
- Move devmenu out of RootControl, implement more of DevSettings (acoates@microsoft.com)
- Fix SampleApp run-windows with deploy (dannyvv@microsoft.com)

## 0.0.0-master.84

Thu, 04 Jun 2020 00:05:05 GMT

### Changes

- Fix ReloadInstance crash caused by unloaded instance (vmorozov@microsoft.com)
- Re-enable RNTester integration tests. (julio.rocha@microsoft.com)

## 0.0.0-master.83

Wed, 03 Jun 2020 00:05:25 GMT

### Changes

- Deprecate acceptsKeyboardFocus (kaigu@microsoft.com)
- Remove react-native-community/cli dependency from template (acoates@microsoft.com)
- Code Cleanup: Some low haning fruit lint fixes (dannyvv@microsoft.com)
- Allow paremeterization of buildLogDirectory for msbuild task when using run-windows (dannyvv@microsoft.com)
- Minor autolinking template update prep (jthysell@microsoft.com)

## 0.0.0-master.82

Tue, 02 Jun 2020 00:07:29 GMT

### Changes

- Enable ReactNativeHost integration test (vmorozov@microsoft.com)

## 0.0.0-master.81

Mon, 01 Jun 2020 00:05:31 GMT

### Changes

- Update projects to use shared props/targets (dannyvv@microsoft.com)

## 0.0.0-master.80

Sun, 31 May 2020 00:05:21 GMT

### Changes

- blacklist all ProjectImports.zip (asklar@microsoft.com)
- docs CI (asklar@microsoft.com)

## 0.0.0-master.79

Sat, 30 May 2020 00:04:58 GMT

### Changes

- Remove references to ReactWindowsCore projects (julio.rocha@microsoft.com)
- Add XamlUIService with ElementFromReactTag (acoates@microsoft.com)
- Fixes CLI C# app not deploying (jthysell@microsoft.com)

## 0.0.0-master.78

Fri, 29 May 2020 00:09:08 GMT

### Changes

- Remove unused ABI definitions (vmorozov@microsoft.com)
- Update shared props to have csc emit absolute paths on errors and warnings (dannyvv@microsoft.com)
- add support for long paths to rnw-dependencies.ps1 (asklar@microsoft.com)

## 0.0.0-master.77

Thu, 28 May 2020 00:05:30 GMT

### Changes

- set up react application to use an app's reactrootview (asklar@microsoft.com)

## 0.0.0-master.76

Wed, 27 May 2020 00:05:42 GMT

### Changes

- Allow developers to independently set hostname and port. (12337821+nasadigital@users.noreply.github.com)
- Move I18nManager to spec based native module (acoates@microsoft.com)
- Add UIDispatcher property to ReactInstanceSettings and IReactContext (vmorozov@microsoft.com)

## 0.0.0-master.74

Sat, 23 May 2020 00:04:45 GMT

### Changes

- Implemented support for native module std::weak_ptr (vmorozov@microsoft.com)
- Fix deploy issue causing 2nd+ builds to break due to bug in appxrecipe which gets imported (asklar@microsoft.com)
- Drop LazySupportManager (julio.rocha@microsoft.com)
- Fix ReactInstance error state to avoid crashes (vmorozov@microsoft.com)
- Move DeviceInfo and AppState to new native module impl (acoates@microsoft.com)

## 0.0.0-master.73

Fri, 22 May 2020 00:05:31 GMT

### Changes

- Work around regression in app packaging in VS 16.5-16.6 (asklar@microsoft.com)
- Do not emit click events for buttons other than left. (igklemen@microsoft.com)
- Fixed Microsoft.ReactNative build in VS 16.6.0 (vmorozov@microsoft.com)
- fix for animation bug (kmelmon@microsoft.com)
- dont overwrite msbuild log when deploying (asklar@microsoft.com)
- Fix spacing and extra dot in copyright of the property sheets (dannyvv@microsoft.com)
- Standardize version of Microsoft.NETCore.UniversalWindowsPlatform to 6.2.9 (dannyvv@microsoft.com)
- Remove unneeded reference form Microsoft.ReactNative.Managed (dannyvv@microsoft.com)

## 0.0.0-master.72

Thu, 21 May 2020 00:04:53 GMT

### Changes

- deploy from layout (asklar@microsoft.com)
- ReactNotificationService to allow communications between native modules (vmorozov@microsoft.com)

## 0.0.0-master.71

Wed, 20 May 2020 00:04:49 GMT

### Changes

- Implement Keybpoarding/Gamepad Activation for Pressability (ngerlem@microsoft.com)
- Reduce need for custom metro configs (acoates@microsoft.com)
- Update the react-native-windows-init template to use the managed library the default (instead of the shared csproj file). (dannyvv@microsoft.com)

## 0.0.0-master.70

Tue, 19 May 2020 00:05:07 GMT

### Changes

- Fixes for bundle loading and back handler crash using xaml islands (acoates@microsoft.com)
- Remove legacy ReactUWP (acoates@microsoft.com)
- Support 0..n args in native module callbacks (vmorozov@microsoft.com)
- Adapt ViewWin32, etc to "accessibilityState" (ngerlem@microsoft.com)

## 0.0.0-master.69

Sun, 17 May 2020 00:04:22 GMT

### Changes

- ReactNonAbiValue for IReactPropertyBag non-ABI values (vmorozov@microsoft.com)
- Prevent hard crash on invalid jsx (button inside text e.g.) (asklar@microsoft.com)
- Fixed ReactContext CallJSFunction and EmitJSEvent (vmorozov@microsoft.com)

## 0.0.0-master.68

Sat, 16 May 2020 00:04:28 GMT

### Changes

- some projects have target instead of bin folders (asklar@microsoft.com)
- .\JustMyXaml.ps1 (asklar@microsoft.com)

## 0.0.0-master.67

Fri, 15 May 2020 00:04:44 GMT

### Changes

- winui3 option for app project template (asklar@microsoft.com)
- Reduce PCH Sizes (ngerlem@microsoft.com)
- Remove unused lifecycle events in ReactNativeHost (kaigu@microsoft.com)
- Move pch files into a Pch subfolder. (vmorozov@microsoft.com)
- exact match of WinAppDriver required (asklar@microsoft.com)

## 0.0.0-master.66

Thu, 14 May 2020 00:04:37 GMT

### Changes

- add rnw-dependencies.ps1 (asklar@microsoft.com)
- Use DispatcherQueue instead of CoreDispatcher for Mso::DispatchQueue (vmorozov@microsoft.com)
- Allow storing non-WinRT types in ReactPropertyBag (vmorozov@microsoft.com)
- revert dfc57fcf2504f57baab20f550b36a618eaa99e56 (kmelmon@microsoft.com)
- don't exit the powershell session and pause for keyboard input if interactive (asklar@microsoft.com)

## 0.0.0-master.65

Wed, 13 May 2020 00:04:21 GMT

### Changes

- add ReactNativeHost to Win32 C++/WinRT ABI (aeulitz@microsoft.com)
- Fixed ReactContext copy/move semantic (vmorozov@microsoft.com)
- Use spec file for DevSettings NativeModule (acoates@microsoft.com)

## 0.0.0-master.64

Tue, 12 May 2020 00:04:39 GMT

### Changes

- Fire onLoad event when a bitmap image is opeed (lamdoan@microsoft.com)
- Expose ability for apps to provide their own RedBox implementation (acoates@microsoft.com)

## 0.0.0-master.63

Sun, 10 May 2020 00:05:02 GMT

### Changes

- Reenable V8 for desktop projects (tudorm@microsoft.com)

## 0.0.0-master.62

Sat, 09 May 2020 00:04:42 GMT

### Changes

- Autolinking (jthysell@microsoft.com)
- Implemented PropertyBag for ReactContext and ReactInstanceSettings (vmorozov@microsoft.com)
- Align local metro config with future CLI changes (acoates@microsoft.com)
- separate desktop from uwp ReactWindowsCore since desktop uses V8 (asklar@microsoft.com)
- Set C# langversion of templates back to 7.3 from 8.0 (dannyvv@microsoft.com)
- Correct binary message tagging (julio.rocha@microsoft.com)
- Added Microsoft.ReactNative.IntegrationTests project to Microsoft.ReactNative.sln (vmorozov@microsoft.com)

## 0.0.0-master.61

Fri, 08 May 2020 00:05:16 GMT

### Changes

- Promote awaitable DispatchQueue callback to member field (julio.rocha@microsoft.com)
- Upgrade the shared managed project to a proper csharp library. (dannyvv@microsoft.com)
- Ensure InitializeCore is run before app code (acoates@microsoft.com)

## 0.0.0-master.60

Thu, 07 May 2020 00:05:07 GMT

### Changes

- Don't delete nuget.exe if using locally-installed (e.g. chocolatey) (asklar@microsoft.com)
- Only bundle when not running Intellisense builds (asklar@microsoft.com)
- Disable CxxFrameHandler4 (ngerlem@microsoft.com)
- Switch package layout to use `files` in `pacakge.json` rather than `.npmignore` (dannyvv@microsoft.com)

## 0.0.0-master.59

Wed, 06 May 2020 00:05:03 GMT

### Changes

- WinUI 3 Alpha support in RNW (asklar@microsoft.com)
- Treat yes the same as y in template overwrite prompt (acoates@microsoft.com)

## 0.0.0-master.58

Tue, 05 May 2020 00:04:31 GMT

### Changes

- Make generated msbuild project compile in parralel (/m) (dannyvv@microsoft.com)
- fix appPackage path (kaigu@microsoft.com)

## 0.0.0-master.57

Sun, 03 May 2020 00:04:42 GMT

### Changes

- Implement WinRTWebSocketResource unit tests (julio.rocha@microsoft.com)

## 0.0.0-master.56

Sat, 02 May 2020 00:05:12 GMT

### Changes

- - Change MouseEnter/MouseLeave behavior to match web behavior - Improve performance of frequently-called mouse move handler (aschultz@microsoft.com)
- Delay load ChakraCore.dll (tudorm@microsoft.com)

## 0.0.0-master.55

Fri, 01 May 2020 00:04:15 GMT

### Changes

- Removed the ref count work around from classes derived from ReactApplications (vmorozov@microsoft.com)
- Minor cleanup: Remove unused namespace from C# files (dannyvv@microsoft.com)

## 0.0.0-master.54

Thu, 30 Apr 2020 18:40:40 GMT

### Changes

- Expose YellowBox functionality to native code (asklar@microsoft.com)
- .net projects need more work to build a package layout (asklar@microsoft.com)
- Package Fixes (ngerlem@microsoft.com)

## 0.0.0-master.53

Thu, 30 Apr 2020 00:04:27 GMT

### Changes

- auto-detect RTL and push into root view (kmelmon@microsoft.com)
- Refactor TestHook out of ViewManagerBase and special case layout properties (asklar@microsoft.com)
- Expose Debug macro for hresult_error. Start catching these... (asklar@microsoft.com)
- Try uninstalling without elevation in case since it is not needed for layout installs (debug) (asklar@microsoft.com)

## 0.0.0-master.52

Wed, 29 Apr 2020 00:04:54 GMT

### Changes

- Start forking namespace and includes for WUX->MUX move (asklar@microsoft.com)
- Update react version (kaigu@microsoft.com)
- Exclude unittests and proposals from being published in the npm package (dannyvv@microsoft.com)

## 0.0.0-master.51

Tue, 28 Apr 2020 00:04:13 GMT

### Changes

- Added C# ReactContext for Module Initialize method (vmorozov@microsoft.com)
- Use readme file from root of repo for react-native-windows (acoates@microsoft.com)

## 0.0.0-master.50

Mon, 27 Apr 2020 00:04:59 GMT

### Changes

- Added C++ ReactContext for Module Initialize method (vmorozov@microsoft.com)

## 0.0.0-master.49

Sun, 26 Apr 2020 00:04:59 GMT

### Changes

- Use separate brush for text control border (kaigu@microsoft.com)
- Fix typo causing activity indicators to be sad and colorless (asklar@microsoft.com)
- Disable copying V8 dll to target output; we need a clearer way to distinguish uwp vs. win32 in the build system but this will do for now to unblock #4475 (tudorm@microsoft.com)

## 0.0.0-master.48

Sat, 25 Apr 2020 00:05:26 GMT

### Changes

- Better error reporting and elevating if necessary in CLI (asklar@microsoft.com)
- Detect when we have more than one SLN as it may not be obvious to the user. Add --sln option to CLI. (asklar@microsoft.com)
- Install from layout in Debug builds (asklar@microsoft.com)

## 0.0.0-master.47

Fri, 24 Apr 2020 00:04:41 GMT

### Changes

- Pass XamlView's through ReactUwp View Managers by Const Ref (ngerlem@microsoft.com)
- fix RTL (kmelmon@microsoft.com)
- Use spec file for clipboard module (acoates@microsoft.com)
- Fix Missing Include in DevSupportManagerUwp (ngerlem@microsoft.com)
- Fixed .Net Native crash for custom struct deserialization (vmorozov@microsoft.com)

## 0.0.0-master.46

Thu, 23 Apr 2020 00:04:37 GMT

### Changes

- Hook up to packager websocket to allow 'r' to reload instance (acoates@microsoft.com)
- Handle syntax errors in RedBox (asklar@microsoft.com)
- Publish ship and debug bits in the nuget (acoates@microsoft.com)
- Implement String ViewManager Command IDs (ngerlem@microsoft.com)

## 0.0.0-master.45

Wed, 22 Apr 2020 00:04:29 GMT

### Changes

- Make default template build non-dev bundle when not using debug configuration (acoates@microsoft.com)
- Implemented C++ TurboModule compile time spec validation (vmorozov@microsoft.com)
- Fix devtools connection to match RN0.62 (acoates@microsoft.com)

## 0.0.0-master.44

Sun, 19 Apr 2020 00:04:29 GMT

### Changes

- fix include paths (kmelmon@microsoft.com)

## 0.0.0-master.43

Sat, 18 Apr 2020 00:04:34 GMT

### Changes

- Allow enabling of direct debugging through the CLI. (12337821+nasadigital@users.noreply.github.com)
- Implelent Shared StatusBarManagerModule and Do Module Cleanup (ngerlem@microsoft.com)
- Implement AppearanceModule (ngerlem@microsoft.com)
- Add support for React Native BackHandler API (jahiggin@microsoft.com)
- remove AccessibilityStates (kmelmon@microsoft.com)
- Fix issue with yoga layout in x64 release (acoates@microsoft.com)

## 0.0.0-master.42

Fri, 17 Apr 2020 00:04:27 GMT

### Changes

- run_wdio.js will run the tests but also set the exit code to zero/non-zero on success/failure respectively. This is important to break the CI/PR build on test failures, which we weren't doing until now. (asklar@winse.microsoft.com)
- Add msbuild.ProjectImports.zip to default blacklist to avoid metro error on run-windows (acoates@microsoft.com)
- implement accessibilityState (kmelmon@microsoft.com)

## 0.0.0-master.41

Wed, 15 Apr 2020 23:02:16 GMT

### Changes

- Fix PickerWindows (and DatePickerExample Page) (ngerlem@microsoft.com)

## 0.0.0-master.40

Wed, 15 Apr 2020 22:26:36 GMT

### Changes

- Use a custom resolver wrapper to allow metro to run for multiple platforms at once (acoates@microsoft.com)

## 0.0.0-master.39

Wed, 15 Apr 2020 19:18:34 GMT

### Changes

- Rename DeveloperSettings.SourceBundlePath -> SourceBundleName (rezha@microsoft.com)

## 0.0.0-master.38

Wed, 15 Apr 2020 07:53:39 GMT

### Changes

- fix bug where not specifying height on TextInput causes 0 height (kmelmon@microsoft.com)

## 0.0.0-master.37

Tue, 14 Apr 2020 19:34:42 GMT

### Changes

- [generator] Use `npx` for consistency (eloy.de.enige@gmail.com)

## 0.0.0-master.36

Tue, 14 Apr 2020 18:14:12 GMT

### Changes

- Elevate to install app certificate if necessary (asklar@microsoft.com)

## 0.0.0-master.35

Tue, 14 Apr 2020 14:59:17 GMT

### Changes

- Upgrade to React Native 0.62 (ngerlem@microsoft.com)

## 0.0.0-master.34

Tue, 14 Apr 2020 02:46:30 GMT

### Changes

- add missing headers to proj file (kmelmon@microsoft.com)

## 0.0.0-master.33

Mon, 13 Apr 2020 18:45:40 GMT

### Changes

- Remove Folly ContinueOnError (ngerlem@microsoft.com)

## 0.0.0-master.32

Mon, 13 Apr 2020 07:51:05 GMT

### Changes

- Refactor View managers to detect unimplemented props (asklar@microsoft.com)

## 0.0.0-master.31

Sat, 11 Apr 2020 02:36:32 GMT

### Changes

- Improve RedBox ux, and don't require elevation unless it's the first time running the build (and need to enable dev mode) (asklar@microsoft.com)

## 0.0.0-master.30

Fri, 10 Apr 2020 22:03:49 GMT

### Changes

- Simplified C++ macros and improved their comments (vmorozov@microsoft.com)

## 0.0.0-master.29

Fri, 10 Apr 2020 14:44:13 GMT

### Changes

- Don't set readyState on connection error (julio@rochsquadron.net)

## 0.0.0-master.28

Fri, 10 Apr 2020 06:15:31 GMT

### Changes

- Improve inner loop and error reporting (local CLI) (asklar@microsoft.com)

## 0.0.0-master.27

Thu, 09 Apr 2020 21:36:07 GMT

### Changes

- Enable managed unit tests (jthysell@microsoft.com)

## 0.0.0-master.26

Thu, 09 Apr 2020 17:16:44 GMT

### Changes

- Creating new Microsoft.ReactNative.sln without legacy ReactUWP (jthysell@microsoft.com)

## 0.0.0-master.25

Wed, 08 Apr 2020 19:59:18 GMT

### Changes

- Remove using namespace in ReactPackageProvider.h (kaigu@microsoft.com)

## 0.0.0-master.24

Wed, 08 Apr 2020 17:48:10 GMT

### Changes

- Rework more publish logic - produce nugets in PRs (acoates@microsoft.com)

## 0.0.0-master.23

Wed, 08 Apr 2020 13:58:29 GMT

### Changes

- Apply version numbers to Microsoft.ReactNative (acoates@microsoft.com)

## 0.0.0-master.22

Wed, 08 Apr 2020 00:50:14 GMT

### Changes

- Optimizing VS component installer to speed up CI builds (jthysell@microsoft.com)

## 0.0.0-master.21

Tue, 07 Apr 2020 21:11:39 GMT

### Changes

- placeholderTextColor for secureTextEntry on TextInput (dzndlhov@microsoft.com)

## 0.0.0-master.20

Tue, 07 Apr 2020 20:21:01 GMT

### Changes

- Fix issue adding react-native-windows to a package in a monorepo (acoates@microsoft.com)

## 0.0.0-master.19

Tue, 07 Apr 2020 18:46:38 GMT

### Patches

- Start of implementation for nuget template (acoates@microsoft.com)

## 0.0.0-master.18

Tue, 07 Apr 2020 07:15:14 GMT

### Changes

- Enabling arm64 where possible (jthysell@microsoft.com)

## 0.0.0-master.17

Tue, 07 Apr 2020 06:29:14 GMT

### Changes

- Clean up normalization (ngerlem@microsoft.com)

## 0.0.0-master.16

Tue, 07 Apr 2020 00:48:37 GMT

### Changes

- undefined (julio@rochsquadron.net)

## 0.0.0-master.15

Mon, 06 Apr 2020 21:15:49 GMT

### Changes

- Update V8 package version (tudorm@microsoft.com)

## 0.0.0-master.14

Sat, 04 Apr 2020 05:07:07 GMT

### Changes

- Clean Up Directory Logic to Fix Publish (ngerlem@microsoft.com)

## 0.0.0-master.13
Thu, 02 Apr 2020 23:27:21 GMT

### Changes

- Fix VS 2019 16.5 build errors (jthysell@microsoft.com)
## 0.0.0-master.12
Thu, 02 Apr 2020 22:39:22 GMT

### Changes

- Improve popups (aschultz@microsoft.com)
## 0.0.0-master.11
Thu, 02 Apr 2020 18:40:22 GMT

### Changes

- Switching to VS 2019 and v142 tools (jthysell@microsoft.com)
## 0.0.0-master.10
Wed, 01 Apr 2020 21:47:08 GMT

### Changes

- Fix crash when calling measure on non-framework element (acoates@microsoft.com)
## 0.0.0-master.9
Wed, 01 Apr 2020 14:24:03 GMT

### Changes

- Fixed Mso test crash (vmorozov@microsoft.com)
## 0.0.0-master.8
Wed, 01 Apr 2020 05:12:32 GMT

### Changes

- Set $(ReactNativeWindowsDir) before it is used (vmorozov@microsoft.com)
## 0.0.0-master.7
Tue, 31 Mar 2020 21:49:07 GMT

### Changes

- Update MSBuild Tasks to use MSBuild x64 (jthysell@microsoft.com)
## 0.0.0-master.6
Tue, 31 Mar 2020 20:21:56 GMT

### Changes

- 1. Make changes in response to Vladimir's feedback in the PR for bb8c00133... 2. Fix a dumb bug in multiRemove; the parameterized query variable was not being bound so nothing ever got deleted (hpratt@microsoft.com)
## 0.0.0-master.5
Tue, 31 Mar 2020 02:03:05 GMT

### Changes

- Allow user to specify direct debugging port (daseramo@microsoft.com)
## 0.0.0-master.4
Mon, 30 Mar 2020 22:21:02 GMT

### Changes

- Updated C++/WinRT to 2.0.200316.3 and Min Windows to RS3+ (10.0.16299.0) (vmorozov@microsoft.com)
## 0.0.0-master.3
Fri, 27 Mar 2020 20:29:40 GMT

### Changes

- Fix direct debugging (acoates@microsoft.com)
## 0.0.0-master.2
Thu, 26 Mar 2020 23:11:17 GMT

### Changes

- Tests for NativeModule registration without attributes (vmorozov@microsoft.com)
## 0.0.0-master.1
Thu, 26 Mar 2020 22:33:20 GMT

### Changes

- 0.0.0 Master Versioning Scheme (ngerlem@microsoft.com)
## 0.61.0-beta.75
Thu, 26 Mar 2020 19:54:26 GMT

### Changes

- Add a timeout to catch hangs during cert generation (acoates@microsoft.com)
## 0.61.0-beta.74
Thu, 26 Mar 2020 16:02:57 GMT

### Changes

- Allow NativeModules events to have 0..n arguments (vmorozov@microsoft.com)
## 0.61.0-beta.73
Thu, 26 Mar 2020 14:54:10 GMT

### Changes

- Enable basic V8 direct debugging (tudorm@microsoft.com)
## 0.61.0-beta.72
Thu, 26 Mar 2020 05:34:25 GMT

### Changes

- Fix hang on react-native-init when run from a non-elevated prompt (acoates@microsoft.com)
## 0.61.0-beta.71
Tue, 24 Mar 2020 07:05:57 GMT

### Changes

- Do not enter loaded state when an error occurs during web bundle loadJSBundles (acoates@microsoft.com)
## 0.61.0-beta.70
Mon, 23 Mar 2020 21:05:53 GMT

### Changes

- Fixed crash when trying to load invalid image uri (jthysell@microsoft.com)
## 0.61.0-beta.69
Mon, 23 Mar 2020 14:55:40 GMT

### Changes

- Implemented Linking module protocol activation (vmorozov@microsoft.com)
## 0.61.0-beta.68
Sat, 21 Mar 2020 00:52:27 GMT

### Changes

- Update V8 package (tudorm@microsoft.com)
## 0.61.0-beta.67
Thu, 19 Mar 2020 04:03:12 GMT

### Changes

- Add stub devStettingModule to react.uwp (acoates@microsoft.com)
## 0.61.0-beta.66
Wed, 18 Mar 2020 23:06:37 GMT

### Changes

- remove unused stubs (kmelmon@microsoft.com)
## 0.61.0-beta.65
Wed, 18 Mar 2020 22:00:44 GMT

### Changes

- Changed C# JSValue API to match C++ version (vmorozov@microsoft.com)
## 0.61.0-beta.64
Wed, 18 Mar 2020 20:19:18 GMT

### Changes

- Fix ReactInstanceWin::GetBundleRootPath (jthysell@microsoft.com)
## 0.61.0-beta.63
Wed, 18 Mar 2020 04:50:36 GMT

### Changes

- remove react-native-windows-extended (kmelmon@microsoft.com)
## 0.61.0-beta.62
Tue, 17 Mar 2020 23:28:32 GMT

### Changes

- Set TabIndex to 0 for Pickers (jagorrin@microsoft.com)
## 0.61.0-beta.61
Tue, 17 Mar 2020 22:54:20 GMT

### Changes

- Enable fast refresh (acoates@microsoft.com)
## 0.61.0-beta.60
Tue, 17 Mar 2020 01:55:59 GMT

### Changes

- Set TabIndex to 0 for ScrollViewers (jagorrin@microsoft.com)
## 0.61.0-beta.59
Mon, 16 Mar 2020 19:54:21 GMT

### Changes

- Remove cli.js (kaigu@microsoft.com)
## 0.61.0-beta.58
Sat, 14 Mar 2020 03:04:21 GMT

### Changes

- Add cli.js (kaigu@microsoft.com)
## 0.61.0-beta.57
Fri, 13 Mar 2020 17:37:53 GMT

### Changes

- Build unit tests in ReactWindows-Universal solution (jthysell@microsoft.com)
- Address PR comments (dida@ntdev.microsoft.com)
- Code-Gen nativeModules.g.h for auto-linking (dida@ntdev.microsoft.com)
## 0.61.0-beta.56
Thu, 12 Mar 2020 19:18:01 GMT

### Changes

- Use redbox for native errors (acoates@microsoft.com)
## 0.61.0-beta.55
Thu, 12 Mar 2020 00:01:14 GMT

### Changes

- Fix JSValueObject operator[] (vmorozov@microsoft.com)
## 0.61.0-beta.54
Wed, 11 Mar 2020 19:32:20 GMT

### Changes

- Fully implement ExceptionsManagerModule (acoates@microsoft.com)
## 0.61.0-beta.53
Wed, 11 Mar 2020 18:14:12 GMT

### Changes

- Code review feedback (acoates@microsoft.com)
## 0.61.0-beta.52
Wed, 11 Mar 2020 16:18:00 GMT

### Changes

- Fix crash in timers during instance shutdown, and in systrace (acoates@microsoft.com)
## 0.61.0-beta.51
Tue, 10 Mar 2020 21:51:40 GMT

### Changes

- Bundle command clean-up (jthysell@microsoft.com)
## 0.61.0-beta.50
Tue, 10 Mar 2020 00:33:15 GMT

### Changes

- Strip out color markers from redbox error messages (acoates@microsoft.com)
## 0.61.0-beta.49
Mon, 09 Mar 2020 20:08:13 GMT

### Changes

- Fix Support For SysTraceSection (ngerlem@microsoft.com)
## 0.61.0-beta.48
Mon, 09 Mar 2020 18:53:03 GMT

### Changes

- Add initial redbox implementation (acoates@microsoft.com)
## 0.61.0-beta.47
Sat, 07 Mar 2020 19:35:40 GMT

### Changes

- Add XamlHelpers (kaigu@microsoft.com)
## 0.61.0-beta.46
Sat, 07 Mar 2020 02:35:07 GMT

### Changes

- Add Override Manifest for Native Code (ngerlem@microsoft.com)
## 0.61.0-beta.45
Sat, 07 Mar 2020 01:43:31 GMT

### Changes

- Remove Most cxxreact Patches and Executor Sandboxing Code (ngerlem@microsoft.com)
- Deprecated JSValue typed value access functions (vmorozov@microsoft.com)
## 0.61.0-beta.44
Sat, 07 Mar 2020 01:08:09 GMT

### Changes

- Dont show loading/redbox in ship mode (acoates@microsoft.com)
## 0.61.0-beta.43
Sat, 07 Mar 2020 00:30:45 GMT

### Changes

- Bundle commands should work if you dont have react-native installed globally (acoates@microsoft.com)
## 0.61.0-beta.42
Fri, 06 Mar 2020 23:55:14 GMT

### Changes

- Add JS engine selection to Playground (tudorm@microsoft.com)
## 0.61.0-beta.41
Fri, 06 Mar 2020 22:46:28 GMT

### Changes

- Support dependency config for RNW (dida@ntdev.microsoft.com)
## 0.61.0-beta.40
Fri, 06 Mar 2020 19:29:08 GMT

### Changes

- Fix include filters (jthysell@microsoft.com)
## 0.61.0-beta.39
Thu, 05 Mar 2020 23:13:07 GMT

### Changes

- undefined (ngerlem@microsoft.com)
- Configure vnext validate-overrides to check both override manifests (ngerlem@microsoft.com)
## 0.61.0-beta.38
Thu, 05 Mar 2020 18:30:29 GMT

### Changes

- Added TryAddAttributedModule in addition to AddAttributedModules (vmorozov@microsoft.com)
## 0.61.0-beta.37
Thu, 05 Mar 2020 05:33:57 GMT

### Changes

- Move ViewPanel import from PropertyUtils to FrameworkElementViewManager (jagorrin@microsoft.com)
## 0.61.0-beta.36
Thu, 05 Mar 2020 03:41:34 GMT

### Changes

- Enable V8 support for win32 (no defaults changed) (tudorm@microsoft.com)
## 0.61.0-beta.35
Thu, 05 Mar 2020 00:11:30 GMT

### Changes

- Support RNW project config (dida@ntdev.microsoft.com)
## 0.61.0-beta.34
Wed, 04 Mar 2020 08:47:08 GMT

### Changes

- CI Enforce that Overrides are Recorded Up to Date (ngerlem@microsoft.com)
## 0.61.0-beta.33
Wed, 04 Mar 2020 08:11:46 GMT

### Changes

- Moved header files from ReactUWP\Utils to include\ReactUWP\Utils. Updated project references. (lamdoan@microsoft.com)
## 0.61.0-beta.32
Tue, 03 Mar 2020 19:02:31 GMT

### Changes

- Remove CMakeLists (ngerlem@microsoft.com)
## 0.61.0-beta.31
Mon, 02 Mar 2020 22:47:03 GMT

### Changes

- More Override Manifest Functionality (Unit Tests Coming Next Iteration) (ngerlem@microsoft.com)
## 0.61.0-beta.30
Mon, 02 Mar 2020 21:53:02 GMT

### Changes

- Upgrade just-scripts and add root test script (ngerlem@microsoft.com)
- Add descriptions to dev menu items (acoates@microsoft.com)
## 0.61.0-beta.29
Mon, 02 Mar 2020 20:59:31 GMT

### Changes

- Fix a couple of places in run-windows that has incorrect assumptions on install layout (acoates@microsoft.com)
## 0.61.0-beta.28
Thu, 27 Feb 2020 23:11:13 GMT

### Changes

- Set IsTabStop to false when tabIndex is negative (lamdoan@microsoft.com)
- TextBox should have a default tabIndex of 0 (lamdoan@microsoft.com)
## 0.61.0-beta.27
Wed, 26 Feb 2020 18:33:35 GMT

### Changes

- Fixed call of methods with two callbacks (vmorozov@microsoft.com)
## 0.61.0-beta.26
Wed, 26 Feb 2020 17:21:19 GMT

### Changes

- Standup first TurboModule (acoates@microsoft.com)
## 0.61.0-beta.25
Wed, 26 Feb 2020 10:17:18 GMT

### Changes

- Fix concurrency issue in NetworkingModule (lamdoan@microsoft.com)
## 0.61.0-beta.24
Tue, 25 Feb 2020 21:50:32 GMT

### Changes

- Plumb directdebugging options through debug menu and instance loading (acoates@microsoft.com)
## 0.61.0-beta.23
Tue, 25 Feb 2020 03:42:21 GMT

### Changes

- Declare MessageQueueThread in header that uses it (acoates@microsoft.com)
## 0.61.0-beta.22
Sat, 22 Feb 2020 00:21:17 GMT

### Changes

- Force coroutine method parameters pass by value (vmorozov@microsoft.com)
## 0.61.0-beta.21
Fri, 21 Feb 2020 23:40:21 GMT

### Changes

- Add a Win32 playground app to RNW (hpratt@microsoft.com)
## 0.61.0-beta.20
Fri, 21 Feb 2020 16:46:52 GMT

### Changes

- Fix ReactNativeHost without package providers (vmorozov@microsoft.com)
## 0.61.0-beta.19
Fri, 21 Feb 2020 01:27:13 GMT

### Changes

- Fix some reliability isssues for current AsyncStorage implementation (dida@ntdev.microsoft.com)
- Upgrade ChakraCore Debugger to 0.0.0.43 (julio.rocha@microsoft.com)
- Update metro configs to support node 12 on windows (acoates)
## 0.61.0-beta.17
Thu, 20 Feb 2020 19:20:15 GMT

### Changes

- Bump min. Windows Desktop supported version to 8. (julio.rocha@microsoft.com)
## 0.61.0-beta.16
Thu, 20 Feb 2020 16:09:49 GMT

### Changes

- Avoid having native modules without methods (vmorozov@microsoft.com)
## 0.61.0-beta.15
Thu, 20 Feb 2020 01:52:30 GMT

### Changes

- Use verdaccio to validate react native init (acoates@microsoft.com)
## 0.61.0-beta.14
Wed, 19 Feb 2020 23:34:40 GMT

### Changes

- Implemented NativeModule initialization with IReactContext (vmorozov@microsoft.com)
## 0.61.0-beta.13
Wed, 19 Feb 2020 22:45:15 GMT

### Changes

- Revert "Drop StdCall in favor of Cdecl for x86. (#4115)" (ngerlem@microsoft.com)
## 0.61.0-beta.12
Wed, 19 Feb 2020 07:10:21 GMT

### Changes

- Use Cdecl for x86. (julio.rocha@microsoft.com)
## 0.61.0-beta.11
Wed, 19 Feb 2020 02:24:03 GMT

### Changes

- Remove useIncrementalUpdates assert from Networking module (mcota@microsoft.com)
## 0.61.0-beta.10
Tue, 18 Feb 2020 21:13:20 GMT

### Changes

- Fix issues with relative IntDir breaking builds (aschultz@microsoft.com)
## 0.61.0-beta.9
Sat, 15 Feb 2020 01:46:09 GMT

### Changes

- update devDependencies to use latest TS types (kmelmon@microsoft.com)
## 0.61.0-beta.8
Fri, 14 Feb 2020 21:40:52 GMT

### Changes

- Downngraded OpenSSL to 1.0.2-p.2. (julio.rocha@microsoft.com)
## 0.61.0-beta.7
Thu, 13 Feb 2020 21:30:14 GMT

### Changes

- Force Interpretation of Code as UTF-8 When no BOM Present (ngerlem@microsoft.com)
## 0.61.0-beta.6
Thu, 13 Feb 2020 05:48:00 GMT

### Changes

- Fixed Microsoft.ReactNative compiler warnings and start treating warnings as errors (vmorozov@microsoft.com)
## 0.61.0-beta.5
Thu, 13 Feb 2020 01:37:12 GMT

### Changes

- Set doc generation to output to new path. Update docs to include recent changes. (aschultz@microsoft.com)
## 0.61.0-beta.4
Wed, 12 Feb 2020 21:40:06 GMT

### Changes

- Fixed Developer menu and Live Reaload (vmorozov@microsoft.com)
## 0.61.0-beta.3
Wed, 12 Feb 2020 11:16:52 GMT

### Changes

- Provide a Minimal PlatformConstants Implementation (ngerlem@microsoft.com)
## 0.61.0-beta.2
Wed, 12 Feb 2020 00:46:46 GMT

### Changes

- Fix scroll indicators being shown when not required (jagorrin@microsoft.com)
- Added async code to native module test methods (vmorozov@microsoft.com)
## 0.61.0-beta.1
Tue, 11 Feb 2020 01:21:44 GMT

### Changes

- Fix Dimension returning DIP instead of physical pixels (dida@ntdev.microsoft.com)
- Added Microsoft.ReactNative dependency on Mso (vmorozov@microsoft.com)
- Merge IntegrationTests and RNTester with Stock React Native 60.0.6 (nick@nickgerleman.com)
- Add Flow Check Suppressions (nick@nickgerleman.com)
- Export loadScriptFromString from react-native-win32 (acoates@microsoft.com)
- Export Instance->getModuleRegistry from react-native-win32.dll (acoates@microsoft.com)
- More Patches, IntegrationTests and RNTester Cleanup (nick@nickgerleman.com)
- Use unforked react-native 0.60.6 (nick@nickgerleman.com)
- Implemented ReactHost for Microsoft.ReactNative (vmorozov@microsoft.com)
- Implemented ReactPromise<void> (vmorozov@microsoft.com)
- Enable screen reader announcements for accessibilityLiveRegion (jagorrin@microsoft.com)
- Enabled C++/WinRT heap enforcement (vmorozov@microsoft.com)
- upgrade to 61 (kmelmon@microsoft.com)
## 0.60.0-vnext.133
Thu, 30 Jan 2020 23:29:21 GMT

### Changes

- Allow Any Prerelease Version 0f 0.60 as Peer Dependency (nick@nickgerleman.com)
## 0.60.0-vnext.132
Thu, 30 Jan 2020 22:08:51 GMT

### Changes

- Simple fixes to IReactContext.cpp and add a writer helper function for direct event type constant (dida@ntdev.microsoft.com)
## 0.60.0-vnext.131
Thu, 30 Jan 2020 21:10:31 GMT

### Changes

- Add callstack to the error message when a JS error occurs. (kinhln@microsoft.com)
## 0.60.0-vnext.130
Thu, 30 Jan 2020 09:17:27 GMT

### Changes

- Add as Windows Overrides (nick@nickgerleman.com)
## 0.60.0-vnext.129
Thu, 30 Jan 2020 01:22:53 GMT

### Changes

- Share RCTNetworking Library Between windows and windesktop (nick@nickgerleman.com)
## 0.60.0-vnext.128
Wed, 29 Jan 2020 01:55:04 GMT

### Changes

- Fix Crash When Running CLI Projects (nick@nickgerleman.com)
## 0.60.0-vnext.127
Tue, 28 Jan 2020 18:08:35 GMT

### Changes

- Make the native build oblivous to the differences between microsoft/react-native and facebook/react-native with patches (nick@nickgerleman.com)
## 0.60.0-vnext.126
Tue, 28 Jan 2020 16:46:27 GMT

### Changes

- Add Yoga Patch from microsoft/react-native (nick@nickgerleman.com)
## 0.60.0-vnext.125
Tue, 28 Jan 2020 06:50:47 GMT

### Changes

- copy IntegrationTests from fork (kmelmon@microsoft.com)
## 0.60.0-vnext.124
Sat, 25 Jan 2020 21:45:06 GMT

### Changes

- Removed pch.h from shared items projects (vmorozov@microsoft.com)
## 0.60.0-vnext.123
Fri, 24 Jan 2020 22:33:56 GMT

### Changes

- Fork RNTester script and update 'React' to 'react' (email not defined)
## 0.60.0-vnext.122
Fri, 24 Jan 2020 21:21:47 GMT

### Changes

- Update to react-native@0.60.0-microsoft.40 (acoates@microsoft.com)
## 0.60.0-vnext.121
Thu, 23 Jan 2020 18:42:26 GMT

### Changes

- Logging and Tracing OSS_RN Removal (nick@nickgerleman.com)
## 0.60.0-vnext.120
Thu, 23 Jan 2020 06:39:59 GMT

### Changes

- Update Folly Build Logic (nick@nickgerleman.com)
## 0.60.0-vnext.119
Thu, 23 Jan 2020 00:16:13 GMT

### Changes

- Apply React Native Patches for All Consumers of ReackPackageDirectories (nick@nickgerleman.com)
## 0.60.0-vnext.118
Wed, 22 Jan 2020 07:24:31 GMT

### Changes

- Remove OSS_RN Specialization in unistd.h Stub (nick@nickgerleman.com)
## 0.60.0-vnext.117
Wed, 22 Jan 2020 03:15:49 GMT

### Changes

- Added ARM64 variant to Universal projects. (julio.rocha@microsoft.com)
- Added Mso library required for ReactHost (vmorozov@microsoft.com)
## 0.60.0-vnext.116
Wed, 22 Jan 2020 01:31:33 GMT

### Changes

- VM: Adding support for JSValue for ViewManager APIs (jthysell@microsoft.com)
- Upgrade OpenSSL NuGet to 1.1.1-d.2. (julio.rocha@microsoft.com)
## 0.60.0-vnext.115
Thu, 16 Jan 2020 05:34:40 GMT

### Changes

- Add initial set of React Native Patches (nick@nickgerleman.com)
## 0.60.0-vnext.114
Thu, 16 Jan 2020 03:02:14 GMT

### Changes

- convert haste imports to use relative requires (kmelmon@microsoft.com)
## 0.60.0-vnext.113
Wed, 15 Jan 2020 21:22:45 GMT

### Changes

- Upgrade to OpenSSL 1.1.1d/0. (julio.rocha@microsoft.com)
## 0.60.0-vnext.112
Thu, 09 Jan 2020 23:20:10 GMT

### Changes

- Header Refactoring Around jsireact to Unblock ms/rn (nick@nickgerleman.com)
## 0.60.0-vnext.111
Thu, 09 Jan 2020 22:06:05 GMT

### Changes

- Add GetFacebookReactInstance :: uwpInstance -> react::Instance (zihanc@microsoft.com)
- Added unit tests for Events with JSValue args (jthysell@microsoft.com)
## 0.60.0-vnext.110
Wed, 08 Jan 2020 23:03:25 GMT

### Changes

- Image - basic svg support (mcota@microsoft.com)
## 0.60.0-vnext.109
Wed, 08 Jan 2020 01:43:57 GMT

### Changes

- Fixing bugs when using .NetNative (jthysell@microsoft.com)
## 0.60.0-vnext.108
Tue, 07 Jan 2020 19:44:17 GMT

### Changes

- Fix RNTester bundle (#2728) (acoates@microsoft.com)
## 0.60.0-vnext.107
Mon, 06 Jan 2020 21:29:21 GMT

### Changes

- Call StartAnimatiom on m_scaleCombined for ScaleX / ScaleY animations (thshea@microsoft.com)
## 0.60.0-vnext.106
Mon, 30 Dec 2019 22:14:37 GMT

### Changes

- Avoid app crash because content is not bundled for the first time (licanhua@live.com)
## 0.60.0-vnext.105
Fri, 27 Dec 2019 17:01:20 GMT

### Changes

- Changed Microsoft.ReactNative to be independent from ReactUWP project (vmorozov@microsoft.com)
## 0.60.0-vnext.104
Sat, 21 Dec 2019 03:41:33 GMT

### Changes

- Export ability to query native module names (ngerlem@microsoft.com)
## 0.60.0-vnext.103
Sat, 21 Dec 2019 00:18:22 GMT

### Changes

- Fix RNTester bundle (#2728) (acoates@microsoft.com)
## 0.60.0-vnext.102
Fri, 20 Dec 2019 19:01:50 GMT

### Changes

- Provide source uri in SourceCode module when using livereload (acoates@microsoft.com)
## 0.60.0-vnext.101
Fri, 20 Dec 2019 18:19:02 GMT

### Changes

- Get flow clean, and turn on flow check during build (acoates@microsoft.com)
## 0.60.0-vnext.100
Fri, 20 Dec 2019 05:18:08 GMT

### Changes

- Fix cornerRadius issue for TextInput and some updates to TreeDump (dida@ntdev.microsoft.com)
## 0.60.0-vnext.99
Thu, 19 Dec 2019 18:44:51 GMT

### Changes

- Removed Bridge sub-namespace in favor of Microsoft.ReactNative (vmorozov@microsoft.com)
## 0.60.0-vnext.98
Wed, 18 Dec 2019 21:47:15 GMT

### Changes

- port windowsbrush changes from Microsoft fork of react-native (kmelmon@microsoft.com)
## 0.60.0-vnext.97
Wed, 18 Dec 2019 01:40:40 GMT

### Changes

- Conditionally use BitmapImage (email not defined)
## 0.60.0-vnext.96
Mon, 16 Dec 2019 21:22:31 GMT

### Changes

- TreeDump for E2E test and fix for image border issue (dida@ntdev.microsoft.com)
## 0.60.0-vnext.95
Fri, 13 Dec 2019 17:32:38 GMT

### Changes

- Fix toggle debugger setting issue with ReactApplication (dida@ntdev.microsoft.com)
## 0.60.0-vnext.94
Thu, 12 Dec 2019 22:18:13 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.31 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.93
Thu, 12 Dec 2019 00:17:06 GMT

### Changes

- Strongly typed value serialization and deserialization using IJSValueReader, JSValue, and IJSValueWriter (vmorozov@microsoft.com)
## 0.60.0-vnext.92
Wed, 11 Dec 2019 23:31:47 GMT

### Changes

- Submitting the ETW schema resouce dll and the register script (anandrag@microsoft.com)
## 0.60.0-vnext.91
Mon, 09 Dec 2019 18:40:45 GMT

### Changes

- Aligning Microsoft.Windows.CppWinRT Versions (jthysell@microsoft.com)
## 0.60.0-vnext.90
Fri, 06 Dec 2019 21:56:22 GMT

### Changes

- Update E2ETest to use ReactApplication (dida@ntdev.microsoft.com)
## 0.60.0-vnext.89
Fri, 06 Dec 2019 19:14:55 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.28 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.88
Fri, 06 Dec 2019 17:14:06 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.26 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.87
Thu, 05 Dec 2019 19:54:18 GMT

### Changes

- Add support for custom ViewManagers with children (jthysell@microsoft.com)
## 0.60.0-vnext.86
Tue, 03 Dec 2019 23:21:34 GMT

### Changes

- Add WinUI (licanhua@live.com)
## 0.60.0-vnext.85
Mon, 02 Dec 2019 17:34:30 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.24 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.84
Tue, 26 Nov 2019 00:13:20 GMT

### Changes

- View Manager Event Support (jthysell@microsoft.com)
## 0.60.0-vnext.83
Mon, 25 Nov 2019 21:54:38 GMT

### Changes

- Properly support ScrollView KeyboardDismissMode (dida@ntdev.microsoft.com)
## 0.60.0-vnext.82
Mon, 25 Nov 2019 21:23:52 GMT

### Changes

- Revert "Add Winui 2.2 framework package (#3622)" (licanhua@live.com)
## 0.60.0-vnext.81
Sat, 23 Nov 2019 08:02:20 GMT

### Changes

- Add WinUI (licanhua@live.com)
- Support ScrollView pagingEnabled (dida@ntdev.microsoft.com)
- Add react-native run-windows --info (licanhua@live.com)
- manually bump to vnext.80 (kmelmon@microsoft.com)
## 0.60.0-vnext.79
Fri, 22 Nov 2019 20:45:17 GMT

### Changes

- Commit ecd4456c... was wrong. `ABI::Windows::ApplicationModel::IPackage::get_Current` will, when called without package identity, return `HRESULT_FROM_WIN32(APPMODEL_ERROR_NO_PACKAGE)`, not just `APPMODEL_ERROR_NO_PACKAGE` (which is a win32 error code, not an hresult). 🤦‍♂️ (hpratt@microsoft.com)
## 0.60.0-vnext.78
Fri, 22 Nov 2019 04:47:41 GMT

### Changes

- remove ReleaseBundle and DebugBundle (licanhua@live.com)
## 0.60.0-vnext.77
Fri, 22 Nov 2019 00:48:20 GMT

### Changes

- Support keyboardDismissMode on-drag for ScrollView (dida@ntdev.microsoft.com)
## 0.60.0-vnext.76
Thu, 21 Nov 2019 21:53:08 GMT

### Changes

- Merge remote-tracking branch 'upstream/master' (hpratt@microsoft.com)
## 0.60.0-vnext.75
Thu, 21 Nov 2019 01:46:31 GMT

### Changes

- Fix layout children position issue when outer border exists (dida@ntdev.microsoft.com)
- Updating react-native to version: 0.60.0-microsoft.23 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.74
Wed, 20 Nov 2019 18:56:27 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.22 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.73
Tue, 19 Nov 2019 22:36:29 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.21 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.72
Mon, 18 Nov 2019 22:41:41 GMT

### Changes

- Stop requiring Platform.cpp from MS/RN (acoates@microsoft.com)
## 0.60.0-vnext.71
Fri, 15 Nov 2019 18:17:47 GMT

### Changes

- Add TurboModule and JsCallInvoker files to ReactCommon only when targeting to microsoft/react-native (zihanc@microsoft.com)
## 0.60.0-vnext.70
Tue, 12 Nov 2019 04:50:06 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.20 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.69
Mon, 11 Nov 2019 22:01:18 GMT

### Changes

- Workaround for flyout centering problem (kenander@microsoft.com)
## 0.60.0-vnext.68
Sat, 09 Nov 2019 01:27:15 GMT

### Changes

- Native Module Setup Guide * Added NativeModuleSetup.md, #3623 * Added link to new guide in NativeModules.md, ViewManagers.md * Updated ProjectStructure.md with Microsoft.ReactNative projects * Removed PropertySheets in SampleApps and CLI solutions (jthysell@microsoft.com)
## 0.60.0-vnext.67
Fri, 08 Nov 2019 00:30:50 GMT

### Changes

- ViewManager Command Updates * Added command to CustomUserControlViewManagerCPP, #3600 * Added support for simpler C# view manager commands signatures * Added support for object properties for view managers, #3613 * Added documentation for C++ view manager * Added documentation for ViewManager commands, #3599 (jthysell@microsoft.com)
## 0.60.0-vnext.66
Thu, 07 Nov 2019 17:37:11 GMT

### Changes

- Support refreshControl (dida@ntdev.microsoft.com)
## 0.60.0-vnext.65
Wed, 06 Nov 2019 13:58:14 GMT

### Changes

- Merging upstream changes (anandrag@microsoft.com)
## 0.60.0-vnext.64
Wed, 06 Nov 2019 06:59:22 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.16 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.63
Wed, 06 Nov 2019 01:56:47 GMT

### Changes

- Native Modules: Updated samples and docs to show how to use events (jthysell@microsoft.com)
## 0.60.0-vnext.62
Tue, 05 Nov 2019 23:11:52 GMT

### Changes

- VM: Split up IViewManager into optional interfaces #3565 (jthysell@microsoft.com)
## 0.60.0-vnext.61
Fri, 01 Nov 2019 23:40:12 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.15 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.60
Fri, 01 Nov 2019 21:25:27 GMT

### Changes

- Support snapToStart and snapToEnd for ScrollView (dida@ntdev.microsoft.com)
## 0.60.0-vnext.59
Fri, 01 Nov 2019 18:09:07 GMT

### Changes

- Call native animation callbacks only once (asklar@microsoft.com)
## 0.60.0-vnext.58
Thu, 31 Oct 2019 18:11:20 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.14 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.57
Thu, 31 Oct 2019 16:59:45 GMT

### Changes

- Add todo stub for FacadeType::Progress in AnimatedNode (adamgor@microsoft.com)
## 0.60.0-vnext.56
Wed, 30 Oct 2019 22:56:11 GMT

### Changes

- Set a default 3D perspective (asklar@microsoft.com)
## 0.60.0-vnext.55
Wed, 30 Oct 2019 17:12:12 GMT

### Changes

- fix build error (enm10k@gmail.com)
## 0.60.0-vnext.54
Tue, 29 Oct 2019 23:29:07 GMT

### Changes

- Fix crash in RS5 due to missing facade property (asklar@winse.microsoft.com)
## 0.60.0-vnext.53
Tue, 29 Oct 2019 19:36:34 GMT

### Changes

- Support Keyboard events (dida@ntdev.microsoft.com)
## 0.60.0-vnext.52
Tue, 29 Oct 2019 01:31:27 GMT

### Changes

- Update the CLI templates to use Microsoft.ReactNative (jthysell@microsoft.com)
## 0.60.0-vnext.51
Mon, 28 Oct 2019 23:51:07 GMT

### Changes

- Move closer to allowing JSI with OSS RN (acoates@microsoft.com)
## 0.60.0-vnext.50
Sat, 26 Oct 2019 21:50:14 GMT

### Changes

- Fix remote debugger for Playground app. Now you can pass  the hostname:port of a running metro bundler instance and it will connect correctly (asklar@winse.microsoft.com)
## 0.60.0-vnext.49
Sat, 26 Oct 2019 02:55:31 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.13 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.48
Fri, 25 Oct 2019 23:07:25 GMT

### Changes

- Custom View Managers E2E (jthysell@microsoft.com)
## 0.60.0-vnext.47
Fri, 25 Oct 2019 21:43:48 GMT

### Changes

- go back to native implementation for getPropertyNames (stecrain@microsoft.com)
## 0.60.0-vnext.46
Fri, 25 Oct 2019 20:56:10 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.12 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.45
Fri, 25 Oct 2019 01:44:45 GMT

### Changes

- make run-windows work on x86 machines (asklar@winse.microsoft.com)
## 0.60.0-vnext.44
Thu, 24 Oct 2019 18:04:40 GMT

### Changes

- fix assert/missing else when setting alignSelf to null (adamgor@microsoft.com)
## 0.60.0-vnext.43
Thu, 24 Oct 2019 09:06:03 GMT

### Changes

- Register 2x NYI stubs for NativeAnimatedModule (adamgor@microsoft.com)
## 0.60.0-vnext.42
Wed, 23 Oct 2019 20:38:22 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.11 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.41
Tue, 22 Oct 2019 03:28:58 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.9 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.40
Tue, 22 Oct 2019 01:01:42 GMT

### Changes

- Fix build issue with react-native run-windows (kmelmon@microsoft.com)
## 0.60.0-vnext.39
Fri, 18 Oct 2019 21:22:01 GMT

### Changes

- Fix run-windows app installation hang (dida@ntdev.microsoft.com)
## 0.60.0-vnext.38
Fri, 18 Oct 2019 20:36:35 GMT

### Changes

- add developer menu property (kmelmon@microsoft.com)
## 0.60.0-vnext.37
Thu, 17 Oct 2019 23:10:13 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.8 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.36
Thu, 17 Oct 2019 20:58:22 GMT

### Changes

- Fix two issues: 1) you cannot animated 2 subchannels of the same property with different animations. to fix this we animated yet another property set for translation and scale owned by the props nodes and use one animation to animate all of the subchannels for the uiElement. 2) Reference parameter names which started with a multi digit number are unsupported so i added an n to the start of each name, which was previously just the node's tag. (stpete@microsoft.com)
## 0.60.0-vnext.35
Thu, 17 Oct 2019 16:18:52 GMT

### Changes

- Support autoCapitalize Characters (dida@ntdev.microsoft.com)
## 0.60.0-vnext.34
Thu, 17 Oct 2019 02:48:28 GMT

### Changes

- Support onSubmitEditing event (dida@ntdev.microsoft.com)
## 0.60.0-vnext.33
Thu, 17 Oct 2019 00:14:32 GMT

### Changes

- exclude Generated Files (kmelmon@microsoft.com)
## 0.60.0-vnext.32
Wed, 16 Oct 2019 02:19:39 GMT

### Changes

- Extend flyout shadow fix to Popups (kenander@microsoft.com)
## 0.60.0-vnext.31
Tue, 15 Oct 2019 23:45:52 GMT

### Changes

- Reduced and simplified sample native modules (jthysell@microsoft.com)
## 0.60.0-vnext.30
Tue, 15 Oct 2019 22:35:30 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.7 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.29
Tue, 15 Oct 2019 13:17:22 GMT

### Changes

- dont new up exceptions (adamgor@microsoft.com)
## 0.60.0-vnext.28
Mon, 14 Oct 2019 23:02:23 GMT

### Changes

- Microsoft.ReactNative attribute-based NativeModules (jthysell@microsoft.com)
## 0.60.0-vnext.27
Sat, 12 Oct 2019 16:49:47 GMT

### Changes

- add tests (kmelmon@microsoft.com)
## 0.60.0-vnext.26
Fri, 11 Oct 2019 22:15:37 GMT

### Changes

- add Common.vcxproj to CLI solutions (kmelmon@microsoft.com)
## 0.60.0-vnext.25
Fri, 11 Oct 2019 20:47:08 GMT

### Changes

- Support caretHidden (dida@ntdev.microsoft.com)
## 0.60.0-vnext.24
Fri, 11 Oct 2019 10:21:12 GMT

### Changes

- add support for offline bundles (kmelmon@microsoft.com)
## 0.60.0-vnext.23
Wed, 09 Oct 2019 18:17:42 GMT

### Changes

- Add support for forever animations. (stpete@microsoft.com)
## 0.60.0-vnext.22
Wed, 09 Oct 2019 15:13:21 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.5 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.21
Tue, 08 Oct 2019 18:33:16 GMT

### Changes

- re-enable Chakra Core on UWP (stecrain@microsoft.com)
## 0.60.0-vnext.20
Mon, 07 Oct 2019 21:46:39 GMT

### Changes

- Bump V8 and Hermes JSI package versions (tudorm@microsoft.com)
## 0.60.0-vnext.19
Fri, 04 Oct 2019 19:04:20 GMT

### Changes

- Update ChakraCore to v1.11.13 (acoates@microsoft.com)
## 0.60.0-vnext.18
Fri, 04 Oct 2019 16:32:45 GMT

### Changes

- Update to react-native@0.60.0-microsoft.3 (acoates@microsoft.com)
## 0.60.0-vnext.17
Thu, 03 Oct 2019 22:17:13 GMT

### Changes

- change file (randyfl@microsoft.com)
- Support contextMenuHidden (dida@ntdev.microsoft.com)
- Remove react-native as a haste root, as all the files should be in react-native-windows (acoates@microsoft.com)
- Connect DeviceInfo dimensions with root element (dida@ntdev.microsoft.com)
## 0.60.0-vnext.16
Tue, 01 Oct 2019 18:17:42 GMT

### Changes

- Turn ControlFlowGuard on by default (acoates@microsoft.com)
- The customer provided examples revealed a few of issues.  One, we were dubble counding the starting value of animatiosn in some cases. Two we were incorrectly relying on JS to tell us to flatten our animated values. Three we were detaching the expression animations that tied the property to the UIElement and in certain cases the JS thread does not inform us that we need to rebuild this cuppling.  There is an open issue with this final part #3280 (stpete@microsoft.com)
## 0.60.0-vnext.11
Mon, 30 Sep 2019 18:28:23 GMT

### Changes

- Update to folly 2019.09.30.00 (acoates@microsoft.com)
## 0.60.0-vnext.10
Mon, 30 Sep 2019 16:21:08 GMT

### Changes

- Bump up to latest published version. (#3275) (julio.rocha@microsoft.com)
## 0.60.0-vnext.9
Sat, 28 Sep 2019 01:21:33 GMT

### Changes

- add support for cpp template app (email not defined)
## 0.60.0-vnext.8
Sat, 28 Sep 2019 00:21:32 GMT

### Changes

- Fix build break when using latest Visual Studio compiler versions (acoates@microsoft.com)
## 0.60.0-vnext.7
Fri, 27 Sep 2019 17:40:33 GMT

### Changes

- Allow focus transfer to Flyout on Open (kenander@microsoft.com)
## 0.60.0-vnext.6
Fri, 27 Sep 2019 17:34:59 GMT

### Changes

- change file (randyfl@microsoft.com)
## 0.60.0-vnext.5
Fri, 27 Sep 2019 00:08:28 GMT

### Changes

- Allow focus transfer to Flyout on Open (kenander@microsoft.com)
## 0.60.0-vnext.4
Wed, 25 Sep 2019 18:32:05 GMT

### Changes

- Enabling Systrace/ETW plumbing by default. The tracing will be controlled by whether the provider is enabled or not (anandrag@microsoft.com)
## 0.60.0-vnext.3
Tue, 24 Sep 2019 23:29:18 GMT

### Changes

- Create stub StatusBarManager to avoid crash when using <StatusBar> (acoates@microsoft.com)
## 0.60.0-vnext.2
Tue, 24 Sep 2019 19:11:20 GMT

### Changes

- Update to react-native 0.60 (acoates@microsoft.com)
## 0.59.0-vnext.190
Thu, 19 Sep 2019 17:02:46 GMT

### Changes

- Support keyboardType for TextInput (dida@ntdev.microsoft.com)
## 0.59.0-vnext.189
Wed, 18 Sep 2019 18:18:37 GMT

### Changes

- Add the SpringAnimationDriver (stpete@microsoft.com)
## 0.59.0-vnext.188
Tue, 17 Sep 2019 17:42:42 GMT

### Changes

- Refined logic for handling Flyout shadows (kenander@microsoft.com)
## 0.59.0-vnext.187
Tue, 17 Sep 2019 11:32:39 GMT

### Changes

- Simplify NuGet packing YAML. (julio@rochsquadron.net)
## 0.59.0-vnext.186
Tue, 17 Sep 2019 05:17:28 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.82 (53619745+rnbot@users.noreply.github.com)
## 0.59.0-vnext.185
Sat, 14 Sep 2019 14:09:03 GMT

### Changes

- Add PackageCertificatePassword attribute to MyApp.csproj to solve the build issue in VS2019 due to temp pfx certificate issue (dida@ntdev.microsoft.com)
## 0.59.0-vnext.184
Fri, 13 Sep 2019 21:56:22 GMT

### Changes

- Support live reload for developer menu (dida@ntdev.microsoft.com)
## 0.59.0-vnext.183
Fri, 13 Sep 2019 15:59:25 GMT

### Changes

- MultiLine TextInput should accepts Enter key (dida@ntdev.microsoft.com)
## 0.59.0-vnext.182
Thu, 12 Sep 2019 23:51:35 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.79 (53619745+rnbot@users.noreply.github.com)
- Updating react-native to version: 0.59.0-microsoft.80 (53619745+rnbot@users.noreply.github.com)
## 0.59.0-vnext.179
Wed, 11 Sep 2019 18:44:17 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.77 (53619745+rnbot@users.noreply.github.com)
## 0.59.0-vnext.178
Tue, 10 Sep 2019 23:25:37 GMT

### Changes

- Fix onPress not firing for Touchable* keyboard events (thshea@microsoft.com)
## 0.59.0-vnext.177
Tue, 10 Sep 2019 19:16:08 GMT

### Changes

- Fix Login E2E test failure (dida@ntdev.microsoft.com)
## 0.59.0-vnext.176
Tue, 10 Sep 2019 00:44:04 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.74 (53619745+rnbot@users.noreply.github.com)
## 0.59.0-vnext.175
Mon, 09 Sep 2019 21:39:46 GMT

### Changes

- Alphabetize def files. (yicyao@microsoft.com)
## 0.59.0-vnext.174
Mon, 09 Sep 2019 19:22:09 GMT

### Changes

- Put desktop unit tests into namespaces. (yicyao@microsoft.com)
## 0.59.0-vnext.173
Sat, 07 Sep 2019 20:17:19 GMT

### Changes

- Support dynamic swapping TextBox/PasswordBox view for SecureTextEntry property (dida@ntdev.microsoft.com)
## 0.59.0-vnext.172
Fri, 06 Sep 2019 18:03:18 GMT

### Changes

- Fix issue with using getInt instead of asDouble for JSI (jthysell@microsoft.com)
## 0.59.0-vnext.171
Thu, 05 Sep 2019 17:37:33 GMT

### Changes

- ReactUWP def cleanup and unit test fixes (jthysell@microsoft.com)
## 0.59.0-vnext.170
Tue, 03 Sep 2019 21:49:49 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.73 (53619745+rnbot@users.noreply.github.com)
## 0.59.0-vnext.169
Tue, 03 Sep 2019 20:24:37 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.72 (53619745+rnbot@users.noreply.github.com)
## 0.59.0-vnext.168
Fri, 30 Aug 2019 23:08:02 GMT

### Changes

- set IsTabStop = false for SnapPointManagingContentControl (stecrain@microsoft.com)
## 0.59.0-vnext.167
Fri, 30 Aug 2019 20:30:25 GMT

### Changes

- fix double-apply TransformMatrix bug (email not defined)
## 0.59.0-vnext.166
Fri, 30 Aug 2019 20:17:20 GMT

### Changes

- changed alert export (email not defined)
## 0.59.0-vnext.165
Fri, 30 Aug 2019 19:46:18 GMT

### Changes

- Fixed sampleapps build and updated CI (jthysell@microsoft.com)
## 0.59.0-vnext.164
Thu, 29 Aug 2019 22:14:06 GMT

### Changes

- changed alert export (email not defined)
## 0.59.0-vnext.163
Thu, 29 Aug 2019 20:21:10 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.71 (53619745+rnbot@users.noreply.github.com)
- change file (randyfl@microsoft.com)
## 0.59.0-vnext.162
Thu, 29 Aug 2019 19:38:17 GMT

### Changes

- Allow windowsbrush: color syntax to use non-SolidColor brushes (thshea@microsoft.com)
## 0.59.0-vnext.161
Thu, 29 Aug 2019 01:41:01 GMT

### Changes

- Use PreparedScriptStore for V8 in win32 (tudorm@microsoft.com)
## 0.59.0-vnext.160
Thu, 29 Aug 2019 01:33:54 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.70 (53619745+rnbot@users.noreply.github.com)
## 0.59.0-vnext.159
Wed, 28 Aug 2019 19:55:48 GMT

### Changes

- Added React.Windows.Desktop.Test project. (julio@rochsquadron.net)
- Add isOverlayEnabled (cknestri@microsoft.com)
## 0.59.0-vnext.158
Wed, 28 Aug 2019 18:34:51 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.69 (53619745+rnbot@users.noreply.github.com)
## 0.59.0-vnext.157
Wed, 28 Aug 2019 02:44:14 GMT

### Changes

- Let View process color objects (email not defined)
## 0.59.0-vnext.156
Tue, 27 Aug 2019 04:41:01 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.68 (53619745+rnbot@users.noreply.github.com)
## 0.59.0-vnext.155
Mon, 26 Aug 2019 21:10:00 GMT

### Changes

- Enable experimental V8 JSI Executor support (off by default) (tudorm@microsoft.com)
## 0.59.0-vnext.154
Sat, 24 Aug 2019 15:32:08 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.67 (53619745+rnbot@users.noreply.github.com)
## 0.59.0-vnext.153
Sat, 24 Aug 2019 04:24:35 GMT

### Changes

- Fix change log formatting (acoates@microsoft.com)
## 0.59.0-vnext.152
Sat, 24 Aug 2019 04:09:20 GMT

### Changes

- move cppwinrt less exception helper, use in DevSupportManager (andrewh@microsoft.com)
## 0.59.0-vnext.151
Fri, 23 Aug 2019 01:27:19 GMT

### Changes

- turn off RTTI (email not defined)

## 0.59.0-vnext.150
Thu, 22 Aug 2019 21:17:45 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.49 (53619745+rnbot@users.noreply.github.com)

## 0.59.0-vnext.149
Thu, 22 Aug 2019 15:43:48 GMT

### Changes

- Improve Networking error reporting (andrewh@microsoft.com)

## 0.59.0-vnext.148
Wed, 21 Aug 2019 20:44:12 GMT

### Changes

- Only send focus/blur from OriginalSource, remove Click event for enter/space (andrewh@microsoft.com)

## 0.59.0-vnext.147
Wed, 21 Aug 2019 06:12:54 GMT

### Changes

- add default message queue that uses CoreDispatcher (email not defined)

## 0.59.0-vnext.146
Wed, 21 Aug 2019 00:42:37 GMT

### Changes

- revert fix, not quite working (email not defined)

## 0.59.0-vnext.145
Tue, 20 Aug 2019 23:40:14 GMT

### Changes

- Light up Inspector debugging tool. (kinhln@microsoft.com)
- Remove old playground app (acoates@microsoft.com)

## 0.59.0-vnext.144
Tue, 20 Aug 2019 21:54:07 GMT

### Changes

- Minor fixes made in RN60 update that can be brought right to master (acoates@microsoft.com)

## 0.59.0-vnext.143
Tue, 20 Aug 2019 07:26:49 GMT

### Changes

- Don't use DefaultNativeMessageQueueThread in live reload (andrewh@microsoft.com)

## 0.59.0-vnext.142
Tue, 20 Aug 2019 03:02:53 GMT

### Changes

- add additional new warning disable to MsoFolly.h (andrewh@microsoft.com)

## 0.59.0-vnext.141
Mon, 19 Aug 2019 20:59:57 GMT

### Changes

- fix flyout full placement mode (kmelmon@microsoft.com)

## 0.59.0-vnext.140
Mon, 19 Aug 2019 16:43:43 GMT

### Changes

- Update placement mapping (cknestri@microsoft.com)
- Start API documentation (acoates@microsoft.com)
- Always use unforked version of folly + Update folly to v2019.08.12.00 (acoates@microsoft.com)
- resolved build error (email not defined)
- Improved fix for overlapping XAML theme shadows on Flyouts (kenander@microsoft.com)
- add batching queue (kmelmon@microsoft.com)
- restore accidental deletion (kmelmon@microsoft.com)
- Microsoft.ReactNative SampleApps (jthysell@microsoft.com)

## 0.59.0-vnext.139
Tue, 13 Aug 2019 19:56:52 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.43 (53619745+rnbot@users.noreply.github.com)
- Change windows template to not use cli wrapper (acoates@microsoft.com)

## 0.59.0-vnext.138
Mon, 12 Aug 2019 23:13:59 GMT

### Changes

- Rename platform uwp->windows (acoates@microsoft.com)

## 0.59.0-vnext.137
Mon, 12 Aug 2019 21:06:24 GMT

### Changes

- Improve output of run-windows command (acoates@microsoft.com)

## 0.59.0-vnext.136
Mon, 12 Aug 2019 20:02:04 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.42 (53619745+rnbot@users.noreply.github.com)

## 0.59.0-vnext.135
Sun, 11 Aug 2019 04:57:41 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.41 (53619745+rnbot@users.noreply.github.com)

## 0.59.0-vnext.134
Sat, 10 Aug 2019 19:21:55 GMT

### Changes

- Fix switch shimmy issue (ruaraki@microsoft.com)

## 0.59.0-vnext.133
Fri, 09 Aug 2019 22:37:48 GMT

### Changes

- use more specific typing for currentTheme, improve examples (andrewh@microsoft.com)

## 0.59.0-vnext.132
Fri, 09 Aug 2019 19:20:03 GMT

### Changes

- Update UwpPreparedScriptStore to use shared_ptrs (jthysell@microsoft.com)

## 0.59.0-vnext.131
Fri, 09 Aug 2019 06:07:04 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.40 (53619745+rnbot@users.noreply.github.com)

## 0.59.0-vnext.130
Fri, 09 Aug 2019 04:18:45 GMT

### Changes

- Added list, listitem roles to ViewAccessibility.uwp.js (jthysell@microsoft.com)

## 0.59.0-vnext.129
Fri, 09 Aug 2019 03:27:03 GMT

### Changes

- Fix Sample/index.tsx layout, set Tag as Name in debug (andrewh@microsoft.com)

## 0.59.0-vnext.128
Thu, 08 Aug 2019 23:35:13 GMT

### Changes

- fix hit-testing bug (kmelmon@microsoft.com)

## 0.59.0-vnext.127
Thu, 08 Aug 2019 22:19:50 GMT

### Changes

- Add textDecoration support to Nested Text (dida@ntdev.microsoft.com)

## 0.59.0-vnext.126
Thu, 08 Aug 2019 20:57:44 GMT

### Changes

- Moving picker fix (decrowle@microsoft.com)

## 0.59.0-vnext.125
Wed, 07 Aug 2019 17:07:35 GMT

### Changes

- Adding accessibilityPosInSet and accessibilitySizeOfSet props to ViewWindows, #2894 (jthysell@microsoft.com)

## 0.59.0-vnext.124
Wed, 07 Aug 2019 16:00:25 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.38 (53619745+rnbot@users.noreply.github.com)

## 0.59.0-vnext.123
Tue, 06 Aug 2019 21:09:10 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.37 (53619745+rnbot@users.noreply.github.com)

## 0.59.0-vnext.122
Tue, 06 Aug 2019 18:30:23 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.36 (53619745+rnbot@users.noreply.github.com)

## 0.59.0-vnext.121
Tue, 06 Aug 2019 15:46:10 GMT

### Changes

- Modify how we setup build directories to better support monorepos (acoates@microsoft.com)

## 0.59.0-vnext.120
Tue, 06 Aug 2019 00:59:54 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.35 (53619745+rnbot@users.noreply.github.com)

## 0.59.0-vnext.119
Mon, 05 Aug 2019 17:37:47 GMT

### Changes

- Put back a couple of win32 dll exports (acoates@microsoft.com)
- Updating react-native to version: 0.59.0-microsoft.34 (53619745+rnbot@users.noreply.github.com)

## 0.59.0-vnext.118
Mon, 05 Aug 2019 06:09:37 GMT

### Changes

- Include additional jsi headers in nuget (acoates@microsoft.com)

## 0.59.0-vnext.117
Mon, 05 Aug 2019 00:09:47 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.33 (53619745+rnbot@users.noreply.github.com)

## 0.59.0-vnext.116
Sat, 03 Aug 2019 17:26:50 GMT

### Changes

- change dev menu shortcut to ctrl+shift+D (kmelmon@microsoft.com)

## 0.59.0-vnext.115
Sat, 03 Aug 2019 04:30:38 GMT

### Changes

- change process.env.TEMP to os.tmpdir() (email not defined)
- fix onFocus/onBlur events for Touchables (andrewh@microsoft.com)
- New switch props (thumbColor and trackColor), and fix few bugs found in TextInputViewManager (dida@ntdev.microsoft.com)
- addedundefined developer menu (kmelmon@microsoft.com)
- fix image not firing on load. (stecrain@microsoft.com)
- Delay-load dependencies absent on Win7 (acoates@microsoft.com)
- Add the SpringAnimationDriver (stpete@microsoft.com)

## 0.59.0-vnext.110
Sun, 28 Jul 2019 12:44:00 GMT

### Changes

- Allow run-windows to be more flexible about project layout to support default c++ layouts (acoates@microsoft.com)

## 0.59.0-vnext.109
Fri, 26 Jul 2019 08:32:04 GMT

### Changes

- add null checks to fix crashes with reload / shutdown with pending text updates (andrewh@microsoft.com)

## 0.59.0-vnext.108
Fri, 26 Jul 2019 07:23:20 GMT

### Changes

- Fix for flyouts unexpectedly dismissing when opening picker (ruaraki@microsoft.com)

## 0.59.0-vnext.107
Fri, 26 Jul 2019 05:43:01 GMT

### Changes

- use win32 file api to avoid runtime broker slowing us down (stecrain@microsoft.com)

## 0.59.0-vnext.106
Thu, 25 Jul 2019 00:29:50 GMT

### Changes

- Fixes to ReactImage lifetime with downloading Images (acoates@microsoft.com)

## 0.59.0-vnext.105
Wed, 24 Jul 2019 21:03:23 GMT

### Changes

- Hookup beachball for publishing (acoates@microsoft.com)
