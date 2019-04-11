// @ts-check
const execSync = require('child_process').execSync;
const path = require('path');
const fs = require('fs');
const rootPath = execSync('git rev-parse --show-toplevel');

// The default value of our devDependencies assumes that the react-native package is available on a NPM feed
// For users outside of sdx-platform, we should redirect the dev dependency to a tar from github
if (path.normalize(rootPath.toString().trim()) === path.resolve(__dirname, '../..')) {
    const pkgJsonPath = path.resolve(__dirname, '../package.json');
    let pkgJson = JSON.parse(fs.readFileSync(pkgJsonPath, 'utf8'));

    if (pkgJson.devDependencies['react-native'].indexOf('https://github.com') === -1) {

        pkgJson.rnDepVersion = pkgJson.devDependencies['react-native'];
        pkgJson.devDependencies['react-native'] = `https://github.com/Microsoft/react-native/archive/v${pkgJson.devDependencies['react-native']}.tar.gz`;
        fs.writeFileSync(pkgJsonPath, JSON.stringify(pkgJson, null, 2));
    }
}