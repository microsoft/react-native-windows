/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
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
