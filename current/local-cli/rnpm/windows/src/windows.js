/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */
'use strict';

const Common = require('./common');
const chalk = require('chalk');
const execSync = require('child_process').execSync;
const path = require('path');
const fs = require('fs');

const REACT_NATIVE_WINDOWS_GENERATE_PATH = function () {
  return path.resolve(
    process.cwd(),
    'node_modules',
    'react-native-windows',
    'local-cli',
    'generate-windows.js'
  );
};

/**
 * Returns latest version of react-native-windows where version contains 'vnext'
 * @param {'yarn'|'npm'} pkgmgr
 * @param {boolean} isYarn
 */
const getLatestVnextVersion = function (pkgmgr, isYarn) {
  let response = JSON.parse(execSync(`${pkgmgr} info react-native-windows --silent --json`));
  response = isYarn ? response.data : response;
  const version = response["dist-tags"].vnext;
  return version;
}

module.exports = function windows(config, args, options) {
  const name = args[0] ? args[0] : Common.getReactNativeAppName();
  const ns = options.namespace ? options.namespace : name;
  const template = options.template;
  const execOptions = options.verbose ? { stdio: 'inherit' } : {};
  const cwd = process.cwd();
  const isYarn = Common.isGlobalCliUsingYarn(cwd);

  if (template === 'vnext') {
    const pkgmgr = isYarn ? 'yarn' : 'npm';
    const version = options.windowsVersion ? options.windowsVersion : getLatestVnextVersion(pkgmgr, isYarn);
    const projectPackageJson = path.join(cwd, 'package.json');
    const reactNativeWindowsPackageJson = path.join(cwd, 'node_modules', 'react-native-windows', 'package.json');

    console.log('Installing deps...');
    return Common.getInstallPackage(version).then((rnwPackage) => {
      console.log(`Installing ${rnwPackage}...`);
      const pkgmgrInstall = isYarn ? `${pkgmgr} add` : `${pkgmgr} install --save`
      execSync(`${pkgmgrInstall} ${rnwPackage}`, execOptions);
      console.log(chalk.green(`${rnwPackage} successfully installed.`));

      const vnextPackageJson = JSON.parse(fs.readFileSync(reactNativeWindowsPackageJson, { encoding: 'UTF8' }));
      let reactNativeVersion = vnextPackageJson.peerDependencies['react-native'];
      const depDelim = ' || ';
      const delimIndex = reactNativeVersion.indexOf(depDelim);
      if (delimIndex !== -1) {
        reactNativeVersion = reactNativeVersion.substring(delimIndex + depDelim.length);
      }

      // patching package.json to have proper react-native version and start script
      const prjPackageJson = JSON.parse(fs.readFileSync(projectPackageJson, { encoding: 'UTF8' }));
      prjPackageJson.scripts.start = 'node node_modules/react-native-windows/Scripts/cli.js start';
      prjPackageJson.dependencies['react-native'] = reactNativeVersion;
      fs.writeFileSync(projectPackageJson, JSON.stringify(prjPackageJson, null, 2));
      execSync(isYarn ? pkgmgr : `${pkgmgr} i`, execOptions);

      const generateWindows = require(REACT_NATIVE_WINDOWS_GENERATE_PATH());
      generateWindows(process.cwd(), name, ns);
    });
  }
  else {
    const version = options.windowsVersion ? options.windowsVersion : Common.getReactNativeVersion();
    return Common.getInstallPackage(version)
      .then(rnwPackage => {
        console.log(`Installing ${rnwPackage}...`);
        const pkgmgr = isYarn ? 'yarn add' : 'npm install --save';

        execSync(`${pkgmgr} ${rnwPackage}`, execOptions);
        console.log(chalk.green(`${rnwPackage} successfully installed.`));

        const generateWindows = require(REACT_NATIVE_WINDOWS_GENERATE_PATH());
        generateWindows(process.cwd(), name, ns);
      }).catch(error => console.error(chalk.red(error.message)));
  }
};
