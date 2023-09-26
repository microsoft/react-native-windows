/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @ts check
 * @format
 */

// This template calls the old generateWindows template system,
// without any experimental features, i.e. the equivalence of:
// npx react-native-windows --language cpp --projectType app

const {makeGenerateWindowsWrapper} = require('../generateWrapper');

module.exports = makeGenerateWindowsWrapper('cpp', 'app', true); // TODO: Remove this as the default
