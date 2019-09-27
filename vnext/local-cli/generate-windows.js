'use strict';

const fs = require('fs');
const path = require('path');
const {
  copyProjectTemplateAndReplace,
  installDependencies,
 } = require('./generator-windows');

/**
 * Simple utility for running the Windows generator.
 *
 * @param  {String} projectDir root project directory (i.e. contains index.js)
 * @param  {String} name       name of the root JS module for this app
 * @param  {String} ns         namespace for the project
 * @param  {Object} options    command line options container
 */
function generateWindows (projectDir, name, ns, options) {
  if (!fs.existsSync(projectDir)) {
    fs.mkdirSync(projectDir);
  }

  installDependencies(options);

  copyProjectTemplateAndReplace(
    path.join(__dirname, 'generator-windows', 'templates'),
    projectDir,
    name,
    { ns, overwrite: options.overwrite }
  );
}

module.exports = generateWindows;
