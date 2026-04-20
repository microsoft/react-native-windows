# Port RCTTestModule to React Native Windows TurboModule

## Method Mapping

| ObjC method | RNW method | Notes |
|---|---|---|
| `markTestCompleted` | `MarkTestCompleted()` | Delegates to `MarkTestPassed(true)` |
| `markTestPassed:` | `MarkTestPassed(bool)` | Sets atomic `TestStatus` enum |
| `verifySnapshot:` | `VerifySnapshot(callback)` | Always returns `true` (no snapshot support on Windows) |
| `sendAppEvent:body:` | `SendAppEvent(name, body)` | No-op on Windows |
| `shouldResolve` | `ShouldResolve(promise)` | Resolves with `1` |
| `shouldReject` | `ShouldReject(promise)` | Rejects with empty error |

## Key Design Choices

- Uses `REACT_MODULE` macro with `REACT_METHOD` decorators (JS names preserved as lowercase)
- `TestStatus` enum replaces ObjC's `RCTTestStatus`, exposed via `Status()` accessor for the test runner to poll
- `std::atomic<TestStatus>` for thread safety since status is set from JS thread and read from test thread
