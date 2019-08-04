// @ts-check
// Script to update dependencies to a specific version of react-native
// This can be triggered using lerna to update the whole repo

const path = require('path');
const fs = require('fs');

if (process.argv.length < 3) {
  throw new Error('Need to provide react-native version as arg');
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

function replaceDeps(dependencies) {
  if (dependencies && dependencies['react-native']) {
    dependencies['react-native'] = dependencies['react-native'].replace(/0\.\d+.\d+-microsoft\.\d+/g, rnVersion);
  }
}

replaceDeps(pkgJson.dependencies);
replaceDeps(pkgJson.peerDependencies);
replaceDeps(pkgJson.devDependencies);

fs.writeFileSync(pkgJsonPath, JSON.stringify(pkgJson, null, 2) + '\n');