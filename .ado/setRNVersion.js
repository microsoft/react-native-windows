// @ts-check
// Script to update dependencies to a specific version of react-native
// This can be triggered using lerna to update the whole repo

const path = require('path');
const fs = require('fs');
const child_process = require('child_process');

if (process.argv.length < 3) {
  throw new Error('Need to provide react-native version, and if you want change files generates the repo root dir as arg');
}
const rnVersion = process.argv[2];

if (!rnVersion.match(/^\d+\.\d+.\d+-microsoft\.\d+$/)) {
  throw new Error('Version format must be X.X.X-microsoft.X');
}
const pkgJsonPath = path.resolve(process.cwd(), 'package.json');

if (!fs.existsSync(pkgJsonPath)) {
  throw new Error('package.json not found. Needs to be run in the root folder of a packge.')
}

const pkgJson = require(pkgJsonPath);

if (process.argv.length === 4) {
  // Generate change file if repo root is provided
  const rootPath = process.argv[3];

  // Generate change file
  const beachballPath = path.resolve(rootPath, 'node_modules/beachball/bin/beachball.js');
  const changeType = pkgJson.name === 'react-native-windows' ? 'hotfix' : 'minor';
  const changeDescription = `"Updating react-native to version: ${rnVersion}"`;
  child_process.execSync(`node ${beachballPath} change --package ${pkgJson.name} --type ${changeType} --message ${changeDescription}`);
} else {
  // Update package.json for new version

  function replaceDeps(dependencies) {
    if (dependencies && dependencies['react-native']) {
      dependencies['react-native'] = dependencies['react-native'].replace(/0\.\d+.\d+-microsoft\.\d+/g, rnVersion);
    }
  }

  replaceDeps(pkgJson.dependencies);
  replaceDeps(pkgJson.peerDependencies);
  replaceDeps(pkgJson.devDependencies);

  fs.writeFileSync(pkgJsonPath, JSON.stringify(pkgJson, null, 2) + '\n');
}
