#!/usr/bin/env node
/**
 * Copyright (c) 2015-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
'use strict';

/**
 * This script bumps a new version for open source releases.
 * It updates the version in json/gradle files and makes sure they are consistent between each other
 * After changing the files it makes a commit and tags it.
 * All you have to do is push changes to remote and CI will make a new build.
 */

const {
  cat,
  echo,
  exec,
  exit,
  ShellString,
} = require('shelljs');

const minimist = require('minimist');

let argv = minimist(process.argv.slice(2), {
  alias: {remote: 'r'},
  default: {remote: 'origin'},
});

// - check we are in release branch, e.g. 0.33-stable
let branch = exec(`git symbolic-ref --short HEAD`, {silent: true}).stdout.trim();

if (branch.indexOf(`-stable`) === -1) {
  echo(`You must be in 0.XX-stable branch to bump a version`);
  exit(1);
}

// e.g. 0.33
let versionMajor = branch.slice(0, branch.indexOf(`-stable`));

// - check that argument version matches branch
// e.g. 0.33.1 or 0.33.0-rc4
let version = argv._[0];
if (!version || version.indexOf(versionMajor) !== 0) {
  echo(`You must pass a tag like 0.${versionMajor}.[X]-rc[Y] to bump a version`);
  exit(1);
}

// Generate version files to detect mismatches between JS and native.
let match = version.match(/^(\d+)\.(\d+)\.(\d+)(?:-(.+))?$/);
if (!match) {
  echo(`You must pass a correctly formatted version; couldn't parse ${version}`);
  exit(1);
}
let [, major, minor, patch, prerelease] = match;

ShellString(cat('scripts/versiontemplates/ReactNativeVersion.cs.template')
  .replace('${major}', major)
  .replace('${minor}', minor)
  .replace('${patch}', patch)
  .replace('${prerelease}', prerelease ? `"${prerelease}"` : 'null'))
  .to('ReactWindows/ReactNative.Shared/Modules/SystemInfo/ReactNativeVersion.cs');

ShellString(cat('scripts/versiontemplates/ReactNativeVersion.windows.js.template')
  .replace('${major}', major)
  .replace('${minor}', minor)
  .replace('${patch}', patch)
  .replace('${prerelease}', prerelease !== undefined ? `'${prerelease}'` : 'null'))
  .to('Libraries/Core/ReactNativeVersion.windows.js');

let packageJson = JSON.parse(cat(`package.json`));
packageJson.version = version;
ShellString(JSON.stringify(packageJson, null, 2)).to(`package.json`);

// - make commit chore(NPM): [${version}] Bump version numbers
if (exec(`git commit -a -m "chore(NPM): [${version}] Bump version numbers"`).code) {
  echo(`failed to commit`);
  exit(1);
}

// - add tag v${version}
if (exec(`git tag v${version}`).code) {
  echo(`failed to tag the commit with v${version}, are you sure this release wasn't made earlier?`);
  echo(`You may want to rollback the last commit`);
  echo(`git reset --hard HEAD~1`);
  exit(1);
}

// Push newly created tag
let remote = argv.remote;
exec(`git push ${remote} v${version}`);

// Tag latest if doing stable release
if (version.indexOf(`rc`) === -1) {
  exec(`git tag -d latest`);
  exec(`git push ${remote} :latest`);
  exec(`git tag latest`);
  exec(`git push ${remote} latest`);
}

exec(`git push ${remote} ${branch} --follow-tags`);

exit(0);
