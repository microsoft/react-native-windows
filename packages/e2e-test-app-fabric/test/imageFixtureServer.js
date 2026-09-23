/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

const fs = require('fs');
const http = require('http');
const path = require('path');

const {IMAGE_FIXTURE_PORT} = require('../imageFixtureConfig');

// These are the exact files the Image examples used to download from
// raw.githubusercontent.com, so the images the app renders - and the visual
// trees captured from them - are unchanged.
const FIXTURE_DIR = path.resolve(
  __dirname,
  '../../@react-native/tester/js/assets',
);

const FIXTURES = new Map([
  ['/flux@3x.png', path.join(FIXTURE_DIR, 'flux@3x.png')],
  ['/flowers.png', path.join(FIXTURE_DIR, 'flowers.png')],
]);

/**
 * @param {string | undefined} requestUrl
 * @returns {string | undefined}
 */
function resolveFixture(requestUrl) {
  // Examples append cache-busting query strings (?r=1&t=..., ?hash=...), which
  // must not change which fixture is served.
  const pathname = decodeURIComponent(
    // eslint-disable-next-line @microsoft/sdl/no-insecure-url
    new URL(requestUrl ?? '/', `http://127.0.0.1:${IMAGE_FIXTURE_PORT}`)
      .pathname,
  );
  return FIXTURES.get(pathname);
}

/**
 * Serves the Image example assets over loopback so the E2E tests that wait for
 * a download to complete do not depend on public network access.
 *
 * @returns {Promise<import('http').Server>}
 */
function startImageFixtureServer() {
  for (const fixture of FIXTURES.values()) {
    if (!fs.existsSync(fixture)) {
      throw new Error(`Image fixture is missing: ${fixture}`);
    }
  }

  const server = http.createServer((request, response) => {
    const fixture = resolveFixture(request.url);
    const isReadRequest = request.method === 'GET' || request.method === 'HEAD';

    if (fixture === undefined || !isReadRequest) {
      // Unknown paths have to 404 so the Image "Error Handler" example keeps
      // exercising its failure path.
      response.writeHead(404, {'Content-Type': 'text/plain'});
      response.end('Not Found');
      return;
    }

    const contents = fs.readFileSync(fixture);
    response.writeHead(200, {
      'Content-Type': 'image/png',
      'Content-Length': contents.length,
    });
    response.end(request.method === 'HEAD' ? undefined : contents);
  });

  return new Promise((resolve, reject) => {
    server.once('error', reject);
    server.listen(IMAGE_FIXTURE_PORT, '127.0.0.1', () => {
      server.removeListener('error', reject);
      resolve(server);
    });
  });
}

/**
 * @param {import('http').Server} server
 * @returns {Promise<void>}
 */
function stopImageFixtureServer(server) {
  return new Promise((resolve, reject) => {
    // The app keeps connections alive, which would otherwise hold the close
    // callback open until they time out.
    server.close(error => (error ? reject(error) : resolve()));
    server.closeAllConnections();
  });
}

module.exports = {
  startImageFixtureServer,
  stopImageFixtureServer,
};
