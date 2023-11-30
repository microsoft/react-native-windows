/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

const fs = require('fs');
const path = require('path');
const sanitizeFilename = require('sanitize-filename');
// disabled temporarily
//const {LogBox} = require('react-native');

const screenshotDir = './errorShots';
fs.mkdirSync(screenshotDir, {recursive: true});

process.env.NODE_ENV = 'test';

// disabled temporarily
//LogBox.ignoreAllLogs(true);
