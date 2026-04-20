# Port RCTTestModule to React Native Windows TurboModule

## Summary

Ported the iOS `RCTTestModule` to a C++ RNW TurboModule and wired it into a headless desktop integration test (`Proto2`). The test loads a JS bundle from Metro, which calls `TestModule.markTestPassed(true)` via the bridgeless TurboModule proxy, and the native test awaits the signal.

## Method Mapping

| ObjC method | RNW method | Notes |
|---|---|---|
| `markTestCompleted` | `MarkTestCompleted()` | Delegates to `MarkTestPassed(true)` |
| `markTestPassed:` | `MarkTestPassed(bool)` | Sets atomic `TestStatus` enum |
| `verifySnapshot:` | `VerifySnapshot(callback)` | Always returns `true` (no snapshot support on Windows) |
| `sendAppEvent:body:` | `SendAppEvent(name, body)` | Emits via `RCTDeviceEventEmitter` |
| `shouldResolve` | `ShouldResolve(promise)` | Resolves with `1` |
| `shouldReject` | `ShouldReject(promise)` | Rejects with empty error |

## Key Design Choices

- Uses `REACT_MODULE` macro with `REACT_METHOD` decorators (JS names preserved as lowercase)
- `TestStatus` enum replaces ObjC's `RCTTestStatus`, exposed via `Status()` accessor for the test runner to poll
- `std::atomic<TestStatus>` for thread safety since status is set from JS thread and read from test thread
- Static `AwaitCompletion()` / `Reset()` for synchronization between JS and native test thread
- Mock `DeviceInfo` module prevents crashes in headless (no-XAML) environment
- JS bypasses `require('react-native')` initialization (which needs `AppState` etc.) and accesses `global.nativeModuleProxy` directly
- Fabric enabled via stub `MockCompositionContext` (no real compositor needed)
- DLL manifest manually activated via `CreateActCtxW`/`ActivateActCtx` (required because test runs inside `testhost.exe`)

## Files Changed

| File | Change |
|---|---|
| `vnext/Desktop.IntegrationTests/Modules/TestModule.h` | New: ported TurboModule |
| `vnext/Desktop.IntegrationTests/MockCompositionContext.h` | New: mock Composition interfaces for headless Fabric |
| `vnext/Desktop.IntegrationTests/Prototype.cpp` | Major rewrite: headless test harness + Proto2 test |
| `vnext/Desktop.IntegrationTests/Application.manifest` | Added WinRT activatable class entries |
| `vnext/Desktop.IntegrationTests/React.Windows.Desktop.IntegrationTests.vcxproj` | Uncommented manifest include |
| `vnext/src-win/IntegrationTests/DummyTest.js` | Uses bridgeless TurboModule proxy |
