#!/usr/bin/env node

/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

require('source-map-support').install();
const cli = require('./lib-commonjs/Cli');

cli.reactNativeWindowsInit();
