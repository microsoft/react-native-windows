/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {execSync} from 'child_process';
import {quote} from 'shell-quote';
import {findRepoPackageSync} from '@react-native-windows/package-utils';

import type {RepoOptions} from 'beachball/lib/types/BeachballOptions';
import type {ChangeInfo} from 'beachball/lib/types/ChangeInfo';

const Options: RepoOptions = {
  ...require('@rnw-scripts/generated-beachball-config'),

  // Do not generate tags for monorepo packages by default. May be overridden in
  // a package's "package.json" by adding:
  // ```
  // "beachball": {
  //   "gitTags": true
  // }
  // ```
  gitTags: false,

  hooks: {
    // Stamp versions when we publish a new package
    postbump: (_packagePath, name, version) => {
      if (name === 'react-native-windows') {
        console.log(`Stamping RNW Version ${version}`);
        execSync(`yarn stamp-version ` + quote([`${version}`]));
      }
    },
  },

  transform: {
    changeFiles: (changeInfo, changeFilePath) =>
      Array.isArray(changeInfo.changes)
        ? {
            ...changeInfo,
            changes: changeInfo.changes.map(info =>
              transformChangeInfo(info, changeFilePath),
            ),
          }
        : transformChangeInfo(changeInfo as ChangeInfo, changeFilePath),
  },
};

function transformChangeInfo(
  changeInfo: ChangeInfo,
  changeFilePath: string,
): ChangeInfo {
  return {
    ...changeInfo,
    type: correctChangeType(changeInfo),
    comment: correctComment(changeInfo, changeFilePath),
  };
}

function correctChangeType(changeInfo: ChangeInfo) {
  // Changes made to our main branch are often rolled into prerelease packages,
  // where a released branch should treat these changes as creating a new patch
  // release.
  if (
    changeInfo.type === 'prerelease' &&
    !isPrerelease(changeInfo.packageName)
  ) {
    return 'patch';
  }

  return changeInfo.type;
}

function isPrerelease(packageName: string): boolean {
  const packageJson = findRepoPackageSync(packageName)?.json;
  return packageJson && packageJson.version.includes('-');
}

function correctComment(
  changeInfo: ChangeInfo,
  changeFilePath: string,
): string {
  let comment = changeInfo.comment;

  // Validate comment is a string
  if (typeof comment !== 'string') {
    // beachball catches and swallows exceptions, so must force exit here
    console.error(
      `ERROR: There is an invalid comment detected: '${changeFilePath}': comment is not a string`,
    );
    process.exit(1);
  }

  // Remove versions from messages that look like "[0.xx] Message"
  comment = comment.match(/(\s*\[[\d\.]+\]\s*)?((.|\n)*)/)?.[2] ?? comment;

  comment = comment.trim();

  // Validate (remaining) comment is not empty
  if (comment === '') {
    // beachball catches and swallows exceptions, so must force exit here
    console.error(
      `ERROR: There is an invalid comment detected: '${changeFilePath}': comment is blank or whitespace`,
    );
    process.exit(1);
  }

  return comment;
}

module.exports = Options;
