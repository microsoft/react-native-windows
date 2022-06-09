# debug-test project

This folder contains tests to verify that the JavaScript (JS) code in ReactNative for Windows (RNW) packages can be debugged. The test
code is intended for RNW contributors; it is not meant to be published. Currently, test execution needs to be triggered manually. In
the future, CI steps should get authored to ensure that these tests get performed automatically.

## Manually Triggering Test Execution

1. Ensure the UWP Playground app has been built and its app package has been registered (i.e. the Windows Start menu contains an entry
for it). This can be achieved by building and launching the UWP Playground app from Visual Studio.
2. In a RNW dev command prompt, change into the debug-test folder.
3. With neither Metro nor the UWP Playground app running, invoke `yarn debugtest`.

This should start the UWP Playground app and Metro, and perform jest tests automating both. The jest test code contains a CDP debugger
frontend implementation through which it performs test validation steps. Test success or failure is indicated by the jest command-line
output. As the test automates the Playground app via UI automation, avoid keyboard and mouse interactions during
test execution.

## Test Design Considerations

- The RNW project is not responsible for the debugger backend implementations of JS engines. The debugging scenarios implemented in this
project do not need to be exhaustive with respect to debugging features. They should rather focus on RNW features or workflows that can
impact debuggability and debugging modes (e.g. engine- or debug-related instance options, reload/refresh operations).

- Ultimately, the test matrix should include multiple JS engines (e.g. Hermes, V8), multiple package-provisioning schemes (e.g. Metro
bundler, file-based bundles), multiple debug modes (e.g. direct debugging, web debugging) and package operations such as reloads or
refreshes. The test setup/framework should be flexible enough to accommodate these variations.

- It is assumed that a functional (debuggable) React instance requires functional native modules (in particular, a UI manager).
For that reason, the tests operate a against a "real" React host app rather than a "headless" React instance using mock native modules.

- To enable certain debugging scenarios such as top-level `debugger` statements or throwing top-level JS code, a multi-package host app
is required instead of a multi-page host app. This is the reason why the tests currently operate against the Playground app rather than
the integration test app.

## Future Improvements

- Add CI steps to automatically execute these tests.

- Enable the integration test app to load different bundles at runtime; then change tests to target integration test app instead of
Playground app.

- Investigate whether the jest test code can directly invoke the Metro JS code rather than spawning a separate Metro process.

- Automate the host app through a non-UIA channel (perhaps the same automation channel used for verification of the visual tree).
