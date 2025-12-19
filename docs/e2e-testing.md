# End-to-End Testing

## e2e-test-app project structure

A test app, test library and test cases are in [`/packages/e2e-test-app-fabric/`](../packages/e2e-test-app-fabric), and are organized as below.

 - `test` – includes Jest tests using webdriverio
 - `windows` – the UWP native app
 - `jest.config.js` – configuration for Jest and WebDriverIO

## Running Tests

**Install [WinAppDriver v1.2.1](https://github.com/microsoft/WinAppDriver/releases/download/v1.2.1/WindowsApplicationDriver_1.2.1.msi)**

This will be automatically done for you if you use the [RNW dependency script](https://microsoft.github.io/react-native-windows/docs/rnw-dependencies) with `rnwDev` as an argument.

The E2E tests assume it is installed in the default location under `C:\Program Files (x86)\Windows Application Driver\WinAppDriver.exe`. If you choose a different path, you will need to specify it in [`/packages/e2e-test-app-fabric/jest.config.js`](../packages/e2e-test-app-fabric/jest.config.js).
```js
module.exports = {
...
  testEnvironmentOptions: {
    ...
    winAppDriverBin: 'D:\\Program Files (x86)\\Windows Application Driver\\WinAppDriver.exe',
  },
};
```

**Build the native test app**

> C:\repo\react-native-windows> `cd packages\e2e-test-app-fabric`
>
> C:\repo\react-native-windows\packages\e2e-test-app-fabric> `yarn windows --no-launch`

**Running all tests**

> C:\repo\react-native-windows\packages\e2e-test-app-fabric> `yarn start`
> 
> C:\repo\react-native-windows\packages\e2e-test-app-fabric> `yarn e2etest`


**Running a specific test**

> C:\repo\react-native-windows\packages\e2e-test-app-fabric> `yarn start`
> 
⚠ Only the test filename (without the rest of the path) should be included.
> C:\repo\react-native-windows\packages\e2e-test-app-fabric> `yarn e2etest visitAllPages.test.ts`

**Break on app start**

> C:\repo\react-native-windows\packages\e2e-test-app-fabric> `yarn start`
> 
> C:\repo\react-native-windows\packages\e2e-test-app-fabric> `yarn e2etest:debug visitAllPages.test.ts`

## Debugging E2E Tests in CI
### Increasing verbosity
By default the only messages printed during tests are related to errors returned by WinAppDriver or assertion failures. It is possible to increase verbosity to show individual WebDriver wire commands by editing [`/packages/e2e-test-app-fabric/jest.config.js`](../packages/e2e-test-app-fabric/jest.config.js).

```js
module.exports = {
...
  testEnvironmentOptions: {
    ...
    webdriverOptions: {
      // Level of logging verbosity: trace | debug | info | warn | error
      logLevel: 'error',
      ...
    },
  },
};
```

### Test Artifacts

If you have access to the AzureDevOps pipeline you'll be able to see test failures and debug crashes.
Here are the artifacts that are produced during the build:
- error screenshots of the app when a test failed
- test run XML - this contains some information like the name of the wdio test that failed and the JS stack
- tree dump outputs - you can compare these to the output of the main branch to see if there is a the difference responsible for the test failing. 
- crash dumps of the e2e test app (RNTesterApp)

You can access these by going to the AzureDevOps run for your PR and clicking on the artifacts link:

![Artifacts](img/e2e-artifacts.png)

## Architecture

### WinAppDriver

WinAppDriver uses [UIA](https://docs.microsoft.com/en-us/windows/uwp/design/accessibility/accessibility-testing) to
inspect and manipulate a test application, allowing clients to connect to it using the w3c WebDriver protocol.

### WebDriverIO

WebDriverIO is a JavaScript library which connects to WinAppDriver using the WebDriver protocol. It provides [global
APIs](https://webdriver.io/docs/api) for querying the application UI tree such as selectors (see below) and a global `browser` object to manipulate the
test application.

### Jest

Jest is the test runner used for end-to-end testing, including assertsion libraries, test selection, etc. WebDriverIO setup is
provided by a custom environment [`@react-native-windows/automation`](../packages/@react-native-windows/automation).

## Authoring Tests

Tests are written using Jest and WebDriverIO (see more below) against a test app running `RNTester`. `RNTester` example
pages are used as Test UI, which is examined via Jest tests. Tests should attempt to target an existing `RNTester` page, but "Legacy" tests exist as custom pages only used by e2e-test-app-fabric

### Writing a test against an RNTester example page

```ts
// LegacyControlStyleTestPage.test.ts

describe('FancyWidget', () => {

  beforeAll(async () => {
    await goToComponentExample('FancyWidget');
  });

  test('FancyWidget is populated with placeholder', async () => {
    // Query for an element with accessibilityId of "foo" (see "locators" below)
    const field = await app.findElementByTestID('foo');
    expect(await field.getText()).toBe('placeholder');
  });

});
```

### Adding a custom RNTester page

Before adding a custom page, consider whether the change can be made to an existing RNTester page and upstreamed. If needed, new examples may be integrated into the Windows fork of RNTester, [`@react-native-windows/tester`](../packages/@react-native-windows/tester).

Hooks are recommended to author the test page. (see [https://reactjs.org/docs/hooks-intro.html](https://reactjs.org/docs/hooks-intro.html) and this [Pluralsight course](https://www.pluralsight.com/courses/using-react-hooks) to learn more about Hooks)

```js
// ControlStyleTestPage.tsx
export const title = 'LegacyControlStyleTest';
export const description = 'Legacy e2e test for Control Styles';
export const examples = [
  {
    render: function(): JSX.Element {
      return <ControlStyleTestPage />;
    },
  },
];
```

```js
  // RNTesterList.windows.js
  ...
  {
    key: 'LegacyControlStyleTest',
    module: require('../examples-win/LegacyTests/ControlStyleTestPage'),
  },
  ...
```

## Snapshot tests

E2ETests uses [snapshot tests](https://jestjs.io/docs/en/snapshot-testing) to detect unintended visual changes. Instead
of testing against the react tree, e2e-test-app-fabric compares the fully rendered composition tree. This allows testing the
correctness of ViewComponents.

```ts
import {dumpVisualTree} from '@react-native-windows/automation-commands';

test('Example test', async () => {
  const dump = await dumpVisualTree('test-id-here');
  expect(dump).toMatchSnapshot();
});
```

Snapshots can be updated by either:
1. Locally running `jest --updateSnapshot`.
2. Copying from the "shapshots" artifact uploaded from a failing PR


 
