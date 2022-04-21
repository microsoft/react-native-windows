/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @ts check
 * @format
 */

const cli = require('@react-native-windows/cli');

module.exports = {
  // **** This section defined commands and options on how to provide the Windows platform to external applications
  commands: cli.commands.map(command => ({
    ...command,
    func: lazyInstallSourcemaps(command.func),
  })),
  platforms: {
    windows: {
      linkConfig: () => null,
      projectConfig: lazyInstallSourcemaps(cli.projectConfig),
      dependencyConfig: lazyInstallSourcemaps(cli.dependencyConfig),
      npmPackageName: 'react-native-windows',
    },
  },
  get healthChecks() {
    return cli.getHealthChecks();
  },
};

// We ship compiled JavaScript to end-users running the CLI. We use
// "source-map-support" to install a sourcemap-aware exception handler, so that
// stack traces shown to end-users (and included in our issues), resolve
// correctly to the TS sources. Resolution is only done when an exception occurs,
// but there is some initial overhead to installing the exception handler.
//
// We want to do this before our code is hooked, to ensure the handler is
// installed, but doing this as part of global initialization adds a cost to
// anyone who depends on `react-native-windows`, and parses their
// `react-native.config`, regardless of whether they call the functions. We
// delay installing the support from require-time, to function invocation time,
// which should catch most cases without the penalty.
let sourcemapSupportInstalled = false;
function lazyInstallSourcemaps(fn) {
  return (...args) => {
    if (!sourcemapSupportInstalled) {
      require('source-map-support').install();
      sourcemapSupportInstalled = true;
    }

    return fn(...args);
  };
}
