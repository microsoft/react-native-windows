'use strict';

const fs = require('fs');
const path = require('path');
const copyProjectTemplateAndReplace = require('./generator-wpf').copyProjectTemplateAndReplace;

/**
 * Simple utility for running the Windows generator.
 *
 * @param  {String} projectDir root project directory (i.e. contains index.js)
 * @param  {String} name       name of the root JS module for this app
 * @param  {String} ns         namespace for the project
 */
function generateWindows (projectDir, name, ns) {
  if (!fs.existsSync(projectDir)) {
    fs.mkdirSync(projectDir);
  }
  copyProjectTemplateAndReplace(
    path.join(__dirname, 'generator-wpf', 'templates'),
    projectDir,
    name,
    { ns }
  );
}

module.exports = generateWindows;
