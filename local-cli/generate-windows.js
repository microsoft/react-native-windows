'use strict';

var fs = require('fs');
var path = require('path');
var yeoman = require('yeoman-environment');

/**
 * Simple utility for running the Windows yeoman generator.
 *
 * @param  {String} projectDir root project directory (i.e. contains index.js)
 * @param  {String} name       name of the root JS module for this app
 */
function generateWindows (projectDir, name) {
  var oldCwd = process.cwd();
  process.chdir(projectDir);

  var env = yeoman.createEnv();
  var generatorPath = path.join(__dirname, 'generator-windows');
  env.register(generatorPath, 'react:app:windows');
  var args = ['react:app:windows', name].concat(process.argv.slice(4));
  env.run(args, undefined, function () {
    process.chdir(oldCwd);
  });
};

generateWindows(path.join(__dirname), 'foo');

