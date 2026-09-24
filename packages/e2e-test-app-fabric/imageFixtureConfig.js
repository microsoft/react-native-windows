/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

// Loopback fixture server used by the Image E2E tests. It is started by jest
// (globalSetup.js) and consumed by the app through index.js, so this module has
// to stay free of Node-only imports: it gets bundled into the app by Metro.
const IMAGE_FIXTURE_PORT = 8089;
// Loopback-only test fixture; TLS would add nothing but certificate management.
// eslint-disable-next-line @microsoft/sdl/no-insecure-url
const IMAGE_FIXTURE_BASE_URL = `http://127.0.0.1:${IMAGE_FIXTURE_PORT}/`;

module.exports = {
  IMAGE_FIXTURE_PORT,
  IMAGE_FIXTURE_BASE_URL,
};
