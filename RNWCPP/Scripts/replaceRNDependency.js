// @ts-check
const path = require('path');
const fs = require('fs');

const pkgJsonPath = path.resolve(__dirname, '../package.json');
let pkgJson = JSON.parse(fs.readFileSync(pkgJsonPath, 'utf8'));
pkgJson.rnDepVersion = pkgJson.devDependencies['react-native'];
pkgJson.devDependencies['react-native'] = `https://github.com/Microsoft/react-native/archive/v${pkgJson.devDependencies['react-native']}.tar.gz`;
fs.writeFileSync(pkgJsonPath, JSON.stringify(pkgJson, null, 2));