/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const {execSync} = require('child_process');
const {findRepoPackageSync} = require('@react-native-windows/package-utils');

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
  },

  transform: {
    changeFiles: (changeInfo) => ({
      ...changeInfo,
      type: correctChangeType(changeInfo),
      comment: formatComment(changeInfo.comment),
    }),
  }
}

function correctChangeType(changeInfo) {
  // Changes made to our main branch are often rolled into prerelease packages,
  // where a released branch should treat these changes as creating a new patch
  // release.
  if (changeInfo.type === 'prerelease' && !isPrerelease(changeInfo.packageName)) {
    return 'patch';
  }

  return changeInfo.type;
}

function isPrerelease(packageName) {
  const {version} = findRepoPackageSync(packageName).json;
  return version.includes('-');
}

function formatComment(comment) {
  // Remove versions from messages that look like "[0.xx] Message"
  return comment.match(/(\s*\[[\d\.]+\]\s*)?((.|\n)*)/)[2];
}