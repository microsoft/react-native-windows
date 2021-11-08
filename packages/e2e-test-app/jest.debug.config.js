/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

const config = require('./jest.config');

config.testEnvironmentOptions.breakOnStart = true;
module.exports = config;
