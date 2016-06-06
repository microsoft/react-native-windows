'use strict';

var chalk = require('chalk');
var fs = require('fs');
var path = require('path');
var utils = require('../generator-utils');
var uuid = require('uuid');
var yeoman = require('yeoman-generator');

const REACT_NATIVE_PACKAGE_JSON_PATH = function() {
  return path.resolve(
    process.cwd(),
    'node_modules',
    'react-native',
    'package.json'
  );
}

module.exports = yeoman.Base.extend({
  constructor: function () {
    yeoman.Base.apply(this, arguments);
    this.argument('name', { type: String, required: true });

    this.option('ns', {
      desc: 'Namespace for the application (MyNamespace.MyApp)',
      type: String,
      defaults: this.name
    });
  },

  configuring: function() {
    this.fs.copy(
      this.templatePath('_gitignore'),
      this.destinationPath(path.join('windows', '.gitignore'))
    );
  },
  
  writing: function () {
    var projectGuid = uuid.v4();
    var packageGuid = uuid.v4();
    var templateVars = { name: this.name, ns: this.options.ns, projectGuid, packageGuid };

    this.fs.copyTpl(
      this.templatePath('index.windows.js'),
      this.destinationPath('index.windows.js'),
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

    this.fs.copy(
      this.templatePath(path.join('keys', 'MyApp_TemporaryKey.pfx')),
      this.destinationPath(path.join('windows', this.name, this.name + '_TemporaryKey.pfx'))
    );
  },

  install: function() {
    var reactNativeWindowsPackageJson = require('../../package.json');
    var peerDependencies = reactNativeWindowsPackageJson.peerDependencies;
    if (!peerDependencies) {
      return;
    }
    
    if (fs.existsSync(REACT_NATIVE_PACKAGE_JSON_PATH())) {
      return;
    }

    var reactNativeVersion = peerDependencies['react-native'];
    if (!reactNativeVersion) {
      return;
    }
    
    console.log(`Installing react-native@${reactNativeVersion}...`);
    this.npmInstall(`react-native@${reactNativeVersion}`, { '--save': true });

    var reactVersion = peerDependencies.react;
    if (!reactVersion) {
      return;
    }

    console.log(`Installing react@${reactVersion}...`);
    this.npmInstall(`react@${reactVersion}`, { '--save': true });
  },

  end: function() {
    var projectPath = path.resolve(this.destinationRoot(), 'windows', this.name);
    this.log(chalk.white.bold('To run your app on UWP:'));
    this.log(chalk.white('   Open ' + projectPath + '.sln in Visual Studio'));
    this.log(chalk.white('   Deploy the application and run on the specified destination'));
  }
});
