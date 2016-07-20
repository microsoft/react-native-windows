'use strict';

const fs = require('fs');
const path = require('path');
const yeoman = require('yeoman-environment');

/**
 * Simple utility for running the Windows yeoman generator.
 *
 * @param  {String} projectDir root project directory (i.e. contains index.js)
 * @param  {String} name       name of the root JS module for this app
 * @param  {String} ns         namespace for the project
 */
function generateWindows (projectDir, name, ns) {
  const oldCwd = process.cwd();

  if (!fs.existsSync(projectDir)) {
    fs.mkdirSync(projectDir);
  }

  const env = yeoman.createEnv();
  const generatorPath = path.join(__dirname, 'generator-windows');
  env.register(generatorPath, 'react:windows');
  const args = ['react:windows', name, ns].concat(process.argv.slice(4));
  env.run(args, { ns: ns }, function () {
    process.chdir(oldCwd);
  });
}

module.exports = generateWindows;
