/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const {execSync} = require('child_process');

module.exports = {
  ...require('@rnw-scripts/generated-beachball-config'),
  
  // Do not generate tags for monorepo packages by default, to avoid a GitHub
  // release for every package.
  gitTags: false,

  // Commit message used when publishing
  message: 'Publish Packages | ***NO_CI***',

  hooks: {
    // Stamp versions when we publish a new package
    prepublish: (_packagePath, name, version) => {
      if (name === 'react-native-windows') {
        console.log(`Stamping RNW Version ${version}`);

        // prepublish is run before bumping or commiting. Append an additional
        // commit to check-in stamped versions when we push.
        
        execSilent(`yarn stamp-version ${version}`);
        execSilent('git add --all');
        execSilent(`git commit --message "Stamp RNW ${version}"`);
      }
    }
  }
}

function execSilent(command) {
  execSync(command, {stdio: 'pipe'});
}