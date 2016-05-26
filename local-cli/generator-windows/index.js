'use strict';

var chalk = require('chalk');
var fs = require('fs');
var path = require('path');
var yeoman = require('yeoman-generator');
var utils = require('../generator-utils');

module.exports = yeoman.Base.extend({
  constructor: function () {
    yeoman.Base.apply(this, arguments);
    this.argument('name', { type: String, required: true });

    this.option('ns', {
      desc: 'Namespace for the application (MyNamespace.MyApp)',
      type: String,
      defaults: this.name
    });

    this.option('upgrade', {
      desc: 'Specify an upgrade',
      type: Boolean,
      defaults: false
    });
  },

  configuring: function() {
    /* TODO: Fix
    utils.copyAndReplace(
      this.templatePath('../../../.flowconfig'),
      this.destinationPath('.flowconfig'),
      {
        'Libraries\/react-native\/react-native-interface.js' : 'node_modules/react-native/Libraries/react-native/react-native-interface.js',
        '^flow/$' : 'node_modules/react-native/flow\nflow/'
      }
    );
    */

    this.fs.copyTpl(
      this.templatePath('_gitignore'),
      this.destinationPath('.gitignore'),
      { name: this.name }
    );
    this.fs.copy(
      this.templatePath('_watchmanconfig'),
      this.destinationPath('.watchmanconfig')
    );
  },

  writing: function () {
    if (this.options.upgrade) {
      // never upgrade index.*.js files
      return;
    }

    var templateVars = { name: this.name, ns: this.options.ns };

    this.fs.copyTpl(
      this.templatePath('index.windows.js'),
      this.destinationPath('index.windows.js'),
      templateVars
    );

    this.fs.copyTpl(
      this.templatePath(path.join('src', '**')),
      this.destinationPath(path.join('windows', this.name))
    , templateVars);

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

    this.fs.copy(
      this.templatePath(path.join('keys', 'MyApp_TemporaryKey.pfx')),
      this.destinationPath(path.join('windows', this.name, this.name + '_TemporaryKey.pfx'))
    );
  },

  install: function() {
    if (this.options.upgrade) {
      return;
    }

    var reactNativeWindowsPackageJson = require('../../package.json');
    var { peerDependencies } = reactNativeWindowsPackageJson;
    if (!peerDependencies) {
      return;
    }

    var reactNativeVersion = peerDependencies['react-native'];
    if (!reactNativeVersion) {
      return;
    }

    this.npmInstall(`react-native@${reactNativeVersion}`, { '--save': true });

    var reactVersion = peerDependencies.react;
    if (!reactVersion) {
      return;
    }

    this.npmInstall(`react@${reactVersion}`, { '--save': true });
  },

  end: function() {
    var projectPath = path.resolve(this.destinationRoot(), 'windows', this.name);
    this.log(chalk.white.bold('To run your app on UWP:'));
    this.log(chalk.white('   cd ' + this.destinationRoot()));
    this.log(chalk.white('   react-native run-windows'));
    this.log(chalk.white('   - or -'));
    this.log(chalk.white('   Open ' + projectPath + '.sln in Visual Studio'));
    this.log(chalk.white('   Deploy the application and run on the specified destination'));
    }
});
