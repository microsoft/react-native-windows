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
  },
};
```

WebDriverIO options for initializing a remote may be passed through `testEnvironmentOptions.webdriverOptions`.
