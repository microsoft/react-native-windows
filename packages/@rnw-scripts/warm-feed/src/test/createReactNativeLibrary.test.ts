/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {
  existsSync,
  mkdirSync,
  mkdtempSync,
  rmSync,
  writeFileSync,
} from 'node:fs';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import * as closure from '../closure';
import {
  createReactNativeLibraryModule,
  nightlyFixupSpecs,
  parseCrnlConfig,
  readRnwWorkspaceSpecs,
  resolveBranchVersions,
  scaffoldReactNativeVersion,
  stableMinor,
} from '../specialModules/createReactNativeLibrary';
import type {SpecialModuleContext} from '../specialModules/types';
import type {NpmRegistry} from '../registries';

const tempDirs: string[] = [];

function tempRepo(): string {
  const dir = mkdtempSync(join(tmpdir(), 'warm-repo-'));
  tempDirs.push(dir);
  return dir;
}

afterEach(() => {
  jest.restoreAllMocks();
  for (const dir of tempDirs.splice(0)) {
    rmSync(dir, {recursive: true, force: true});
  }
});

function fakeRegistry(versions: Record<string, string[]>): NpmRegistry {
  return {
    getVersions: async (id: string) => versions[id] ?? [],
    getTarballUrl: async () => null,
  };
}

function mctx(repoRoot: string, registry: NpmRegistry): SpecialModuleContext {
  const log = {info() {}, warn() {}, error() {}, debug() {}};
  return {
    ctx: {log} as unknown as SpecialModuleContext['ctx'],
    repoRoot,
    registry,
    npmRegistryUrl: 'https://feed/npm/',
    npmrcPath: '',
  };
}

test('stableMinor parses stable branch names, null for main', () => {
  expect(stableMinor('0.81-stable')).toBe(81);
  expect(stableMinor('0.85-stable')).toBe(85);
  expect(stableMinor('0.85')).toBe(85);
  expect(stableMinor('main')).toBeNull();
});

test('parseCrnlConfig validates required fields and applies defaults', () => {
  expect(() => parseCrnlConfig({})).toThrow(/createLibraryVersion/);
  expect(() => parseCrnlConfig({createLibraryVersion: '0.63.0'})).toThrow(
    /branches/,
  );
  const cfg = parseCrnlConfig({
    createLibraryVersion: '0.63.0',
    branches: [{name: 'main', nightly: true}, {name: '0.85-stable'}],
  });
  expect(cfg.type).toBe('turbo-module');
  expect(cfg.example).toBe('vanilla');
  expect(cfg.languages).toBe('kotlin-objc');
  expect(cfg.tools).toEqual([
    'eslint',
    'jest',
    'lefthook',
    'release-it',
    'vite',
  ]);
  expect(cfg.branches).toHaveLength(2);
  expect(cfg.branches[0]).toMatchObject({name: 'main', nightly: true});
});

test('parseCrnlConfig accepts a custom tools list', () => {
  const cfg = parseCrnlConfig({
    createLibraryVersion: '0.63.0',
    tools: ['eslint', 'jest'],
    branches: [{name: 'main'}],
  });
  expect(cfg.tools).toEqual(['eslint', 'jest']);
});

test('nightlyFixupSpecs rewrites the RN family and cli specs only', () => {
  const specs: Record<string, string> = {
    'react-native': '0.80.0',
    'react-native-windows': '*',
    '@react-native/new-app-screen': '0.80.0',
    '@react-native/babel-preset': '0.80.0',
    '@react-native/metro-config': '0.80.0',
    '@react-native-community/cli': '20.0.0',
    '@react-native-community/cli-platform-ios': '20.0.0',
    react: '19.0.0',
    other: '1.0.0',
  };
  nightlyFixupSpecs(specs, '0.86.0-nightly', '21.0.0');
  expect(specs['react-native']).toBe('0.86.0-nightly');
  expect(specs['@react-native/new-app-screen']).toBe('0.86.0-nightly');
  expect(specs['@react-native/babel-preset']).toBe('0.86.0-nightly');
  expect(specs['@react-native/metro-config']).toBe('0.86.0-nightly');
  expect(specs['@react-native-community/cli']).toBe('21.0.0');
  expect(specs['@react-native-community/cli-platform-ios']).toBe('21.0.0');
  // '*' peer and unrelated packages are untouched.
  expect(specs['react-native-windows']).toBe('*');
  expect(specs.react).toBe('19.0.0');
  expect(specs.other).toBe('1.0.0');
});

