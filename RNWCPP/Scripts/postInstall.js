// @ts-check
const execSync = require('child_process').execSync;
const path = require('path');
const fs = require('fs');

// The default value of our devDependencies assumes that the react-native package is available on a NPM feed
// For users outside of sdx-platform, we should redirect the dev dependency to a tar from github
let replaceDep = true;

try {
    const rootPath = execSync('git rev-parse --show-toplevel');
    replaceDep = path.normalize(rootPath.toString().trim()) === path.resolve(__dirname, '../..');
}
catch (err) {
    // CLI will fail at the 'git rev-parse' call because it's not in a git repo, but we still need to redirect the dependency
}

if (replaceDep) {
    const pkgJsonPath = path.resolve(__dirname, '../package.json');
    let pkgJson = JSON.parse(fs.readFileSync(pkgJsonPath, 'utf8'));
    pkgJson.devDependencies['react-native'] = pkgJson.rnDepVersion;

    delete pkgJson.rnDepVersion;
    fs.writeFileSync(pkgJsonPath, JSON.stringify(pkgJson, null, 2));
}