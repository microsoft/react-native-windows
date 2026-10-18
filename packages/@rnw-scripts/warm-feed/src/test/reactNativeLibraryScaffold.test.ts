/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {spawnSync} from 'node:child_process';
import {
  copyFileSync,
  existsSync,
  mkdirSync,
  mkdtempSync,
  readFileSync,
  rmSync,
  writeFileSync,
} from 'node:fs';
import {tmpdir} from 'node:os';
import {delimiter, join, resolve} from 'node:path';

const repoRoot = resolve(__dirname, '..', '..', '..', '..', '..');
const scriptsDir = join(repoRoot, 'vnext', 'Scripts');
const tempDirs: string[] = [];
const nightly = '0.88.0-nightly-20260805-f2188610c';
const windowsTest = process.platform === 'win32' ? test : test.skip;

jest.setTimeout(30000);

afterEach(() => {
  for (const dir of tempDirs.splice(0)) {
    rmSync(dir, {recursive: true, force: true});
  }
});

interface ToolCall {
  tool: string;
  args: string[];
  registry: string;
  userconfig: string;
}

function fixture(extraEnv: NodeJS.ProcessEnv = {}) {
  const root = mkdtempSync(join(tmpdir(), 'rnw scaffold test-'));
  tempDirs.push(root);
  const bin = join(root, 'bin');
  const checkout = join(root, 'RNW checkout');
  const scripts = join(checkout, 'vnext', 'Scripts');
  mkdirSync(bin);
  mkdirSync(scripts, {recursive: true});
  for (const file of [
    'GetReactNativeLibraryScaffoldVersion.js',
    'UpdateNightlyDependencies.ps1',
  ]) {
    copyFileSync(join(scriptsDir, file), join(scripts, file));
  }
  const callsPath = join(root, 'calls.jsonl');
  writeFileSync(
    join(bin, 'tool.js'),
    `
const fs = require('node:fs');
const path = require('node:path');
const [tool, ...args] = process.argv.slice(2);
fs.appendFileSync(process.env.RNW_TEST_CALLS, JSON.stringify({
  tool, args,
  registry: process.env.npm_config_registry,
  userconfig: process.env.npm_config_userconfig,
}) + '\\n');
if (tool === 'npm' && args[0] === 'view') {
  const spec = args[1];
  if (spec === process.env.RNW_TEST_FAIL_SPEC ||
      spec === '@react-native-community/template@1000.0.0') {
    console.error('simulated registry lookup failure for ' + spec);
    process.exit(1);
  }
  if (spec === process.env.RNW_TEST_RESPONSE_SPEC) {
    process.stdout.write(process.env.RNW_TEST_RESPONSE || '');
  } else {
    console.log(JSON.stringify(spec.endsWith('@latest')
      ? '1000.0.0' : spec.slice(spec.lastIndexOf('@') + 1)));
  }
} else if (tool === 'npm' && args[0] === 'show') {
  const field = args[2];
  console.log(field === 'dependencies.@react-native-community/cli' ? '20.0.0'
    : field === 'devDependencies.react' ? '19.2.3'
    : args[1].slice(args[1].lastIndexOf('@') + 1));
} else if (tool === 'npx') {
  const version = args[args.indexOf('--react-native-version') + 1];
  const project = path.join(process.cwd(), args[args.length - 1]);
  fs.mkdirSync(path.join(project, 'example'), {recursive: true});
  const manifest = {
    dependencies: {
      'react-native': version, '@react-native/new-app-screen': version,
      react: '19.2.3', other: '1.0.0',
    },
    devDependencies: {
      '@react-native/babel-preset': version, '@react-native/metro-config': version,
      '@react-native-community/cli': '19.0.0',
      '@react-native-community/cli-platform-android': '19.0.0',
      '@react-native-community/cli-platform-ios': '19.0.0',
    },
    peerDependencies: {'react-native': '*'},
    optionalDependencies: {'@react-native/test-config': version},
    resolutions: {'react-native': version, '@react-native-community/cli': '19.0.0'},
  };
  for (const file of ['package.json', path.join('example', 'package.json')]) {
    fs.writeFileSync(path.join(project, file), JSON.stringify(manifest));
  }
} else if (tool === 'yarn') {
  // Stop at the first install: no network, native build, or generated git commit.
  process.exit(73);
} else {
  console.error('Unexpected test command: ' + tool + ' ' + args.join(' '));
  process.exit(2);
}
`,
  );
  for (const tool of ['npm', 'npx', 'yarn']) {
    writeFileSync(
      join(bin, `${tool}.cmd`),
      `@echo off\r\n"${process.execPath}" "%~dp0tool.js" ${tool} %*\r\nexit /b %ERRORLEVEL%\r\n`,
    );
    writeFileSync(
      join(bin, tool),
      `#!/usr/bin/env node\nprocess.argv.splice(2, 0, '${tool}');\nrequire('./tool.js');\n`,
      {mode: 0o755},
    );
  }
  const env = {
    ...Object.fromEntries(
      Object.entries(process.env).filter(([key]) => !/^npm_config_/i.test(key)),
    ),
    npm_config_registry: 'https://feed.invalid/npm/',
    npm_config_userconfig: join(root, '.npmrc'),
    RNW_ROOT: checkout,
    RNW_TEST_CALLS: callsPath,
    ...extraEnv,
  };
  const pathKey =
    Object.keys(env).find(key => key.toLowerCase() === 'path') ?? 'PATH';
  const childEnv: NodeJS.ProcessEnv = env;
  childEnv[pathKey] = `${bin}${delimiter}${process.env[pathKey]}`;
  return {root, scripts, env: childEnv, callsPath};
}

