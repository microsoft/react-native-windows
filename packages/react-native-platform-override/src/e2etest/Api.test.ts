/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as Api from '../Api';
import * as path from 'path';
import {normalizePath} from '../PathUtils';
import {usingRepository} from './Resource';

const SAMPLE_REPO_VERSION = '0.0.0-56cf99a96';

test('validateManifest', async () => {
  await usingRepository('sampleOverrideRepo', async (_, repoPath) => {
    const opts = {
      manifestPath: path.join(repoPath, 'overrides.json'),
      reactNativeVersion: SAMPLE_REPO_VERSION,
    };

    const validatonErrors = await Api.validateManifest(opts);

    expect(validatonErrors).toEqual([
      {
        overrideName: 'setUpDeveloperTools.windesktop.js',
        type: 'missingFromManifest',
      },
    ]);
  });
});

test('hasOverride', async () => {
  await usingRepository('sampleOverrideRepo', async (_, repoPath) => {
    const opts = {
      manifestPath: path.join(repoPath, 'overrides.json'),
    };

    expect(
      await Api.hasOverride('ReactCommon\\yoga\\yoga\\Yoga.cpp', opts),
    ).toBe(true);

    expect(
      await Api.hasOverride('ReactCommon\\yoga\\yoga\\Karate.cpp', opts),
    ).toBe(false);
  });
});

test('removeOverride', async () => {
  await usingRepository('sampleOverrideRepo', async (_, repoPath) => {
    const opts = {
      manifestPath: path.join(repoPath, 'overrides.json'),
    };

    expect(
      await Api.removeOverride('ReactCommon\\yoga\\yoga\\Yoga.cpp', opts),
    ).toBe(true);

    expect(
      await Api.removeOverride('ReactCommon\\yoga\\yoga\\Karate.cpp', opts),
    ).toBe(false);
  });
});

test('addOverride', async () => {
  await usingRepository('sampleOverrideRepo', async (_, repoPath) => {
    const opts = {
      manifestPath: path.join(repoPath, 'overrides.json'),
      reactNativeVersion: SAMPLE_REPO_VERSION,
    };

    const factory = await Api.getOverrideFactory(opts);
    const override = await factory.createPlatformOverride(
      'setUpDeveloperTools.windesktop.js',
    );

    await Api.addOverride(override, opts);
    expect(
      await Api.hasOverride('setUpDeveloperTools.windesktop.js', opts),
    ).toBe(true);

    expect(await Api.validateManifest(opts)).toEqual([]);
  });
});

test('upgradeOverrides', async () => {
  await usingRepository('sampleOverrideRepo', async (repo, repoPath) => {
    const opts = {
      manifestPath: path.join(repoPath, 'overrides.json'),
      reactNativeVersion: '0.0.0-42c8dead6',
      allowConflicts: false,
      progressListener: expectIncrementing(3),
    };

    const upgradeResults = await Api.upgradeOverrides(opts);

    expect(upgradeResults).toEqual([
      {
        filesWritten: true,
        hasConflicts: false,
        overrideName: normalizePath(
          'ReactCommon\\turbomodule\\samples\\SampleTurboCxxModule.cpp',
        ),
      },
      {
        filesWritten: true,
        hasConflicts: false,
        overrideName: normalizePath(
          'ReactCommon\\turbomodule\\samples\\SampleTurboCxxModule.h',
        ),
      },
      {
        filesWritten: false,
        hasConflicts: true,
        overrideName: normalizePath('ReactCommon\\yoga\\yoga\\Yoga.cpp'),
      },
    ]);

    const manifest = JSON.parse(
      (await repo.readFile('sampleOverrideRepo/overrides.json'))!.toString(),
    );

    expect(manifest.baseVersion).toBe(opts.reactNativeVersion);

    for (const serializedOverride of manifest.overrides) {
      if (serializedOverride.file === 'ReactCommon/yoga/yoga/Yoga.cpp') {
        expect(serializedOverride.baseVersion).toBe('0.0.0-56cf99a96');
      } else {
        expect(serializedOverride.baseVersion).toBeUndefined();
      }
    }
  });
});

test('diffOverride', async () => {
  await usingRepository('sampleOverrideRepo', async (_, repoPath) => {
    const opts = {
      manifestPath: path.join(repoPath, 'overrides.json'),
    };

    const diff = await Api.diffOverride('ReactCommon/yoga/yoga/Yoga.cpp', opts);
    expect(diff.length).toBeGreaterThan(0);
  });
});

function expectIncrementing(
  expectedTotal: number,
): Api.UpgradeProgressListener {
  let expectedCurrent = 1;

  return (current, total) => {
    expect(current).toBe(expectedCurrent);
    expect(total).toBe(expectedTotal);
    expect(current).toBeLessThanOrEqual(total);
    expectedCurrent++;
  };
}
