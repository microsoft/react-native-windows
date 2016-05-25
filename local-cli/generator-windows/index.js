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

    this.fs.copy(
      this.templatePath('_gitignore'),
      this.destinationPath('.gitignore')
    );
    this.fs.copy(
      this.templatePath('_watchmanconfig'),
      this.destinationPath('.watchmanconfig')
    );
    this.fs.copy(
      this.templatePath('_buckconfig'),
      this.destinationPath('.buckconfig')
    );
  },

  writing: function () {
    if (this.options.upgrade) {
      // never upgrade index.*.js files
      return;
    }

    this.fs.copyTpl(
      this.templatePath('index.windows.js'),
      this.destinationPath('index.windows.js'),
      { name: this.name }
    );
  },

  install: function() {
    if (this.options.upgrade) {
      return;
    }

    var reactNativePackageJson = require('../../package.json');
    var { peerDependencies } = reactNativePackageJson;
    if (!peerDependencies) {
      return;
    }

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
