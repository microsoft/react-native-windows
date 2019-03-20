// @ts-check
const path = require('path');
const fs = require('fs');

fs.unlinkSync(path.resolve(__dirname, '../.npmrc'));

const pkgJsonPath = path.resolve(__dirname, '../package.json');
let pkgJson = JSON.parse(fs.readFileSync(pkgJsonPath, 'utf8'));
pkgJson.dependencies['react-native'] = `https://github.com/Microsoft/react-native/archive/v${pkgJson.dependencies['react-native']}.tar.gz`;
fs.writeFileSync(pkgJsonPath, JSON.stringify(pkgJson, null, 2));