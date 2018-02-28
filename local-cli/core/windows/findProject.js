/**
 * Copyright (c) 2015-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
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
