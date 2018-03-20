/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */
'use strict';

const fs = require('fs');
const glob = require('glob');
const path = require('path');

/**
 * Find an C# project file
 *
 * @param {String} folder Name of the folder where to seek
 * @return {String}
 */
module.exports = function findProjectWithFilePattern(folder, pattern) {
  const csprojPaths = glob.sync(path.join('**', '*.csproj'), {
    cwd: folder,
    ignore: ['node_modules/**', '**/build/**', 'Examples/**', 'examples/**', '**/bin/**', '**/obj/**' ],
  });

  // Search all folders with *.csproj files for one that exports an `IReactPackage` 
  for (var csprojPath of csprojPaths) {
    const csprojFullPath = path.join(folder, csprojPath);
    const csprojDir = path.dirname(csprojFullPath);
    const files = glob.sync(path.join('**', '*.cs'), {
      cwd: csprojDir,
      ignore: ['node_modules/**', '**/build/**', 'Examples/**', 'examples/**', '**/bin/**', '**/obj/**' ],
    });

    const packages = files
      .map(filePath => fs.readFileSync(path.join(csprojDir, filePath), 'utf8'))
      .map(file => file.match(pattern))
      .filter(match => match);

    if (packages.length) {
      return csprojFullPath;
    }
  }

  return null;
};
