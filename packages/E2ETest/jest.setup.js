/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

const fs = require('fs');
const path = require('path');

const screenshotDir = './errorShots';
fs.mkdirSync(screenshotDir, {recursive: true});

// Register to screenshot on each test failure
global.jasmine.getEnv().addReporter({
  specDone: async result => {
    if (result.status === 'failed') {
      const filename = path.join(
        screenshotDir,
        `${result.fullName.replace(/\s/g, '-')}.png`,
      );

      await browser.saveScreenshot(filename);
    }
  },
});
