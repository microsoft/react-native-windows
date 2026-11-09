// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

const assert = require('node:assert/strict');
const {execFile, execFileSync} = require('node:child_process');
const {once} = require('node:events');
const {promises: fs} = require('node:fs');
const http = require('node:http');
const os = require('node:os');
const path = require('node:path');
const {test} = require('node:test');
const {promisify} = require('node:util');

const run = promisify(execFile);
const repoRoot = path.resolve(__dirname, '..', '..');

for (const name of ['@rnw-tests/registry-fixture', 'rnw-registry-fixture']) {
  test(`Windows CLI-init prunes published ${name} versions using the publish registry, not the default proxy`, async t => {
    if (process.platform !== 'win32') {
      t.skip('CLI-init uses the Windows Node/npm installation layout');
      return;
    }

    const template = await fs.readFile(
      path.join(repoRoot, '.ado', 'templates', 'verdaccio-start.yml'),
      'utf8',
    );
    const command = template.match(
      /^\s*- script: node (\.ado\/scripts\/npmPack\.js[^\r\n]*--check-npm[^\r\n]*)$/m,
    );
    assert.ok(command, 'CLI-init must check already published packages');
    const [script, ...args] = command[1].split(/\s+/);
    const publishRegistry = template.match(
      /npm publish "[^"]+" --registry (\S+)/,
    )?.[1];
    assert.ok(publishRegistry, 'CLI-init must specify its publish registry');

    const root = await fs.mkdtemp(
      path.join(os.tmpdir(), 'rnw-verdaccio-pruning-'),
    );
    t.after(() => fs.rm(root, {recursive: true, force: true}));
    const packages = path.join(root, 'packages');
    const userConfig = path.join(root, '.npmrc');
    const globalConfig = path.join(root, '.global-npmrc');
    await Promise.all([
      fs.mkdir(packages),
      fs.mkdir(path.join(root, 'package')),
      fs.writeFile(userConfig, ''),
      fs.writeFile(globalConfig, ''),
    ]);
    for (const version of ['1.0.0', '1.0.1']) {
      await fs.writeFile(
        path.join(root, 'package', 'package.json'),
        JSON.stringify({name, version}),
      );
      execFileSync('tar', [
        '-czf',
        path.join(packages, `${version}.tgz`),
        '-C',
        root,
        'package',
      ]);
    }

    const requests = [];
    const server = http.createServer((request, response) => {
      const pathname = decodeURIComponent(
        new URL(request.url, 'http://localhost').pathname,
      );
      requests.push(pathname);
      const isPublishRegistry = pathname === `/verdaccio/${name}`;
      response.writeHead(isPublishRegistry ? 200 : 404, {
        'Content-Type': 'application/json',
      });
      response.end(
        JSON.stringify(
          isPublishRegistry
            ? {
                name,
                'dist-tags': {latest: '1.0.0'},
                versions: {'1.0.0': {name, version: '1.0.0'}},
              }
            : {error: 'Not found'},
        ),
      );
    });
    server.listen(0, '127.0.0.1');
    await once(server, 'listening');
    t.after(() => promisify(server.close).call(server));
    const address = server.address();
    assert.ok(address && typeof address !== 'string');
    const baseUrl = `http://127.0.0.1:${address.port}`;
    const registryIndex = args.indexOf('--registry');
    if (registryIndex !== -1) {
      assert.equal(args[registryIndex + 1], publishRegistry);
      args[registryIndex + 1] = `${baseUrl}/verdaccio/`;
    }

    await run(process.execPath, [path.join(repoRoot, script), ...args, packages], {
      cwd: root,
      timeout: 60000,
      env: {
        ...process.env,
        NPM_CONFIG_REGISTRY: `${baseUrl}/proxy/`,
        NPM_CONFIG_USERCONFIG: userConfig,
        NPM_CONFIG_GLOBALCONFIG: globalConfig,
        NPM_CONFIG_CACHE: path.join(root, 'cache'),
        NPM_CONFIG_FETCH_RETRIES: '0',
        NPM_CONFIG_FETCH_TIMEOUT: '2000',
      },
    });

    assert.deepEqual(await fs.readdir(packages), ['1.0.1.tgz']);
    assert.ok(requests.includes(`/verdaccio/${name}`));
    assert.ok(!requests.some(request => request.startsWith('/proxy/')));
  });
}
