/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

require('ts-node').register({ transpileOnly: true });

const fs = require('fs');

exports.config = {
  // WebdriverIO allows it to run your tests in arbitrary locations (e.g. locally or
  // on a remote machine).
  runner: 'local',

  // Define which test specs should run. The pattern is relative to the directory
  // from which `wdio` was called.
  specs: ['wdio/test/**/*.test.ts'],

  // The property handles how many capabilities from the same test should run tests.
  maxInstances: 1,

  capabilities: [
    {
      // maxInstances can get overwritten per capability.
      maxInstances: 1,

      app: 'ReactUWPTestApp_cezq6h4ygq1hw!App',
      'ms:experimental-webdriver': true,
    },
  ],

  // Level of logging verbosity: trace | debug | info | warn | error
  logLevel: 'error',

  // Warns when a deprecated command is used
  deprecationWarnings: true,

  // If you only want to run your tests until a specific amount of tests have failed use
  // bail (default is 0 - don't bail, run all tests).
  bail: 0,

  // Default timeout for all waitFor* commands.
  waitforTimeout: 60000,

  // Default timeout in milliseconds for request
  // if Selenium Grid doesn't send response
  connectionRetryTimeout: 90000,

  // Default request retries count
  connectionRetryCount: 10,

  port: 4723,

  // Test runner services
  services: [
    [
      'winappdriver',
      {
        logPath: './reports/',
      },
    ],
  ],

  // Framework you want to run your specs with.
  // The following are supported: Mocha, Jasmine, and Cucumber
  framework: 'jasmine',

  // Test reporter for stdout.
  // see also: https://webdriver.io/docs/dot-reporter.html
  reporters: ['spec', ['junit', { outputDir: '.\\reports' }]],

  jasmineNodeOpts: {
    // Needs to be longer than the 1m webdriverio timeout to let its errors be propgated
    defaultTimeoutInterval: 100000,
    expectationResultHandler: function(passed, _assertion) {
      if (!passed) {
        const name = 'ERROR-' + Date.now();
        browser.saveScreenshot('./errorShots/' + name + '.png');
      }
    },
  },

  /**
   * Gets executed once before all workers get launched.
   */
  onPrepare: function(_config, _capabilities) {
    fs.mkdirSync('./errorShots', { recursive: true });
  },
};
