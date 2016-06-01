'use strict';

var fs = require('fs');
var path = require('path');
var yeoman = require('yeoman-environment');

/**
 * Simple utility for running the Windows yeoman generator.
 *
 * @param  {String} projectDir root project directory (i.e. contains index.js)
 * @param  {String} name       name of the root JS module for this app
 * @param  {String} ns         namespace for the project
 */
function generateWindows (projectDir, name, ns) {
  var oldCwd = process.cwd();

  if (!fs.existsSync(projectDir)) {
    fs.mkdirSync(projectDir);
  }

  var env = yeoman.createEnv();
  var generatorPath = path.join(__dirname, 'generator-windows');
  env.register(generatorPath, 'react:windows');
  var args = ['react:windows', name, ns].concat(process.argv.slice(4));
  env.run(args, { ns: ns }, function () {
    process.chdir(oldCwd);
  });
};

module.exports = generateWindows;