function calls(f: ReturnType<typeof fixture>): ToolCall[] {
  if (!existsSync(f.callsPath)) return [];
  return readFileSync(f.callsPath, 'utf8')
    .trim()
    .split('\n')
    .map(line => JSON.parse(line) as ToolCall);
}

function runResolver(f: ReturnType<typeof fixture>, generator = '0.63.0') {
  return spawnSync(
    process.execPath,
    [join(f.scripts, 'GetReactNativeLibraryScaffoldVersion.js'), generator],
    {cwd: f.root, env: f.env, encoding: 'utf8'},
  );
}

function runCli(f: ReturnType<typeof fixture>, version: string) {
  return spawnSync(
    `"${join(scriptsDir, 'creaternwlib.cmd')}"`,
    ['/r', '19.2.3', '/rn', version, '/rnw', '0.0.0-canary.1061', 'testlib'],
    {cwd: f.root, env: f.env, encoding: 'utf8', shell: true},
  );
}

test('the shared resolver ignores latest=1000.0.0 and checks exact matching RN/template versions', () => {
  const f = fixture();
  const result = runResolver(f);
  expect(result.error).toBeUndefined();
  expect(result.status).toBe(0);
  expect(result.stdout.trim()).toBe('0.85.0');
  expect(calls(f).map(call => call.args)).toEqual([
    ['view', 'react-native@0.85.0', 'version', '--json'],
    ['view', '@react-native-community/template@0.85.0', 'version', '--json'],
  ]);
  expect(
    calls(f).every(call => call.registry === f.env.npm_config_registry),
  ).toBe(true);
  expect(
    calls(f).every(call => call.userconfig === f.env.npm_config_userconfig),
  ).toBe(true);
});

test('the fixture isolates inherited npm configuration regardless of key casing', () => {
  const previousEnv = process.env;
  try {
    process.env = {
      ...previousEnv,
      NPM_CONFIG_REGISTRY: 'https://inherited.invalid/npm/',
      NPM_CONFIG_USERCONFIG: join(tmpdir(), 'inherited.npmrc'),
    };
    const f = fixture();
    expect(
      Object.keys(f.env)
        .filter(key => /^npm_config_/i.test(key))
        .sort(),
    ).toEqual(['npm_config_registry', 'npm_config_userconfig']);
    const result = runResolver(f);
    expect(result.error).toBeUndefined();
    expect(result.status).toBe(0);
    expect(result.stdout.trim()).toBe('0.85.0');
    expect(calls(f).map(call => [call.registry, call.userconfig])).toEqual([
      [f.env.npm_config_registry, f.env.npm_config_userconfig],
      [f.env.npm_config_registry, f.env.npm_config_userconfig],
    ]);
  } finally {
    process.env = previousEnv;
  }
});

test.each(['latest', '0.64.0'])(
  'an unreviewed generator version (%s) cannot silently reuse the bootstrap pin',
  generator => {
    const f = fixture();
    const result = runResolver(f, generator);
    expect(result.status).toBe(1);
    expect(result.stdout).toBe('');
    expect(result.stderr).toContain('No nightly scaffold is configured');
    expect(calls(f)).toEqual([]);
  },
);

