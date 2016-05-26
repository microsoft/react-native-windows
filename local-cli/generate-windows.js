'use strict';

var fs = require('fs');
var childProcess = require('child_process');
var path = require('path');
var yeoman = require('yeoman-environment');

/**
 * Simple utility for running the Windows yeoman generator.
 *
 * @param  {String} projectDir root project directory (i.e. contains index.js)
 * @param  {String} name       name of the root JS module for this app
 */
function generateWindows (projectDir, name, ns) {
  var oldCwd = process.cwd();

  var exists = fs.existsSync(projectDir);
  exists && childProcess.execSync('rmdir '+ projectDir + ' /s /q');
  !exists && fs.mkdirSync(projectDir);

  process.chdir(projectDir);

  var env = yeoman.createEnv();
  var generatorPath = path.join(__dirname, 'generator-windows');
  env.register(generatorPath, 'react:app');
  var args = ['react:app', name, ns].concat(process.argv.slice(4));
  env.run(args, { ns: ns }, function () {
    process.chdir(oldCwd);
  });
};

generateWindows('C:\\github\\foo', 'Foo', 'Bar');

