/**
 * Copyright (c) 2015-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */
'use strict';

const fs = require('fs');

/**
 * Gets GUID of dependency project.
 *
 * @param {String} folder Path to dependency project .csproj
 */
module.exports = function findProjectGuid(path) {
  const file = fs.readFileSync(path, 'utf8');
  const result = file.match(/<ProjectGuid>\{(.*)\}<\/ProjectGuid>/);
  return result ? result[1] : null;
};
