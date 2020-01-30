/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */
'use strict';

const fs = require('fs');
const glob = require('glob');
const path = require('path');

/**
 * Gets package's namespace
 * by searching for its declaration in all C# files present in the folder
 *
 * @param {String} folder Folder to find C# files
 */
module.exports = function getNamespace(folder) {
  const files = glob.sync('**/*.cs', { cwd: folder });

  const packages = files
    .map(filePath => fs.readFileSync(path.join(folder, filePath), 'utf8'))
    .map(file => file.match(/namespace (.*)[\s\S]+IReactPackage/))
    .filter(match => match);

  return packages.length ? packages[0][1] : null;
};