test('scaffoldReactNativeVersion shares the CLI resolver and its registry configuration', () => {
  const context = mctx('C:\\RNW checkout', fakeRegistry({}));
  context.npmrcPath = 'C:\\feed auth\\.npmrc';
  const run = jest.spyOn(closure, 'runTool').mockReturnValue('0.85.0\r\n');
  expect(
    scaffoldReactNativeVersion(
      context,
      {
        reactNative: '0.88.0-nightly-20260805-f2188610c',
        reactNativeCli: '20.0.0',
        nightly: true,
      },
      '0.63.0',
    ),
  ).toBe('0.85.0');
  expect(run).toHaveBeenCalledWith(
    'node',
    [
      `"${join(context.repoRoot, 'vnext', 'Scripts', 'GetReactNativeLibraryScaffoldVersion.js')}"`,
      '0.63.0',
    ],
    context.repoRoot,
    {
      npm_config_userconfig: context.npmrcPath,
      npm_config_registry: context.npmRegistryUrl,
    },
    'resolve cRNL 0.63.0 nightly scaffold',
  );
});

test('scaffoldReactNativeVersion passes a stable version through without a bootstrap lookup', () => {
  const run = jest.spyOn(closure, 'runTool');
  expect(
    scaffoldReactNativeVersion(
      mctx('/no/repo', fakeRegistry({})),
      {
        reactNative: '0.85.3',
        reactNativeCli: '20.0.0',
        reactNativeWindowsSpec: '0.85.3',
        nightly: false,
      },
      '0.63.0',
    ),
  ).toBe('0.85.3');
  expect(run).not.toHaveBeenCalled();
});

test('resolveBranchVersions: main reads the nightly from vnext, no rnw spec', async () => {
  const repo = tempRepo();
  mkdirSync(join(repo, 'vnext'));
  writeFileSync(
    join(repo, 'vnext', 'package.json'),
    JSON.stringify({
      devDependencies: {'react-native': '0.86.0-nightly-x'},
      dependencies: {'@react-native-community/cli': '20.0.0'},
    }),
  );
  const v = await resolveBranchVersions(mctx(repo, fakeRegistry({})), {
    name: 'main',
    nightly: true,
  });
  expect(v.reactNative).toBe('0.86.0-nightly-x');
  expect(v.reactNativeCli).toBe('20.0.0');
  expect(v.reactNativeWindowsSpec).toBeUndefined();
  expect(v.nightly).toBe(true);
});

test('resolveBranchVersions: main derives the CLI even when reactNativeVersion is overridden', async () => {
  const repo = tempRepo();
  mkdirSync(join(repo, 'vnext'));
  writeFileSync(
    join(repo, 'vnext', 'package.json'),
    JSON.stringify({
      devDependencies: {'react-native': '0.86.0-nightly-x'},
      dependencies: {'@react-native-community/cli': '20.0.0'},
    }),
  );
  const v = await resolveBranchVersions(mctx(repo, fakeRegistry({})), {
    name: 'main',
    nightly: true,
    reactNativeVersion: '0.86.0-nightly-pinned',
  });
  expect(v.reactNative).toBe('0.86.0-nightly-pinned');
  expect(v.reactNativeCli).toBe('20.0.0');
});

test('resolveBranchVersions: a stable branch reads RN/CLI from its vnext, RNW from the line', async () => {
  const reg = fakeRegistry({
    'react-native-windows': ['0.81.0', '0.81.4', '0.81.5-preview.1'],
  });
  const v = await resolveBranchVersions(
    mctx('/no/repo', reg),
    {name: '0.81-stable'},
    () => ({
      devDependencies: {'react-native': '0.81.4'},
      dependencies: {'@react-native-community/cli': '18.0.0'},
    }),
  );
  expect(v.reactNative).toBe('0.81.4');
  expect(v.reactNativeCli).toBe('18.0.0');
  expect(v.reactNativeWindowsSpec).toBe('0.81.4');
  expect(v.nightly).toBe(false);
});

