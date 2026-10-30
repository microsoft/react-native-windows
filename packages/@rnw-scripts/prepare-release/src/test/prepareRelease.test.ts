/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {afterEach, beforeEach, it, mock} from 'node:test';
import {isDeepStrictEqual} from 'node:util';
import path from 'path';
import {runInNewContext} from 'vm';
import fs from '@react-native-windows/fs';
import * as repoRoot from '@react-native-windows/find-repo-root';
import {GitRepo} from '../git';
import * as github from '../github';
import * as beachball from '../beachballBump';
import * as summary from '../releaseSummary';
import {formatReleasePRTitle, main} from '../prepareRelease';

const pr = {number: 42, url: 'https://github.com/owner/repo/pull/42'};
const commitMessage =
  'RELEASE: Releasing 1 package(s) (0.85-stable)\n\nRNW-Release-Branch: 0.85-stable';
const sentinel = new Error('process.exit sentinel');
const argv = process.argv;
function setupMocks() {
  const git = GitRepo.prototype;
  return {
    exit: mock.method(process, 'exit', () => {
      throw sentinel;
    }),
    log: mock.method(console, 'log', () => {}),
    error: mock.method(console, 'error', () => {}),
    root: mock.method(repoRoot, 'default', async () => 'repo root'),
    read: mock.method(fs, 'readFileSync', () =>
      JSON.stringify({repository: {url: 'https://github.com/owner/repo'}}),
    ),
    git: {
      remoteList: mock.method(
        git,
        'remoteList',
        async () => 'upstream\thttps://github.com/owner/repo (fetch)',
      ),
      currentBranch: mock.method(git, 'currentBranch', async () => 'main'),
      statusPorcelain: mock.method(
        git,
        'statusPorcelain',
        async () => ' M packages/example/package.json',
      ),
      lsRemote: mock.method(git, 'lsRemote', async () => 'sha'),
      fetch: mock.method(git, 'fetch', async () => {}),
      checkout: mock.method(git, 'checkout', async () => {}),
      checkoutNewBranch: mock.method(git, 'checkoutNewBranch', async () => {}),
      stageAll: mock.method(git, 'stageAll', async () => {}),
      commit: mock.method(git, 'commit', async () => {}),
      push: mock.method(git, 'push', async () => {}),
    },
    hasChangeFiles: mock.method(beachball, 'hasChangeFiles', () => true),
    bumpVersions: mock.method(beachball, 'bumpVersions', async () => {}),
    updateLockfile: mock.method(beachball, 'updateLockfile', async () => {}),
    findPR: mock.method(github, 'findPR', async () => null),
    createPR: mock.method(github, 'createPR', async () => pr),
    updatePR: mock.method(github, 'updatePR', async () => {}),
    collect: mock.method(summary, 'collectBumpedPackages', () => [
      {name: 'example', version: '1.0.0', comments: []},
    ]),
    body: mock.method(summary, 'generatePRBody', () => 'PR body'),
  };
}
let mocks: ReturnType<typeof setupMocks>;
type Calls = {mock: {calls: {arguments: unknown[]}[]}};
const calls = (fn: Calls) =>
  fn.mock.calls.map(call => Array.from(call.arguments));
function calledWith(fn: Calls, ...args: unknown[]) {
  assert.ok(calls(fn).some(actual => isDeepStrictEqual(actual, args)));
}
beforeEach(() => {
  process.argv = [
    'node',
    'prepare-release',
    '--branch',
    '0.85-stable',
    '--no-color',
  ];
  mocks = setupMocks();
});
afterEach(() => {
  process.argv = argv;
  mock.restoreAll();
});

// Alias edge cases prove only one trailing suffix is removed; counts cover grammar.
for (const [branch, count, alias] of [
  ['main', 2, 'main'],
  ['0.81-stable', 1, '0.81'],
  ['0.85-stable', 2, '0.85'],
  ['feature', 0, 'feature'],
  ['a-stable-next', 2, 'a-stable-next'],
  ['a-stable-stable', 1, 'a-stable'],
  ['分支🚀', 1, '分支🚀'],
] as const) {
  void it(`formats ${branch} with ${count} packages`, () => {
    assert.equal(
      formatReleasePRTitle(branch, count),
      `🚀 [${alias}]: Release ${count} ${count === 1 ? 'package' : 'packages'}`,
    );
  });
}

