'use strict';
const chalk = require('chalk');
const path = require('path');
const username = require('username');
const uuid = require('uuid');
const childProcess = require('child_process');
const os = require('os');
const {
  createDir,
  copyAndReplaceAll,
  copyAndReplaceWithChangedCallback
} = require('../generator-common');

const windowsDir = 'windows';
const reactAssetsDir = 'ReactAssets';
const projDir = 'proj';

function generateCertificate(srcPath, destPath, newProjectName, currentUser) {
  console.log('Generating self-signed certificate...');
  let toCopyTempKey = false;
  if (os.platform() === 'win32') {
    const certGenCommand = [
      `$cert = New-SelfSignedCertificate -KeyUsage DigitalSignature -KeyExportPolicy Exportable -Subject "CN=${currentUser}" -TextExtension @("2.5.29.37={text}1.3.6.1.5.5.7.3.3", "2.5.29.19={text}Subject Type:End Entity") -CertStoreLocation "Cert:\\CurrentUser\\My"`,
      '$pwd = ConvertTo-SecureString -String password -Force -AsPlainText',
      `New-Item -ErrorAction Ignore -ItemType directory -Path ${path.join(windowsDir, newProjectName)}`,
      `Export-PfxCertificate -Cert "cert:\\CurrentUser\\My\\$($cert.Thumbprint)" -FilePath ${path.join(windowsDir, newProjectName, newProjectName)}_TemporaryKey.pfx -Password $pwd`,
      '$cert.Thumbprint'
    ];
    const certGenProcess = childProcess.spawnSync('powershell', ['-command', certGenCommand.join(';')]);

    if (certGenProcess.status === 0) {
      const certGenProcessOutput = certGenProcess.stdout.toString().trim().split('\n');
      console.log(chalk.green('Self-signed certificate generated successfully.'));
      return certGenProcessOutput[certGenProcessOutput.length - 1];
    } else {
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
  srcPath,
  destPath,
  newProjectName,
  options = {}
) {
  if (!srcPath) {
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
  createDir(path.join(destPath, windowsDir, newProjectName, reactAssetsDir));

  const ns = options.ns || newProjectName;
  const projectGuid = uuid.v4();
  const packageGuid = uuid.v4();
  const currentUser = username.sync(); // Gets the current username depending on the platform.
  const certificateThumbprint = generateCertificate(srcPath, destPath, newProjectName, currentUser);

  const templateVars = {
    '<%=ns%>': ns,
    '<%=name%>': newProjectName,
    '<%=projectGuid%>': projectGuid,
    '<%=packageGuid%>': packageGuid,
    '<%=currentUser%>': currentUser,
    '<%=certificateThumbprint%>': certificateThumbprint ? `<PackageCertificateThumbprint>${certificateThumbprint}</PackageCertificateThumbprint>` : ''
  };

  [
    { from: path.join(srcPath, 'App.windows.js'), to: 'App.windows.js' },
    { from: path.join(srcPath, projDir, 'MyApp.sln'), to: path.join(windowsDir, newProjectName + '.sln') },
    { from: path.join(srcPath, projDir, 'MyApp.csproj'), to: path.join(windowsDir, newProjectName, newProjectName + '.csproj') },
    { from: path.join(srcPath, '_gitignore'), to: path.join(windowsDir, '.gitignore') },
    { from: path.join(srcPath, 'ra_gitignore'), to: path.join(windowsDir, newProjectName, reactAssetsDir, '.gitignore') },
    { from: path.join(srcPath, 'index.windows.bundle'), to: path.join(windowsDir, newProjectName, reactAssetsDir, 'index.windows.bundle') },
  ].forEach((mapping) => copyAndReplaceWithChangedCallback(mapping.from, destPath, mapping.to, templateVars));

  copyAndReplaceAll(path.join(srcPath, 'assets'), destPath, path.join(windowsDir, newProjectName, 'Assets'), templateVars);
  copyAndReplaceAll(path.join(srcPath, 'src'), destPath, path.join(windowsDir, newProjectName), templateVars);

  console.log(chalk.white.bold('To run your app on UWP:'));
  console.log(chalk.white('   react-native run-windows'));
}

module.exports = {
  copyProjectTemplateAndReplace
};