test('resolveBranchVersions: a preview-only RNW line falls back to the newest preview', async () => {
  const reg = fakeRegistry({
    'react-native-windows': ['0.85.0-preview.1', '0.85.0-preview.2'],
  });
  const v = await resolveBranchVersions(
    mctx('/no/repo', reg),
    {name: '0.85-stable'},
    () => ({devDependencies: {'react-native': '0.85.3'}}),
  );
  expect(v.reactNative).toBe('0.85.3');
  expect(v.reactNativeWindowsSpec).toBe('0.85.0-preview.2');
});

test('resolveBranchVersions: explicit overrides win without reading the branch vnext', async () => {
  const v = await resolveBranchVersions(
    mctx('/no/repo', fakeRegistry({})),
    {
      name: '0.83-stable',
      reactNativeVersion: '0.83.7',
      reactNativeWindowsSpec: '0.83.0-canary',
    },
    () => {
      throw new Error('should not read branch vnext when overridden');
    },
  );
  expect(v.reactNative).toBe('0.83.7');
  expect(v.reactNativeWindowsSpec).toBe('0.83.0-canary');
});

test('resolveBranchVersions: throws when the branch vnext pins no react-native', async () => {
  await expect(
    resolveBranchVersions(
      mctx('/no/repo', fakeRegistry({})),
      {name: '0.99-stable'},
      () => ({}),
    ),
  ).rejects.toThrow(/could not determine a react-native version/);
});

test('readRnwWorkspaceSpecs warms only feed-published @react-native-windows/* pins', async () => {
  const repo = tempRepo();
  mkdirSync(join(repo, 'vnext'));
  writeFileSync(
    join(repo, 'vnext', 'package.json'),
    JSON.stringify({
      dependencies: {
        '@react-native-windows/cli': '0.0.0-canary.293',
        '@react-native-community/cli': '20.0.0',
        'react-native': '0.86.0-nightly-x',
      },
      devDependencies: {
        '@react-native-windows/codegen': '0.0.0-canary.133',
        '@react-native-windows/find-dotnet-tools': '0.0.0-canary.2',
        '@rnw-scripts/just-task': 'workspace:*',
      },
    }),
  );
  const reg = fakeRegistry({
    // cli + codegen pins are on the feed; find-dotnet-tools' pin isn't published yet.
    '@react-native-windows/cli': ['0.0.0-canary.290', '0.0.0-canary.293'],
    '@react-native-windows/codegen': ['0.0.0-canary.133'],
    '@react-native-windows/find-dotnet-tools': ['0.0.0-canary.1'],
  });
  expect(await readRnwWorkspaceSpecs(mctx(repo, reg))).toEqual({
    '@react-native-windows/cli': '0.0.0-canary.293',
    '@react-native-windows/codegen': '0.0.0-canary.133',
  });
});

function generatedScaffold(workDir: string, reactNativeVersion: string): void {
  const projectDir = join(workDir, 'warmlib');
  mkdirSync(join(projectDir, 'example'), {recursive: true});
  writeFileSync(
    join(projectDir, 'package.json'),
    JSON.stringify({
      peerDependencies: {'react-native': '*'},
      devDependencies: {
        'react-native': reactNativeVersion,
        '@react-native/babel-preset': reactNativeVersion,
        '@react-native-community/cli': '20.0.0',
        react: '19.2.0',
        'shared-tool': '1.0.0',
      },
    }),
  );
  writeFileSync(
    join(projectDir, 'example', 'package.json'),
    JSON.stringify({
      dependencies: {
        'react-native': reactNativeVersion,
        '@react-native/new-app-screen': reactNativeVersion,
        'shared-tool': '2.0.0',
      },
      devDependencies: {
        '@react-native/metro-config': reactNativeVersion,
        '@react-native-community/cli-platform-android': '20.0.0',
        '@react-native-community/cli-platform-ios': '20.0.0',
      },
    }),
  );
}

