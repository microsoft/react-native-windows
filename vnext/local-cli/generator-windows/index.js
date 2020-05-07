// @ts-check
'use strict';
const chalk = require('chalk');
const path = require('path');
const username = require('username');
const uuid = require('uuid');
const childProcess = require('child_process');
const fs = require('fs');
const os = require('os');
const {
  createDir,
  copyAndReplaceAll,
  copyAndReplaceWithChangedCallback,
} = require('../generator-common');

const windowsDir = 'windows';
const bundleDir = 'Bundle';

function generateCertificate(srcPath, destPath, newProjectName, currentUser) {
  console.log('Generating self-signed certificate...');
  let toCopyTempKey = false;
  if (os.platform() === 'win32') {
    try {
      const timeout = 10000; // 10 seconds;
      const thumbprint = childProcess.execSync(`powershell -command "Write-Output (New-SelfSignedCertificate -KeyUsage DigitalSignature -KeyExportPolicy Exportable -Subject 'CN=${currentUser}' -TextExtension @('2.5.29.37={text}1.3.6.1.5.5.7.3.3', '2.5.29.19={text}Subject Type:End Entity') -CertStoreLocation 'Cert:\\CurrentUser\\My').Thumbprint"`, {timeout}).toString().trim();
      if (!fs.existsSync(path.join(windowsDir, newProjectName))) {
        fs.mkdirSync(path.join(windowsDir, newProjectName));
      }
      childProcess.execSync(`powershell -command "$pwd = (ConvertTo-SecureString -String password -Force -AsPlainText); Export-PfxCertificate -Cert 'cert:\\CurrentUser\\My\\${thumbprint}' -FilePath ${path.join(windowsDir, newProjectName, newProjectName)}_TemporaryKey.pfx -Password $pwd"`, {timeout});
      console.log(chalk.green('Self-signed certificate generated successfully.'));
      return thumbprint;
      } catch (err) {
        console.log(chalk.yellow('Failed to generate Self-signed certificate. Using Default Certificate. Use Visual Studio to renew it.'));
        toCopyTempKey = true;
      }
    } else {
    console.log(chalk.yellow('Using Default Certificate. Use Visual Studio to renew it.'));
    toCopyTempKey = true;
  }
  if (toCopyTempKey) {
    copyAndReplaceWithChangedCallback(
      path.join(srcPath, 'keys', 'MyApp_TemporaryKey.pfx'),
      destPath,
      path.join(windowsDir, newProjectName, newProjectName + '_TemporaryKey.pfx'));
  }
}

