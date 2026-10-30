/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import assert from 'node:assert/strict';
import {afterEach, it, mock} from 'node:test';
import fs from '@react-native-windows/fs';
import {createPR, updatePR} from '../github';
import * as proc from '../proc';

afterEach(() => mock.restoreAll());

// Cover both commands, optional repo flags, and cleanup on success or rejection.
for (const operation of ['create', 'edit'] as const) {
  void it(`${operation} preserves shell arguments and cleans UTF-8 bodies`, async () => {
    const execMock = mock.method(proc, 'exec', () => {
      throw new Error('Unexpected exec call');
    });
    for (const repo of [undefined, 'owner/repo']) {
      for (const failure of [undefined, 'label release not found']) {
        const title = '🚀 [0.85]: Release 1 package';
        const body = 'Unicode 🚀\nsecond line';
        let bodyFile = '';
        execMock.mock.mockImplementationOnce(async (command, options) => {
          bodyFile = command.match(/--body-file "([^"]+)"/)![1];
          const repoFlag = repo ? ` --repo "${repo}"` : '';
          const prefix =
            operation === 'create'
              ? 'gh pr create --head "prepare-release/0.85-stable" --base "0.85-stable"'
              : 'gh pr edit 42';
          const labelFlag = operation === 'create' ? '--label' : '--add-label';
          assert.equal(
            command,
            `${prefix} --title "${title}" --body-file "${bodyFile}"${repoFlag} ${labelFlag} release`,
          );
          assert.deepEqual(options, {cwd: 'repo root'});
          assert.equal(fs.readFileSync(bodyFile, 'utf8'), body);
          if (failure) {
            throw new Error(failure);
          }
          return 'https://github.com/owner/repo/pull/42\n';
        });
        const opts = {title, body, cwd: 'repo root', repo};
        const result =
          operation === 'create'
            ? createPR({
                ...opts,
                head: 'prepare-release/0.85-stable',
                base: '0.85-stable',
              })
            : updatePR({...opts, number: 42});
        if (failure) {
          await assert.rejects(result, new RegExp(failure));
        } else {
          assert.deepEqual(
            await result,
            operation === 'create'
              ? {number: 42, url: 'https://github.com/owner/repo/pull/42'}
              : undefined,
          );
        }
        assert.notEqual(bodyFile, '');
        assert.equal(fs.existsSync(bodyFile), false);
      }
    }
    assert.equal(execMock.mock.callCount(), 4);
  });
}
