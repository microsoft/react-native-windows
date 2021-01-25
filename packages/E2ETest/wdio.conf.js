require('ts-node').register({ transpileOnly: true });

const baseUrl = 'https://webdriver.io';

exports.config = {
  //
  // ====================
  // Runner Configuration
  // ====================
  //
  // WebdriverIO allows it to run your tests in arbitrary locations (e.g. locally or
  // on a remote machine).
  runner: 'local',

  //
  // ==================
  // Specify Test Files
  // ==================
  // Define which test specs should run. The pattern is relative to the directory
  // from which `wdio` was called. Notice that, if you are calling `wdio` from an
  // NPM script (see https://docs.npmjs.com/cli/run-script) then the current working
  // directory is where your package.json resides, so `wdio` will be called from there.
  //
  specs: ['wdio/test/**/*.test.ts'],
  // Patterns to exclude.
  exclude: [
    // 'path/to/excluded/files'
  ],

  // ============
  // Capabilities
  // ============
  // Define your capabilities here. WebdriverIO can run multiple capabilities at the same
  // time. Depending on the number of capabilities, WebdriverIO launches several test
  // sessions. Within your capabilities you can overwrite the spec and exclude options in
  // order to group specific specs to a specific capability.

  // First, you can define how many instances should be started at the same time. Let's
  // say you have 3 different capabilities (Chrome, Firefox, and Safari) and you have
  // set maxInstances to 1; wdio will spawn 3 processes. Therefore, if you have 10 spec
  // files and you set maxInstances to 10, all spec files will get tested at the same time
  // and 30 processes will get spawned. The property handles how many capabilities
  // from the same test should run tests.
  //
  maxInstances: 1,

  // If you have trouble getting all important capabilities together, check out the
  // Sauce Labs platform configurator - a great tool to configure your capabilities:
  // https://docs.saucelabs.com/reference/platforms-configurator
  //
  capabilities: [
    {
      // maxInstances can get overwritten per capability. So if you have an in-house Selenium
      // grid with only 5 firefox instances available you can make sure that not more than
      // 5 instances get started at a time.
      maxInstances: 1,

      app: 'ReactUWPTestApp_cezq6h4ygq1hw!App',
      'ms:experimental-webdriver': true,
    },
  ],

  // ===================
  // Test Configurations
  // ===================
  // Define all options that are relevant for the WebdriverIO instance here

  // Level of logging verbosity: trace | debug | info | warn | error
  logLevel: 'error',

  // Warns when a deprecated command is used
  deprecationWarnings: true,

  // If you only want to run your tests until a specific amount of tests have failed use
  // bail (default is 0 - don't bail, run all tests).
  bail: 0,

  // Set a base URL in order to shorten url command calls. If your `url` parameter starts
  // with `/`, the base url gets prepended, not including the path portion of your baseUrl.
  // If your `url` parameter starts without a scheme or `/` (like `some/path`), the base url
  // gets prepended directly.
  baseUrl: baseUrl,

  // Default timeout for all waitFor* commands.
  waitforTimeout: 60000,

  // Default timeout in milliseconds for request
  // if Selenium Grid doesn't send response
  connectionRetryTimeout: 90000,

  // Default request retries count
  connectionRetryCount: 1,

  port: 4723,

  // Test runner services
  // Services take over a specific job you don't want to take care of. They enhance
  // your test setup with almost no effort. Unlike plugins, they don't add new
  // commands. Instead, they hook themselves up into the test process.
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
  // see also: https://webdriver.io/docs/frameworks.html
  //
  // Make sure you have the wdio adapter package for the specific framework installed
  // before running any tests.
  framework: 'jasmine',

  // Test reporter for stdout.
  // The only one supported by default is 'dot'
  // see also: https://webdriver.io/docs/dot-reporter.html
  reporters: ['spec', ['junit', { outputDir: '.\\reports' }]],

  jasmineNodeOpts: {
    // Needs to be longer than the 1m webdriverio timeout to let its errors be propgated
    defaultTimeoutInterval: 100000,
    expectationResultHandler: function(passed, assertion) {
      /**
       * only take screenshot if assertion failed
       */
      if (passed) {
        return;
      }

      let name = 'ERROR-' + Date.now();
      browser.saveScreenshot('./errorShots/' + name + '.png');
    },
  },

  /**
   * Gets executed before test execution begins. At this point you can access to all global
   * variables like `browser`. It is the perfect place to define custom commands.
   * @param {Array.<Object>} capabilities list of capabilities details
   * @param {Array.<String>} specs List of spec file paths that are to be run
   */
  before: function(_capabilities, _specs) {
    // require('ts-node/register');
    require('ts-node').register({ files: true });
  },
};
