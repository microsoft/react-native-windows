/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import child_process from 'child_process';
import {GitPluginError, simpleGit} from 'simple-git';

beforeEach(() => {
  // Never execute these arguments, even if a dependency update regresses a guard.
  jest.spyOn(child_process, 'spawn').mockImplementation(() => {
    throw new Error('Git spawning disabled for guard test');
  });
});

afterEach(() => {
  jest.restoreAllMocks();
});

test.each(['--receive-p', '--exe'])(
  'disables native Git option abbreviations for %s',
  async option => {
    await expect(
      simpleGit().raw(['push', `${option}=rnw-test-command`]),
    ).rejects.toThrow('Git spawning disabled for guard test');
    // simple-git disables abbreviations in Git rather than expanding them itself.
    expect(child_process.spawn).toHaveBeenCalledTimes(1);
    expect(child_process.spawn).toHaveBeenCalledWith(
      'git',
      ['push', `${option}=rnw-test-command`],
      expect.objectContaining({
        env: expect.objectContaining({
          GIT_TEST_DISALLOW_ABBREVIATED_OPTIONS: 'true',
        }),
      }),
    );
  },
);

test('rejects trailer commands in instance configuration before spawning', async () => {
  await expect(
    simpleGit({config: ['trailer.rnw.cmd=rnw-test-command']}).raw(['status']),
  ).rejects.toBeInstanceOf(GitPluginError);
  expect(child_process.spawn).not.toHaveBeenCalled();
});

test('rejects trailer commands in inline configuration before spawning', async () => {
  await expect(
    simpleGit().raw(['-c', 'trailer.rnw.cmd=rnw-test-command', 'status']),
  ).rejects.toBeInstanceOf(GitPluginError);
  expect(child_process.spawn).not.toHaveBeenCalled();
});

test('rejects the VISUAL editor environment variable before spawning', async () => {
  await expect(
    simpleGit().env({VISUAL: 'rnw-test-editor'}).raw(['status']),
  ).rejects.toBeInstanceOf(GitPluginError);
  expect(child_process.spawn).not.toHaveBeenCalled();
});
