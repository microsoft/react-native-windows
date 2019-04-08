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
const fsextra = require('fs-extra');

const REACT_NATIVE_WINDOWS_GENERATE_PATH = function() {
  return path.resolve(
    process.cwd(),
    'node_modules',
    'react-native-windows',
    'local-cli',
    'generate-windows.js'
  );
};

const RNWCPP_GENERATE_PATH = function() {
  return path.resolve(
    process.cwd(),
    'windows',
    'lib',
    'react-native-windows',
    'RNWCPP',
    'local-cli',
    'generate-windows.js'
  );
};

module.exports = function windows(config, args, options) {
  const name = args[0] ? args[0] : Common.getReactNativeAppName();
  const ns = options.namespace ? options.namespace : name;
  const version = options.windowsVersion ? options.windowsVersion : Common.getReactNativeVersion();
  const template = options.template;

  if (template === 'csharp'){
    return Common.getInstallPackage(version)
    .then(rnwPackage => {
      console.log(`Installing ${rnwPackage}...`);
      const pkgmgr = Common.isGlobalCliUsingYarn(process.cwd()) ? 'yarn add' : 'npm install --save';

      const execOptions = options.verbose ? { stdio: 'inherit' } : {};
      execSync(`${pkgmgr} ${rnwPackage}`, execOptions);
      console.log(chalk.green(`${rnwPackage} successfully installed.`));

      const generateWindows = require(REACT_NATIVE_WINDOWS_GENERATE_PATH());
      generateWindows(process.cwd(), name, ns);
    }).catch(error => console.error(chalk.red(error.message)));
  }
  if (template === 'vnext') {
    const cwd = process.cwd();
    const projectPackageJson = path.join(cwd, 'package.json');
    const libDirPath = path.join(cwd, 'windows', 'lib');
    const libPath = path.join(libDirPath, 'react-native-windows.zip');
    const libDir = path.join(libDirPath, 'react-native-windows');
    const unzipedLib = path.join(libDirPath, 'react-native-windows', `react-native-windows-mostap-rnwcppcli`);
    const RNWCS = path.join(libDirPath, 'react-native-windows', 'RNWCS');
    const RNWCPP = path.join(libDir, 'RNWCPP');
    const RNWCPPNodeModules = path.join(RNWCPP, 'node_modules');
    const RNWCPPPackageJson = path.join(RNWCPP, 'package.json');
    const RNWCPPNpmrc = path.join(RNWCPP, '.npmrc');

    console.log('Downloading lib...');
    return Common.downloadFile(
      libPath,
      `https://github.com/in4margaret/react-native-windows/archive/mostap%2Frnwcppcli.zip`
    ).then(function () {
      console.log('Unziping lib...');
      return Common.unzipFile(libPath, libDir);
    }).then(function () {
      fsextra.moveSync(unzipedLib, libDir);
      fs.unlinkSync(libPath);
      fsextra.removeSync(RNWCS);

      console.log('Installing deps...');
      const isYarn = Common.isGlobalCliUsingYarn(process.cwd());
      const pkgmgr = isYarn ? 'yarn' : 'npm';
      const execOptions = { cwd: RNWCPP };
      if (true) {
        execOptions.stdio = 'inherit';
      }
      execSync(isYarn ? pkgmgr : `${pkgmgr} i`, execOptions);
      console.log('Building lib...');
      execSync(`${pkgmgr} run build`, execOptions);
      fsextra.removeSync(RNWCPPNodeModules);

      // patching package.json to have only necessary dep
      const rnwcppPackageJson = JSON.parse(fs.readFileSync(RNWCPPPackageJson, { encoding: 'UTF8' }));
      const reactNativeVersion = rnwcppPackageJson.dependencies['react-native'];
      rnwcppPackageJson.dependencies = ['react-native-local-cli', 'username', 'uuid'].reduce(function (result, libName) {
        result[libName] = rnwcppPackageJson.dependencies[libName];
        return result;
      }, {});
      rnwcppPackageJson.devDependencies = {};
      fs.writeFileSync(RNWCPPPackageJson, JSON.stringify(rnwcppPackageJson, null, 2));
      execSync(isYarn ? pkgmgr : `${pkgmgr} i`, execOptions);

      // copying .npmrc file
      fsextra.copyFileSync(RNWCPPNpmrc, path.join(cwd, '.npmrc'));
      // pathching package.json to have proper react-native windows and start function
      const prjPackageJson = JSON.parse(fs.readFileSync(projectPackageJson, { encoding: 'UTF8' }));
      prjPackageJson.scripts.start = 'node windows/lib/react-native-windows/RNWCPP/Scripts/cli.js start';
      prjPackageJson.dependencies['react-native'] = reactNativeVersion;
      fs.writeFileSync(projectPackageJson, JSON.stringify(prjPackageJson, null, 2));
      execSync(isYarn ? pkgmgr : `${pkgmgr} i`, { ...execOptions, cwd: cwd });

      const generateWindows = require(RNWCPP_GENERATE_PATH());
      generateWindows(process.cwd(), name, ns);
    }).catch(error => console.error(chalk.red(error.message || error)));
  }

  console.error(chalk.red(`Template :${template} doesn't exist.`));
};
