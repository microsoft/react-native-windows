/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

const {startImageFixtureServer} = require('./test/imageFixtureServer');

module.exports = async function globalSetup() {
  globalThis.__RNW_IMAGE_FIXTURE_SERVER__ = await startImageFixtureServer();
};
