'use strict';
const fs = require('fs');
const chalk = require('chalk');
const path = require('path');
const copyAndReplace = require('react-native-local-cli/util/copyAndReplace');
const walk = require('react-native-local-cli/util/walk');
const prompt = require('react-native-local-cli/generator/promptSync')();
const username = require('username');
const uuid = require('uuid');
const childProcess = require('child_process');
const os = require('os');

function createDir(destPath) {
  if (!fs.existsSync(destPath)) {
    fs.mkdirSync(destPath);
  }
}

function copyAndReplaceWithChangedCallback(srcPath, destRoot, relativeDestPath, replacements) {
  if (!replacements) {
    replacements = {};
  }
  const contentChangedCallback = (_, contentChanged) =>
    upgradeFileContentChangedCallback(
      srcPath,
      relativeDestPath,
      contentChanged
    );

  copyAndReplace(
    srcPath,
    path.join(destRoot, relativeDestPath),
    replacements,
    contentChangedCallback
  );
}

function copyAndReplaceAll(srcPath, destPath, relativeDestDir, replacements) {
  walk(srcPath).forEach(absoluteSrcFilePath => {
    const filename = path.relative(srcPath, absoluteSrcFilePath);
    const relativeDestPath = path.join(relativeDestDir, filename);
    copyAndReplaceWithChangedCallback(absoluteSrcFilePath, destPath, relativeDestPath, replacements);
  });
}

function generateCertificate(srcPath, destPath, newProjectName, currentUser) {
  console.log('Generating self-signed certificate...');
  let toCopyTempKey = false;
  if (os.platform() === 'win32') {
    const certGenCommand = [
      `$cert = New-SelfSignedCertificate -KeyUsage DigitalSignature -KeyExportPolicy Exportable -Subject "CN=${currentUser}" -TextExtension @("2.5.29.37={text}1.3.6.1.5.5.7.3.3", "2.5.29.19={text}Subject Type:End Entity") -CertStoreLocation "Cert:\\CurrentUser\\My"`,
      '$pwd = ConvertTo-SecureString -String password -Force -AsPlainText',
      `New-Item -ErrorAction Ignore -ItemType directory -Path ${path.join('windows', newProjectName)}`,
      `Export-PfxCertificate -Cert "cert:\\CurrentUser\\My\\$($cert.Thumbprint)" -FilePath ${path.join('windows', newProjectName, newProjectName)}_TemporaryKey.pfx -Password $pwd`,
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
      path.join('windows', newProjectName, newProjectName + '_TemporaryKey.pfx'));
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

  createDir(path.join(destPath, 'windows'));
  createDir(path.join(destPath, 'windows', newProjectName));
  createDir(path.join(destPath, 'windows', newProjectName, 'ReactAssets'));

  [
    { from: path.join(srcPath, 'App.windows.js'), to: 'App.windows.js' },
    { from: path.join(srcPath, 'proj', 'MyApp.sln'), to: path.join('windows', newProjectName + '.sln') },
    { from: path.join(srcPath, 'proj', 'MyApp.csproj'), to: path.join('windows', newProjectName, newProjectName + '.csproj') },
    { from: path.join(srcPath, '_gitignore'), to: path.join('windows', '.gitignore') },
    { from: path.join(srcPath, 'ra_gitignore'), to: path.join('windows', newProjectName, 'ReactAssets', '.gitignore') },
    { from: path.join(srcPath, 'index.windows.bundle'), to: path.join('windows', newProjectName, 'ReactAssets', 'index.windows.bundle') },
  ].forEach((mapping) => copyAndReplaceWithChangedCallback(mapping.from, destPath, mapping.to, templateVars));

  copyAndReplaceAll(path.join(srcPath, 'assets'), destPath, path.join('windows', newProjectName, 'Assets'), templateVars);
  copyAndReplaceAll(path.join(srcPath, 'src'), destPath, path.join('windows', newProjectName), templateVars);
}

function upgradeFileContentChangedCallback(
  absoluteSrcFilePath,
  relativeDestPath,
  contentChanged
) {
  if (contentChanged === 'new') {
    console.log(`${chalk.bold('new')} ${relativeDestPath}`);
    return 'overwrite';
  }
  if (contentChanged === 'changed') {
    console.log(
      `${chalk.bold(relativeDestPath)} ` +
      `has changed in the new version.\nDo you want to keep your ${relativeDestPath} or replace it with the ` +
      `latest version?\nIf you ever made any changes ` +
      `to this file, you'll probably want to keep it.\n` +
      `You can see the new version here: ${absoluteSrcFilePath}\n` +
      `Do you want to replace ${relativeDestPath}? ` +
      `Answer y to replace, n to keep your version: `
    );
    const answer = prompt();
    if (answer === 'y') {
      console.log(`Replacing ${relativeDestPath}`);
      return 'overwrite';
    }
    console.log(`Keeping your ${relativeDestPath}`);
    return 'keep';
  }
  if (contentChanged === 'identical') {
    return 'keep';
  }
  throw new Error(
    `Unknown file changed state: ${relativeDestPath}, ${contentChanged}`
  );
}

module.exports = {
  copyProjectTemplateAndReplace
};
