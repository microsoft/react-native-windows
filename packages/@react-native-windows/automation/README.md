# @react-native-windows/automation

`@react-native-windows/automation` makes it easy to perform UI tests against
your `react-native-windows` application using Jest. 

**This package is a work in progress**

## System Requirements

`@react-native-windows/automation` relies on [WinAppDriver 1.2.1](https://github.com/microsoft/WinAppDriver/releases/tag/v1.2.1)
or later to manipulate and inspect your application via [Windows UI Automation](https://docs.microsoft.com/en-us/dotnet/framework/ui-automation/ui-automation-overview).
This is already installed in common CI environments like GitHub Actions or
Azure Pipelines hosted images.

## Configuring Jest

First, ensure that `@react-native-windows/automation` is included as a
dependency in your package:

```json
{
  "devDependencies": {
    "@react-native-windows/automation": "<version>",
  }
}
```

Next, edit your [Jest configuration](https://jestjs.io/docs/configuration) to
use `@react-native-windows/automation` as your test environment. Add the
following to your `jest.config.js` or `jest.config.ts` file:

```js
module.exports = {
...
  // Set up the automation environment before running tests
  testEnvironment: '@react-native-windows/automation',

  // Only run a single test at a time
  maxWorkers: 1,

  // Set up @react-native-windows/automation specific options (see below)
  testEnvironmentOptions: {
    app: '<AppName>',
  },
};
```

## Environment Options

`@react-native-windows/automation` is configured via the
`testEnvironmentOptions` property in your Jest configuration. The following
options are valid:

```js
testEnvironmentOptions: {
  // Required: Your application to launch, as either an AppX package identity,
  // or path to an .exe for an unpackaged application.
  app: 'Microsoft.WindowsAlarms',

  // Optional: Explicit path to WinAppDriver. By default,
  // `@react-native-windows/automation` tries to use
  // "%PROGRAMFILES(X86)%\Windows Application Driver\WinAppDriver.exe"
  winAppDriverBin: 'D:\\WinAppDriver.exe',

  // Optional: Options to be passed to WebDriverIO
  // See https://webdriver.io/docs/options/
  webdriverOptions: {
      // Port to use for WebDriver server (Default 4723)
      port: 4444

      // Level of logging verbosity: trace | debug | info | warn | error
      logLevel: 'error',

      // Default timeout for all waitFor* commands.
      waitforTimeout: 60000,

      // Default timeout in milliseconds for request
      connectionRetryTimeout: 90000,

      // Default request retries count
      connectionRetryCount: 10,
    },
},
```

## Writing Tests

`@react-native-windows/automation` exports an `app` member with functions to
perform globally scoped WebDriver commands, such as locating an element or
waiting for a condition. Several strategies are present to locate an element,
with the reccomended being to use React Native's `testID` property.

A located `AutomationElement` exposes methods to allow application interaction
and introspection, such as clicking or typing.

```js
import {app} from '@react-native-windows/automation';

beforeAll(async () => {
  const appHeader = await app.findElementByTestID('app-header');

  await app.waitUntil(async () => {
    const headerText = await appHeader.getText();
    return headerText.includes('Welcome');
  }) 
});

test('Type abc', async () => {
  const textInput = await app.findElementByTestID('textinput-field');
  await textInput.setValue('abc');
  expect(await textInput.getText()).toBe('abc');
});
```

## Performing Additional Actions

**WIP Not yet exposed**

Sometimes it is useful to exmaine or manipulate the application in ways that
are not exposed to Windows UI Automation. An additional package,
`@react-native-windows/automation-channel` can be included in your application
as a native module to allow writing custom commands performed by your
application in-process. Some pre-built commands can be included using the
`@react-native-windows/automation-commands` package.

## Visual Comparison

**WIP Not yet exposed**

A common use-case of UI testing is to ensure your components look the way you
expect. Tools like [enzyme](https://github.com/enzymejs/enzyme) allow
inspecting your React component tree, but are oblivious to the native UI tree.

`@react-native-windows/automation-commands` exposes a `dumpVisualTree` command
which creates a JSON object corresponding to the XAML tree under a given
testID. This can be used in conjunction with snapshot testing to validate
native component logic stays consistent.

```js
import {dumpVisualTree} from '@react-native-windows/automation-commands';

test('Widget', async () => {
  const dump = await dumpVisualTree('widget-test-id');
  expect(dump).toMatchSnapshot();
});
```