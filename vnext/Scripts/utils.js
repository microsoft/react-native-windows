/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
'use strict';
const path = require('path');
const fs = require('fs');

module.exports = {
  /**
   * Looks for a file in the directories above the current directory in the path.
   * @param {string} dir absolute path to directory
   * @param {string} filename file name to find
   * @returns Path of a directory that contains 'filename'
   */
  getDirectoryNameOfFileAbove: function(dir, filename) {
    const {root} = path.parse(dir);
    let currentDir = dir;
    do {
      if (fs.existsSync(path.join(currentDir, filename))) {
        return currentDir;
      }
      currentDir = path.resolve(currentDir, '..');
    } while (currentDir !== root);
  },
};
