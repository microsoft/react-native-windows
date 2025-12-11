/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import fs from 'fs';
import path from 'path';
import findUp from 'find-up';
import {execSync} from 'child_process';
import {quote} from 'shell-quote';
import {findRepoPackageSync} from '@react-native-windows/package-utils';

import type {RepoOptions} from 'beachball/lib/types/BeachballOptions';
import type {ChangeInfo} from 'beachball/lib/types/ChangeInfo';

const rootPackageName = 'react-native-windows-repo';
const platformOverridePackage = 'react-native-platform-override';

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
    postbump: (packagePath, name, version) => {
      if (name === 'react-native-windows') {
        console.log(`Stamping RNW Version ${version}`);
        execSync(`yarn stamp-version ` + quote([`${version}`]));
      }

      if (name === platformOverridePackage) {
        syncRootDependencyVersion(
          packagePath,
          platformOverridePackage,
          version,
        );
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

type RootPackageJson = {
  name?: string;
  dependencies?: Record<string, string>;
  devDependencies?: Record<string, string>;
  optionalDependencies?: Record<string, string>;
  peerDependencies?: Record<string, string>;
};

type DependencySection = Exclude<keyof RootPackageJson, 'name'>;

const dependencySections: DependencySection[] = [
  'dependencies',
  'devDependencies',
  'optionalDependencies',
  'peerDependencies',
];

function syncRootDependencyVersion(
  packagePath: string,
  dependencyName: string,
  version: string,
) {
  const rootPackage = loadRootPackage(packagePath);

  if (!rootPackage) {
    console.warn(
      `Unable to locate ${rootPackageName} while syncing ${dependencyName}`,
    );
    return;
  }

  let updated = false;
  for (const section of dependencySections) {
    const deps = rootPackage.json[section];
    if (deps && deps[dependencyName] !== undefined) {
      if (deps[dependencyName] !== version) {
        deps[dependencyName] = version;
        updated = true;
      }
    }
  }

  if (updated) {
    fs.writeFileSync(
      rootPackage.path,
      JSON.stringify(rootPackage.json, null, 2) + '\n',
      'utf8',
    );
    console.log(
      `Updated root ${dependencyName} reference to ${version} in ${rootPackage.path}`,
    );
  }
}

function loadRootPackage(
  startPath: string,
): {path: string; json: RootPackageJson} | null {
  let rootPackage: {path: string; json: RootPackageJson} | null = null;

  findUp.sync(
    directory => {
      const candidate = path.join(directory, 'package.json');
      if (!fs.existsSync(candidate)) {
        return undefined;
      }

      try {
        const packageJson = JSON.parse(
          fs.readFileSync(candidate, 'utf8'),
        ) as RootPackageJson;
        if (packageJson.name === rootPackageName) {
          rootPackage = {path: candidate, json: packageJson};
          return candidate;
        }
      } catch {
        // Ignore JSON parse errors and continue walking up the tree.
      }

      return undefined;
    },
    {cwd: startPath},
  );

  return rootPackage;
}

module.exports = Options;