function copyProjectTemplateAndReplace(
  srcRootPath,
  destPath,
  newProjectName,
  options = {}
) {
  if (!srcRootPath) {
    throw new Error('Need a path to copy from');
  }

  if (!destPath) {
    throw new Error('Need a path to copy to');
  }

  if (!newProjectName) {
    throw new Error('Need a project name');
  }

  createDir(path.join(destPath, windowsDir));
  createDir(path.join(destPath, windowsDir, newProjectName));
  createDir(path.join(destPath, windowsDir, newProjectName, bundleDir));
  createDir(path.join(destPath, windowsDir, newProjectName, 'BundleBuilder'));

  const language = options.language;
  const ns = options.ns || newProjectName;
  if (options.experimentalNugetDependency) {
    console.log('Using experimental NuGet dependency.');
  }
  const projDir = options.experimentalNugetDependency ? 'proj-experimental' : 'proj';
  const srcPath = path.join(srcRootPath, language);
  const projectGuid = uuid.v4();
  const rnwVersion = require('react-native-windows/package.json').version;
  const packageGuid = uuid.v4();
  const currentUser = username.sync(); // Gets the current username depending on the platform.
  const certificateThumbprint = generateCertificate(srcPath, destPath, newProjectName, currentUser);

  const templateVars = {
    '// clang-format off': '',
    '// clang-format on': '',
    '<%=ns%>': ns,
    '<%=name%>': newProjectName,
    '<%=projectGuid%>': projectGuid,
    '<%=projectGuidUpper%>': projectGuid.toUpperCase(),
    '<%rnwVersion%>' : rnwVersion,
    '<%=packageGuid%>': packageGuid,
    '<%=currentUser%>': currentUser,
    '<%=certificateThumbprint%>': certificateThumbprint ? `<PackageCertificateThumbprint>${certificateThumbprint}</PackageCertificateThumbprint>` : '',
  };

  [
    { from: path.join(srcRootPath, 'metro.config.js'), to: 'metro.config.js' },
    { from: path.join(srcRootPath, '_gitignore'), to: path.join(windowsDir, '.gitignore') },
    { from: path.join(srcRootPath, 'b_gitignore'), to: path.join(windowsDir, newProjectName, '.gitignore') },
    { from: path.join(srcRootPath, 'index.windows.bundle'), to: path.join(windowsDir, newProjectName, bundleDir, 'index.windows.bundle') },
    { from: path.join(srcPath, projDir, 'MyApp.sln'), to: path.join(windowsDir, newProjectName + '.sln') },
  ].forEach((mapping) => copyAndReplaceWithChangedCallback(mapping.from, destPath, mapping.to, templateVars, options.overwrite));

  if (language === 'cs') {
    [
      { from: path.join(srcPath, projDir, 'MyApp.csproj'), to: path.join(windowsDir, newProjectName, newProjectName + '.csproj') },
    ].forEach((mapping) => copyAndReplaceWithChangedCallback(mapping.from, destPath, mapping.to, templateVars, options.overwrite));
  }
  else {
    [
      { from: path.join(srcPath, projDir, 'MyApp.vcxproj'), to: path.join(windowsDir, newProjectName, newProjectName + '.vcxproj') },
      { from: path.join(srcPath, projDir, 'MyApp.vcxproj.filters'), to: path.join(windowsDir, newProjectName, newProjectName + '.vcxproj.filters') },
      { from: path.join(srcPath, projDir, 'packages.config'), to: path.join(windowsDir, newProjectName, 'packages.config') },
  ].forEach((mapping) => copyAndReplaceWithChangedCallback(mapping.from, destPath, mapping.to, templateVars, options.overwrite));

  // Once we are publishing to nuget.org, this shouldn't be needed anymore
  if (options.experimentalNugetDependency) {
    [
      { from: path.join(srcPath, projDir, 'NuGet.Config'), to: path.join(windowsDir, 'NuGet.Config') },
  ].forEach((mapping) => copyAndReplaceWithChangedCallback(mapping.from, destPath, mapping.to, templateVars, options.overwrite));
  }
  }

  copyAndReplaceAll(path.join(srcPath, 'assets'), destPath, path.join(windowsDir, newProjectName, 'Assets'), templateVars, options.overwrite);
  copyAndReplaceAll(path.join(srcPath, 'src'), destPath, path.join(windowsDir, newProjectName), templateVars, options.overwrite);

  console.log(chalk.white.bold('To run your app on UWP:'));
  console.log(chalk.white('   npx react-native run-windows'));
}

function installDependencies(options) {
  const cwd = process.cwd();

  // Extract react-native peer dependency version
  const vnextPackageJsonPath = require.resolve('react-native-windows/package.json', {paths: [process.cwd()]});
  const vnextPackageJson = JSON.parse(fs.readFileSync(vnextPackageJsonPath, { encoding: 'UTF8' }));
  let reactNativeVersion = vnextPackageJson.peerDependencies['react-native'];
  const depDelim = ' || ';
  const delimIndex = reactNativeVersion.indexOf(depDelim);
  if (delimIndex !== -1) {
    reactNativeVersion = reactNativeVersion.slice(0, delimIndex);
  }

  console.log(chalk.green('Updating to compatible version of react-native:'));
  console.log(chalk.white(`    ${reactNativeVersion}`));

  // Patch package.json to have proper react-native version and install
  const projectPackageJsonPath = path.join(cwd, 'package.json');
  const projectPackageJson = JSON.parse(fs.readFileSync(projectPackageJsonPath, { encoding: 'UTF8' }));
  projectPackageJson.scripts.windows = 'react-native run-windows';
  projectPackageJson.dependencies['react-native'] = reactNativeVersion;
  fs.writeFileSync(projectPackageJsonPath, JSON.stringify(projectPackageJson, null, 2));

  // Install dependencies using correct package manager
  const isYarn = fs.existsSync(path.join(cwd, 'yarn.lock'));
  childProcess.execSync(isYarn ? 'yarn' : 'npm i', options && options.verbose ? { stdio: 'inherit' } : {});
}

module.exports = {
  copyProjectTemplateAndReplace,
  installDependencies,
};
