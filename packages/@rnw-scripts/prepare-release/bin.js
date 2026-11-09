#!/usr/bin/env node

/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

require('source-map-support').install();
require('./lib-commonjs/prepareRelease')
  .main()
  .catch(err => {
    console.error(err);
    process.exit(1);
  });