// New/existing PRs must share the title in live and dry-run paths.
for (const existing of [false, true]) {
  void it(`creates or refreshes with full target branch metadata (existing=${existing})`, async () => {
    const {git, findPR, createPR, updatePR, exit} = mocks;
    findPR.mock.mockImplementation(async () => (existing ? pr : null));
    await main();
    calledWith(git.commit, commitMessage);
    assert.equal(git.commit.mock.callCount(), 1);
    calledWith(git.push, 'upstream', 'prepare-release/0.85-stable', {
      force: true,
    });
    const common = {
      title: '🚀 [0.85]: Release 1 package',
      body: 'PR body',
      cwd: 'repo root',
      repo: 'owner/repo',
    };
    calledWith(
      existing ? updatePR : createPR,
      existing
        ? {...common, number: 42}
        : {...common, head: 'prepare-release/0.85-stable', base: '0.85-stable'},
    );
    assert.equal((existing ? createPR : updatePR).mock.callCount(), 0);
    calledWith(git.checkout, 'main');
    assert.equal(exit.mock.callCount(), 0);
  });
  void it(`previews title and additive label without remote writes (existing=${existing})`, async () => {
    const {git, findPR, log, bumpVersions, createPR, updatePR} = mocks;
    process.argv.push('--dry-run');
    findPR.mock.mockImplementation(async () => (existing ? pr : null));
    await main();
    const output = calls(log).flat().join('\n');
    assert.ok(output.includes('🚀 [0.85]: Release 1 package";'));
    assert.ok(output.includes('label: release'));
    assert.equal(output.split(commitMessage).length - 1, 1);
    calledWith(log, 'PR body');
    assert.equal(bumpVersions.mock.callCount(), 1);
    calledWith(git.commit, commitMessage);
    assert.equal(git.commit.mock.callCount(), 1);
    for (const fn of [git.push, createPR, updatePR]) {
      assert.equal(fn.mock.callCount(), 0);
    }
  });
}
// Bump-only wins even when dry-run is also passed.
for (const dry of [false, true]) {
  void it(`bumps only without PR lookup or preview (dry=${dry})`, async () => {
    process.argv.push('--bump-only', ...(dry ? ['--dry-run'] : []));
    await assert.rejects(main(), err => err === sentinel);
    assert.deepEqual(calls(mocks.exit)[0], [0]);
    assert.equal(mocks.bumpVersions.mock.callCount(), 1);
    assert.equal(
      mocks.bumpVersions.mock.calls[0].arguments[0]!.targetBranch,
      '0.85-stable',
    );
    for (const fn of [
      mocks.findPR,
      mocks.git.push,
      mocks.git.commit,
      mocks.createPR,
      mocks.updatePR,
      mocks.git.checkoutNewBranch,
      mocks.updateLockfile,
    ]) {
      assert.equal(fn.mock.callCount(), 0);
    }
    assert.doesNotMatch(calls(mocks.log).flat().join('\n'), /🚀|label:|Would/);
  });
}

// Model the two gh failure boundaries: no PR on create; same PR after partial edit.
for (const existing of [false, true]) {
  void it(`fails visibly, restores, and retries without duplicate PRs (existing=${existing})`, async () => {
    const {git, findPR, createPR, updatePR, exit, error} = mocks;
    findPR.mock.mockImplementation(async () => (existing ? pr : null));
    (existing ? updatePR : createPR).mock.mockImplementationOnce(async () => {
      throw new Error('label release not found');
    });
    await assert.rejects(main(), err => err === sentinel);
    assert.deepEqual(calls(exit)[0], [1]);
    calledWith(error, 'Error: label release not found');
    assert.equal(git.push.mock.callCount(), 1);
    calledWith(git.checkout, 'main');
    await main();
    assert.equal(findPR.mock.callCount(), 2);
    assert.equal((existing ? updatePR : createPR).mock.callCount(), 2);
    assert.equal((existing ? createPR : updatePR).mock.callCount(), 0);
    if (existing) {
      assert.deepEqual(
        updatePR.mock.calls.map(({arguments: [opts]}) => opts!.number),
        [42, 42],
      );
    }
  });
}
void it('keeps the no-changes exit successful', async () => {
  mocks.git.statusPorcelain.mock.mockImplementation(async () => '');
  await assert.rejects(main(), err => err === sentinel);
  assert.deepEqual(calls(mocks.exit)[0], [0]);
  assert.equal(mocks.git.commit.mock.callCount(), 0);
  // Sentinel unwinds finally/catch; a real process.exit skips restoration.
});
void it('is inert on import', () => {
  const modulePath = require.resolve('../prepareRelease');
  const cached = require.cache[modulePath];
  try {
    delete require.cache[modulePath];
    require(modulePath);
  } finally {
    require.cache[modulePath] = cached;
  }
  assert.equal(mocks.root.mock.callCount(), 0);
  assert.equal(mocks.log.mock.callCount(), 0);
  assert.equal(mocks.exit.mock.callCount(), 0);
});
void it('rejects argument parsing before the internal catch', async () => {
  process.argv.push('--unknown-option');
  await assert.rejects(main(), /Unknown option/);
  assert.equal(mocks.error.mock.callCount(), 0);
  assert.equal(mocks.exit.mock.callCount(), 0);
});
void it('bin installs source maps, invokes main, and reports rejection with exit 1', async () => {
  const failure = new Error('main rejected');
  const install = mock.fn();
  let completion: Promise<void> | undefined;
  const invoke = mock.fn(() => ({
    catch: (handler: (err: Error) => void) => {
      completion = Promise.reject(failure).catch(handler);
    },
  }));
  const requireMock = mock.fn((name: string) =>
    name === 'source-map-support' ? {install} : {main: invoke},
  );
  runInNewContext(readFileSync(path.join(__dirname, '../../bin.js'), 'utf8'), {
    require: requireMock,
    console,
    process,
  });
  assert.ok(completion);
  await assert.rejects(completion, err => err === sentinel);
  assert.deepEqual(calls(requireMock), [
    ['source-map-support'],
    ['./lib-commonjs/prepareRelease'],
  ]);
  assert.equal(install.mock.callCount(), 1);
  assert.equal(invoke.mock.callCount(), 1);
  calledWith(mocks.error, failure);
  assert.deepEqual(calls(mocks.exit)[0], [1]);
});
