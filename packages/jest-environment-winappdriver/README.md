# jest-environment-winappdriver

This package allows Jest to run WebDriverIO tests through WinAppDriver.

## Usage

Environment options can be configured through Jest configuration, like the example below:

```js
module.exports = {
...
  testEnvironment: 'jest-environment-winappdriver',
  maxWorkers: 1,
  testEnvironmentOptions: {
    app: 'ReactUWPTestApp_cezq6h4ygq1hw!App',
    webdriverOptions: {
      logLevel: 'error',
      ...
    },
    winAppDriverBin: 'D:\\Program Files (x86)\\Windows Application Driver\\WinAppDriver.exe',
  },
};
```

WebDriverIO options for initializing a remote may be passed through `testEnvironmentOptions.webdriverOptions`.

Custom installation paths of WinAppDriver can be specified through `testEnvironmentOptions.winAppDriverBin`.
