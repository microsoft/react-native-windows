/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */
'use strict';

const glob = require('glob');
const path = require('path');

/**
 * Find the main file for the C# project
 *
 * @param {String} folder Name of the folder where to seek
 * @return {String}
 */
module.exports = function findMainFile(folder) {
  let mainFilePath = glob.sync('MainReactNativeHost.cs', {
    cwd: folder,
    ignore: ['node_modules/**', '**/build/**', 'Examples/**', 'examples/**'],
  });

  if (mainFilePath.length === 0) {
    mainFilePath = glob.sync('MainPage.cs', {
      cwd: folder,
      ignore: ['node_modules/**', '**/build/**', 'Examples/**', 'examples/**'],
    });
  }

  return mainFilePath && mainFilePath.length > 0 ? path.join(folder, mainFilePath[0]) : null;
};