const scaffoldConfig = {
  createLibraryVersion: '0.63.0',
  branches: [
    {
      name: 'main',
      nightly: true,
      reactNativeVersion: '0.88.0-nightly-20260805-f2188610c',
      reactNativeCliVersion: '21.0.0',
    },
    {
      name: '0.85-stable',
      reactNativeVersion: '0.85.3',
      reactNativeCliVersion: '20.0.0',
      reactNativeWindowsSpec: '0.85.3',
    },
  ],
};

test('the actual warmer generator commands use concrete versions and preserve both rewritten closures', async () => {
  const repo = tempRepo();
  mkdirSync(join(repo, 'vnext'));
  writeFileSync(join(repo, 'vnext', 'package.json'), '{}');
  const workDirs: string[] = [];
  const run = jest
    .spyOn(closure, 'runTool')
    .mockImplementation((cmd, args, cwd) => {
      if (cmd === 'node') return '0.85.0\n';
      expect(cmd).toBe('npx');
      workDirs.push(cwd);
      generatedScaffold(cwd, args[args.indexOf('--react-native-version') + 1]);
      return '';
    });

  const result = await createReactNativeLibraryModule.collectDepSpecs(
    mctx(repo, fakeRegistry({})),
    scaffoldConfig,
  );
  const commands = run.mock.calls.filter(([cmd]) => cmd === 'npx');
  expect(commands).toHaveLength(2);
  for (const [index, version] of ['0.85.0', '0.85.3'].entries()) {
    const [, args, , env] = commands[index];
    expect(args).toEqual(
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
    expect(args[args.indexOf('--react-native-version') + 1]).toBe(version);
    expect(env.npm_config_registry).toBe('https://feed/npm/');
  }
  expect(result.failures).toEqual([]);
  expect(result.sets.map(set => set.label)).toEqual([
    'crnl:main:lib',
    'crnl:main:example',
    'crnl:0.85-stable:lib',
    'crnl:0.85-stable:example',
  ]);
  expect(result.sets[0].specs).toEqual({
    'react-native': scaffoldConfig.branches[0].reactNativeVersion,
    '@react-native/babel-preset': scaffoldConfig.branches[0].reactNativeVersion,
    '@react-native-community/cli': '21.0.0',
    react: '19.2.0',
    'shared-tool': '1.0.0',
  });
  expect(result.sets[1].specs).toEqual({
    'react-native': scaffoldConfig.branches[0].reactNativeVersion,
    '@react-native/new-app-screen':
      scaffoldConfig.branches[0].reactNativeVersion,
    '@react-native/metro-config': scaffoldConfig.branches[0].reactNativeVersion,
    '@react-native-community/cli-platform-android': '21.0.0',
    '@react-native-community/cli-platform-ios': '21.0.0',
    'shared-tool': '2.0.0',
  });
  expect(result.sets[2].specs['react-native']).toBe('0.85.3');
  expect(result.sets[2].specs['react-native-windows']).toBe('0.85.3');
  expect(result.sets[3].specs['react-native']).toBe('0.85.3');
  expect(workDirs.every(dir => !existsSync(dir))).toBe(true);
});

test.each(['node', 'npx'])(
  'a %s failure records the failed branch but still collects the healthy stable closures',
  async failingCommand => {
    const context = mctx('/no/repo', fakeRegistry({}));
    const logError = jest.spyOn(context.ctx.log, 'error');
    const workDirs: string[] = [];
    let failed = false;
    jest.spyOn(closure, 'runTool').mockImplementation((cmd, args, cwd) => {
      if (cmd === 'npx') workDirs.push(cwd);
      if (cmd === failingCommand && !failed) {
        failed = true;
        throw new Error('scaffold lookup or generation failed');
      }
      if (cmd === 'node') return '0.85.0\n';
      generatedScaffold(cwd, args[args.indexOf('--react-native-version') + 1]);
      return '';
    });

    const result = await createReactNativeLibraryModule.collectDepSpecs(
      context,
      scaffoldConfig,
    );
    expect(result.failures).toEqual(['main']);
    expect(result.sets.map(set => set.label)).toEqual([
      'crnl:0.85-stable:lib',
      'crnl:0.85-stable:example',
    ]);
    expect(logError).toHaveBeenCalledWith(
      'cRNL: branch main failed: scaffold lookup or generation failed',
    );
    expect(workDirs.every(dir => !existsSync(dir))).toBe(true);
  },
);
