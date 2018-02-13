'use strict';

const chalk = require('chalk');
const fs = require('fs');
const path = require('path');
const uuid = require('uuid');
const yeoman = require('yeoman-generator');
const destFolder = 'wpf';

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
      this.destinationPath(path.join(destFolder, '.gitignore'))
    );
  },

  writing: function () {
    const projectGuid = uuid.v4();
    const packageGuid = uuid.v4();
    const templateVars = { name: this.name, ns: this.options.ns, projectGuid, packageGuid };

    this.fs.copyTpl(
      this.templatePath('App.windows.js'),
      this.destinationPath('App.windows.js'),
      templateVars
    );

    this.fs.copyTpl(
      this.templatePath(path.join('src', '**')),
      this.destinationPath(path.join(destFolder, this.name)),
      templateVars
    );

    this.fs.copyTpl(
      this.templatePath('index.wpf.bundle'),
      this.destinationPath(path.join(destFolder, this.name, 'ReactAssets', 'index.wpf.bundle')),
      templateVars
    );

    this.fs.copyTpl(
      this.templatePath(path.join('proj', 'MyApp.csproj')),
      this.destinationPath(path.join(destFolder, this.name, this.name + '.csproj'))
    , templateVars);

    // necesarry for special NuGet config
    this.fs.copyTpl(
      this.templatePath(path.join('proj', 'NuGet.Config')),
      this.destinationPath(path.join(destFolder, 'NuGet.Config'))
    , templateVars);

    this.fs.copyTpl(
      this.templatePath(path.join('proj', 'App.config')),
      this.destinationPath(path.join(destFolder, this.name, 'App.config'))
    , templateVars);

    this.fs.copyTpl(
      this.templatePath(path.join('proj', 'packages.config')),
      this.destinationPath(path.join(destFolder, this.name, 'packages.config'))
    , templateVars);

    this.fs.copyTpl(
      this.templatePath(path.join('proj', 'MyApp.sln')),
      this.destinationPath(path.join(destFolder, this.name + '.sln'))
    , templateVars);

    this.fs.copy(
      this.templatePath(path.join('assets', '**')),
      this.destinationPath(path.join(destFolder, this.name, 'Assets'))
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
    this.log(chalk.white.bold('To run your app on WPF:'));
    this.log(chalk.white('   react-native run-wpf'));
  }
});
