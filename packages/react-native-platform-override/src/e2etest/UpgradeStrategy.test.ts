/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as fs from 'fs';
import * as path from 'path';

import UpgradeStrategy, {
  UpgradeResult,
  UpgradeStrategies,
} from '../UpgradeStrategy';
import {usingGitReactRepo, usingOverrideRepo} from './Resource';
import isutf8 from 'isutf8';

test('assumeUpToDate', async () => {
  const overrideFile = '0.61.5/flowconfig.windows.conflict';
  const strategy = UpgradeStrategies.assumeUpToDate();

  await evaluateStrategy({
    strategy,
    overrideFile,
    upgradeVersion: '0.62.2',
    allowConflicts: true,
    expected: {
      referenceFile: '0.61.5/flowconfig.windows.conflict',
      fileWritten: false,
      hasConflicts: false,
    },
  });
});

test('threeWayMerge - Simple Addition', async () => {
  const overrideFile = '0.61.5/flowconfig.windows.addition';
  const baseFile = '.flowconfig';
  const baseVersion = '0.61.5';

  const strategy = UpgradeStrategies.threeWayMerge(
    overrideFile,
    baseFile,
    baseVersion,
  );

  await evaluateStrategy({
    strategy,
    overrideFile,
    upgradeVersion: '0.62.2',
    allowConflicts: true,
    expected: {
      referenceFile: '0.62.2/flowconfig.windows.addition',
      fileWritten: true,
      hasConflicts: false,
    },
  });
});

test('threeWayMerge - Text Conflict (Allowed)', async () => {
  const overrideFile = '0.61.5/flowconfig.windows.conflict';
  const baseFile = '.flowconfig';
  const baseVersion = '0.61.5';

  const strategy = UpgradeStrategies.threeWayMerge(
    overrideFile,
    baseFile,
    baseVersion,
  );

  await evaluateStrategy({
    strategy,
    overrideFile,
    upgradeVersion: '0.62.2',
    allowConflicts: true,
    expected: {
      referenceFile: '0.62.2/flowconfig.windows.conflict',
      fileWritten: true,
      hasConflicts: true,
    },
  });
});

test('threeWayMerge - Text Conflict (Disallowed)', async () => {
  const overrideFile = '0.61.5/flowconfig.windows.conflict';
  const baseFile = '.flowconfig';
  const baseVersion = '0.61.5';

  const strategy = UpgradeStrategies.threeWayMerge(
    overrideFile,
    baseFile,
    baseVersion,
  );

  await evaluateStrategy({
    strategy,
    overrideFile,
    upgradeVersion: '0.62.2',
    allowConflicts: false,
    expected: {
      referenceFile: '0.61.5/flowconfig.windows.conflict',
      fileWritten: false,
      hasConflicts: true,
    },
  });
});

test('threeWayMerge - Binary Conflict', async () => {
  const overrideFile = '0.59.9/Icon-60@2x.conflict.png';
  const baseFile =
    'RNTester/RNTester/Images.xcassets/AppIcon.appiconset/Icon-60@2x.png';
  const baseVersion = '0.59.9';

  const strategy = UpgradeStrategies.threeWayMerge(
    overrideFile,
    baseFile,
    baseVersion,
  );

  await evaluateStrategy({
    strategy,
    overrideFile,
    upgradeVersion: '0.62.2',
    allowConflicts: true,
    expected: {
      referenceFile: '0.59.9/Icon-60@2x.conflict.png',
      fileWritten: false,
      hasConflicts: true,
    },
  });
});

test('copyFile', async () => {
  const overrideFile = '0.61.5/flowconfig.windows.conflict';
  const baseFile = '.flowconfig';

  const strategy = UpgradeStrategies.copyFile(overrideFile, baseFile);

  await evaluateStrategy({
    strategy,
    overrideFile,
    upgradeVersion: '0.62.2',
    allowConflicts: true,
    expected: {
      referenceFile: '0.62.2/flowconfig.pristine',
      fileWritten: true,
      hasConflicts: false,
    },
  });
});

async function evaluateStrategy(opts: {
  strategy: UpgradeStrategy;
  overrideFile: string;
  upgradeVersion: string;
  allowConflicts: boolean;
  expected: {
    referenceFile: string;
  } & UpgradeResult;
}) {
  await usingOverrideRepo(
    [opts.overrideFile],
    async (overrideRepo, overridesPath) => {
      await usingGitReactRepo(async gitReactRepo => {
        const actualResult = await opts.strategy.upgrade(
          gitReactRepo,
          overrideRepo,
          opts.upgradeVersion,
          opts.allowConflicts,
        );

        const {referenceFile, ...expectedResult} = opts.expected;
        expect(actualResult).toEqual(expectedResult);

        const actualContent = await fs.promises.readFile(
          path.join(overridesPath, opts.overrideFile),
        );
        const expectedContent = await fs.promises.readFile(
          path.join(__dirname, 'collateral', referenceFile),
        );

        if (isutf8(actualContent) && isutf8(expectedContent)) {
          expect(actualContent.toString()).toBe(expectedContent.toString());
        } else {
          expect(actualContent.compare(expectedContent)).toBe(0);
        }
      });
    },
  );
}
