/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import fs from 'node:fs/promises';
import path from 'path';
import {simpleGit} from 'simple-git';

import GitReactFileRepository from '../GitReactFileRepository';
import {usingScratchDirectory} from '../e2etest/Resource';

jest.mock('simple-git', () => {
  const actual = jest.requireActual('simple-git');
  return {...actual, simpleGit: jest.fn(actual.simpleGit)};
});

afterEach(() => {
  jest.restoreAllMocks();
});

test.each([false, true])(
  'checks out a fetched version over stale untracked files (existing checkout: %s)',
  async existingCheckout => {
    await usingScratchDirectory(async dir => {
      const sourceDir = path.join(dir, 'source');
      const cacheDir = path.join(dir, 'cache');
      await fs.mkdir(sourceDir);
      await fs.mkdir(cacheDir);

      const sourceGit = simpleGit(sourceDir);
      await sourceGit.init();
      await sourceGit.addConfig('user.name', 'Override Test');
      await sourceGit.addConfig('user.email', 'override-test@example.com');
      await fs.writeFile(path.join(sourceDir, 'README.md'), 'upstream');
      await sourceGit.add('.');
      await sourceGit.commit('Initial version');
      await sourceGit.addTag('v0.0.1');

      const cacheGit = simpleGit(cacheDir);
      jest.mocked(simpleGit).mockReturnValueOnce(cacheGit);
      const repo = await GitReactFileRepository.createAndInit(cacheDir);

      if (existingCheckout) {
        await cacheGit.fetch([sourceDir, 'refs/tags/v0.0.1', '--depth=1']);
        await cacheGit.checkout(['-b', '0.0.1', 'FETCH_HEAD']);
      }

      const files = [
        'package.json',
        path.join('gradle', 'wrapper', 'gradle-wrapper.jar'),
      ];
      for (const file of files) {
        await fs.mkdir(path.dirname(path.join(sourceDir, file)), {
          recursive: true,
        });
        await fs.writeFile(path.join(sourceDir, file), 'new upstream content');
        await fs.mkdir(path.dirname(path.join(cacheDir, file)), {
          recursive: true,
        });
        await fs.writeFile(path.join(cacheDir, file), 'stale cache content');
      }
      await sourceGit.add('.');
      await sourceGit.commit('New version');
      await sourceGit.addTag('v0.0.2');
      await fs.writeFile(path.join(cacheDir, 'unrelated.txt'), 'keep me');

      // Fetch from the local fixture instead of contacting GitHub.
      const fetch = cacheGit.fetch.bind(cacheGit);
      const fetchSpy = jest
        .spyOn(cacheGit, 'fetch')
        .mockImplementationOnce(() =>
          fetch([sourceDir, 'refs/tags/v0.0.2', '--depth=1']),
        );

      for (const file of files) {
        expect((await repo.readFile(file, '0.0.2'))?.toString()).toBe(
          'new upstream content',
        );
      }
      expect(fetchSpy).toHaveBeenCalledTimes(1);
      expect((await cacheGit.branch()).current).toBe('0.0.2');
      expect(
        await fs.readFile(path.join(cacheDir, 'unrelated.txt'), 'utf8'),
      ).toBe('keep me');
    });
  },
);
