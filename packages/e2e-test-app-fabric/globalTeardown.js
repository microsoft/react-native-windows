/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

const {stopImageFixtureServer} = require('./test/imageFixtureServer');

module.exports = async function globalTeardown() {
  const server = globalThis.__RNW_IMAGE_FIXTURE_SERVER__;
  if (server) {
    globalThis.__RNW_IMAGE_FIXTURE_SERVER__ = undefined;
    await stopImageFixtureServer(server);
  }
};
