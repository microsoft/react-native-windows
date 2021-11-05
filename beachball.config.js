/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const {execSync} = require('child_process');

module.exports = {
  ...require(`${__dirname}/packages/@rnw-scripts/generated-beachball-config/beachball.config.g.json`),
  
  // Do not generate tags for monorepo packages by default, to avoid a GitHub
  // release for every package.
  gitTags: false,

  hooks: {
    // Stamp versions when we publish a new package
    postbump: (_packagePath, name, version) => {
      if (name === 'react-native-windows') {
        console.log(`Stamping RNW Version ${version}`);
        execSync(`yarn stamp-version ${version}`);
      }
    }
  }
}
