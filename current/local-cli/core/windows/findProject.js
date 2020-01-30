/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */
'use strict';

const findProjectWithFilePattern = require('./findProjectWithFilePattern');

/**
 * Find a C# dependency project file
 *
 * @param {String} folder Name of the folder where to seek
 * @return {String}
 */
module.exports = function findDependencyProject(folder) {
  return findProjectWithFilePattern(folder, /class\s+\S+\s*:\s*(ReactNativeHost|ReactPage)/);
};
