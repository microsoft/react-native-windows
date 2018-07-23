'use strict';

const chalk = require('chalk');
const childProcess = require('child_process');
const fs = require('fs');
const path = require('path');
const uuid = require('uuid');
const yeoman = require('yeoman-generator');
const username = require('username');
const os = require('os');

const REACT_NATIVE_PACKAGE_JSON_PATH = function () {
  return path.resolve(
    process.cwd(),
    'node_modules',
    'react-native',
    'package.json'
  );
};

module.exports = yeoman.Base.extend({
  constructor: function () {
    yeoman.Base.apply(this, arguments);
    this.argument('name', { type: String, required: true });

    this.option('ns', {
      desc: 'Namespace for the application (MyNamespace.MyApp)',
      type: String,
      defaults: this.name
    });

    this.option('verbose', {
      desc: 'Enables logging',
      type: Boolean,
      defaults: false
    });
  },

  configuring: function () {
    this.fs.copy(
      this.templatePath('_gitignore'),
      this.destinationPath(path.join('windows', '.gitignore'))
    );

    this.fs.copy(
      this.templatePath('ra_gitignore'),
      this.destinationPath(path.join('windows', this.name, 'ReactAssets', '.gitignore'))
    );
  },

  writing: function () {
    const projectGuid = uuid.v4();
    const packageGuid = uuid.v4();
    const currentUser = username.sync(); // Gets the current username depending on the platform.
    const templateVars = { name: this.name, ns: this.options.ns, certificateThumbprint : null, projectGuid, packageGuid, currentUser };

    console.log('Generating self-signed certificate...');
    if (os.platform() === 'win32') {
      const certGenCommand = [
        `$cert = New-SelfSignedCertificate -KeyUsage DigitalSignature -KeyExportPolicy Exportable -Subject "CN=${currentUser}" -TextExtension @("2.5.29.37={text}1.3.6.1.5.5.7.3.3", "2.5.29.19={text}Subject Type:End Entity") -CertStoreLocation "Cert:\\CurrentUser\\My"`,
        '$pwd = ConvertTo-SecureString -String password -Force -AsPlainText',
        `New-Item -ErrorAction Ignore -ItemType directory -Path ${path.join('windows', this.name)}`,
        `Export-PfxCertificate -Cert "cert:\\CurrentUser\\My\\$($cert.Thumbprint)" -FilePath ${path.join('windows', this.name, this.name)}_TemporaryKey.pfx -Password $pwd`,
        '$cert.Thumbprint'
      ];
      const certGenProcess = childProcess.spawnSync('powershell', ['-command', certGenCommand.join(';')]);

      if (certGenProcess.status === 0) {
        const certGenProcessOutput = certGenProcess.stdout.toString().trim().split('\n');
        templateVars.certificateThumbprint = certGenProcessOutput[certGenProcessOutput.length - 1];
        console.log(chalk.green('Self-signed certificate generated successfully.'));
      } else {
        console.log(chalk.yellow('Failed to generate Self-signed certificate. Using Default Certificate. Use Visual Studio to renew it.'));
        this.fs.copy(
          this.templatePath(path.join('keys', 'MyApp_TemporaryKey.pfx')),
          this.destinationPath(path.join('windows', this.name, this.name + '_TemporaryKey.pfx'))
        );
      }
    } else {
      console.log(chalk.yellow('Using Default Certificate. Use Visual Studio to renew it.'));
      this.fs.copy(
        this.templatePath(path.join('keys', 'MyApp_TemporaryKey.pfx')),
        this.destinationPath(path.join('windows', this.name, this.name + '_TemporaryKey.pfx'))
      );
    }

    this.fs.copyTpl(
      this.templatePath('App.windows.js'),
      this.destinationPath('App.windows.js'),
      templateVars
    );

    this.fs.copyTpl(
      this.templatePath(path.join('src', '**')),
      this.destinationPath(path.join('windows', this.name)),
      templateVars
    );

    this.fs.copyTpl(
      this.templatePath('index.windows.bundle'),
      this.destinationPath(path.join('windows', this.name, 'ReactAssets', 'index.windows.bundle')),
      templateVars
    );

    this.fs.copyTpl(
      this.templatePath(path.join('proj', 'MyApp.csproj')),
      this.destinationPath(path.join('windows', this.name, this.name + '.csproj'))
      , templateVars);

    this.fs.copyTpl(
      this.templatePath(path.join('proj', 'MyApp.sln')),
      this.destinationPath(path.join('windows', this.name + '.sln'))
      , templateVars);

    this.fs.copy(
      this.templatePath(path.join('assets', '**')),
      this.destinationPath(path.join('windows', this.name, 'Assets'))
    );

  },

  install: function () {
    const reactNativeWindowsPackageJson = require('../../package.json');
    const peerDependencies = reactNativeWindowsPackageJson.peerDependencies;
    if (!peerDependencies) {
      return;
    }

    if (fs.existsSync(REACT_NATIVE_PACKAGE_JSON_PATH())) {
      return;
    }

    const reactNativeVersion = peerDependencies['react-native'];
    if (!reactNativeVersion) {
      return;
    }

    const spawnOptions = this.options.verbose ? { stdio: 'inherit' } : {};
    console.log(`Installing react-native@${reactNativeVersion}...`);
    this.npmInstall(`react-native@${reactNativeVersion}`, { '--save': true }, null, spawnOptions);

    const reactVersion = peerDependencies.react;
    if (!reactVersion) {
      return;
    }

    console.log(`Installing react@${reactVersion}...`);
    this.npmInstall(`react@${reactVersion}`, { '--save': true }, null, spawnOptions);
  },

  end: function () {
    //const projectPath = path.resolve(this.destinationRoot(), 'windows', this.name);
    this.log(chalk.white.bold('To run your app on UWP:'));
    this.log(chalk.white('   react-native run-windows'));
  }
});