test.each(['react-native@0.85.0', '@react-native-community/template@0.85.0'])(
  'a failed %s lookup is explicit and produces no scaffold version',
  spec => {
    const f = fixture({RNW_TEST_FAIL_SPEC: spec});
    const result = runResolver(f);
    expect(result.status).toBe(1);
    expect(result.stdout).toBe('');
    expect(result.stderr).toContain(`Unable to resolve ${spec}`);
  },
);

test.each(['"latest"', '"1000.0.0"', '[]', 'not-json', ''])(
  'a nonmatching or invalid registry response (%s) is not a successful selection',
  response => {
    const f = fixture({
      RNW_TEST_RESPONSE_SPEC: '@react-native-community/template@0.85.0',
      RNW_TEST_RESPONSE: response,
    });
    const result = runResolver(f);
    expect(result.status).toBe(1);
    expect(result.stdout).toBe('');
    expect(result.stderr).toContain('@react-native-community/template@0.85.0');
  },
);

windowsTest(
  'creaternwlib passes the shared concrete version to npx and rewrites both manifests before install',
  () => {
    const f = fixture();
    const result = runCli(f, nightly);
    expect(result.error).toBeUndefined();
    expect(result.status).toBe(73);
    const command = calls(f).find(call => call.tool === 'npx');
    expect(command).toBeDefined();
    expect(
      command?.args[command.args.indexOf('--react-native-version') + 1],
    ).toBe(runResolver(f).stdout.trim());
    expect(command?.args).toEqual(
      expect.arrayContaining([
        'create-react-native-library@0.63.0',
        '--languages',
        'kotlin-objc',
        '--type',
        'turbo-module',
        '--tools=eslint',
        '--tools=jest',
        '--tools=lefthook',
        '--tools=release-it',
        '--tools=vite',
        '--example',
        'vanilla',
      ]),
    );
    for (const file of ['package.json', join('example', 'package.json')]) {
      const manifest = JSON.parse(
        readFileSync(join(f.root, 'testlib', file), 'utf8'),
      );
      expect(manifest.dependencies).toEqual({
        'react-native': nightly,
        '@react-native/new-app-screen': nightly,
        react: '19.2.3',
        other: '1.0.0',
      });
      expect(manifest.devDependencies).toEqual({
        '@react-native/babel-preset': nightly,
        '@react-native/metro-config': nightly,
        '@react-native-community/cli': '20.0.0',
        '@react-native-community/cli-platform-android': '20.0.0',
        '@react-native-community/cli-platform-ios': '20.0.0',
      });
      expect(manifest.peerDependencies['react-native']).toBe('*');
      expect(manifest.optionalDependencies['@react-native/test-config']).toBe(
        nightly,
      );
      expect(manifest.resolutions).toEqual({
        'react-native': nightly,
        '@react-native-community/cli': '20.0.0',
      });
    }
  },
);

windowsTest.each(['0.81.4', '0.85.3'])(
  'creaternwlib keeps stable RN %s without a bootstrap lookup or nightly rewrite',
  version => {
    const f = fixture();
    const result = runCli(f, version);
    expect(result.status).toBe(73);
    const command = calls(f).find(call => call.tool === 'npx');
    expect(
      command?.args[command.args.indexOf('--react-native-version') + 1],
    ).toBe(version);
    expect(calls(f).filter(call => call.args[0] === 'view')).toEqual([]);
    for (const file of ['package.json', join('example', 'package.json')]) {
      const manifest = JSON.parse(
        readFileSync(join(f.root, 'testlib', file), 'utf8'),
      );
      expect(manifest.dependencies['react-native']).toBe(version);
      expect(manifest.devDependencies['@react-native-community/cli']).toBe(
        '19.0.0',
      );
    }
  },
);

windowsTest(
  'creaternwlib fails before generation when the matching template lookup fails',
  () => {
    const f = fixture({
      RNW_TEST_FAIL_SPEC: '@react-native-community/template@0.85.0',
    });
    const result = runCli(f, nightly);
    expect(result.status).toBe(1);
    expect(result.stdout).toContain('Unable to determine a compatible');
    expect(result.stderr).toContain('@react-native-community/template@0.85.0');
    expect(
      calls(f).some(call => call.tool === 'npx' || call.tool === 'yarn'),
    ).toBe(false);
  },
);
