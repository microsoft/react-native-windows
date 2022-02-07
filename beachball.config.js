/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const {execSync} = require('child_process');
const {findRepoPackageSync} = require('@react-native-windows/package-utils');

module.exports = {
  ...require("@rnw-scripts/generated-beachball-config"),
  
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
  const packageJson = findRepoPackageSync(packageName)?.json;
  return packageJson && packageJson.version.includes('-');
}

function formatComment(comment) {
  // Remove versions from messages that look like "[0.xx] Message"
  return comment.match(/(\s*\[[\d\.]+\]\s*)?((.|\n)*)/)[2];
}