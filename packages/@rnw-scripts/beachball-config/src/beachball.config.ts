/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * 
 * @format
 */

import {execSync} from 'child_process';
import {findRepoPackageSync} from '@react-native-windows/package-utils';

import type {BeachballOptions} from 'beachball/lib/types/BeachballOptions';
import type {ChangeInfo} from 'beachball/lib/types/ChangeInfo';
 
const Options: BeachballOptions = {
  ...require("../generated-beachball-config"),
   
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
    changeFiles: (changeInfo) => Array.isArray(changeInfo.changes)
    ? {...changeInfo, changes: changeInfo.changes.map(transformChangeInfo)}
    : transformChangeInfo(changeInfo as ChangeInfo),
  }
}

function transformChangeInfo(changeInfo: ChangeInfo) : ChangeInfo {
  return {
    ...changeInfo,
    type: correctChangeType(changeInfo),
    comment: formatComment(changeInfo.comment),
  };
}

 
 function correctChangeType(changeInfo: ChangeInfo) {
   // Changes made to our main branch are often rolled into prerelease packages,
   // where a released branch should treat these changes as creating a new patch
   // release.
   if (changeInfo.type === 'prerelease' && !isPrerelease(changeInfo.packageName)) {
     return 'patch';
   }
 
   return changeInfo.type;
 }
 
 function isPrerelease(packageName: string): boolean {
   const packageJson = findRepoPackageSync(packageName)?.json;
   return packageJson && packageJson.version.includes('-');
 }
 
 function formatComment(comment: string): string {
   // Remove versions from messages that look like "[0.xx] Message"
   return comment.match(/(\s*\[[\d\.]+\]\s*)?((.|\n)*)/)?.[2] ?? comment;
 }

module.exports = Options;
