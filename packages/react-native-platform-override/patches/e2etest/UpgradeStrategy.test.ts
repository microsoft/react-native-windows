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
import {acquireGitRepo, usingFiles} from './Resource';
import GitReactFileRepository from '../GitReactFileRepository';
import {hashFileOrDirectory} from '../Hash';
import isUtf8 from 'isutf8';

let gitReactRepo: GitReactFileRepository;
let disposeReactRepo: () => Promise<void>;

beforeAll(async () => {
  [gitReactRepo, disposeReactRepo] = await acquireGitRepo();
});

afterAll(async () => {
  await disposeReactRepo();
});

test('assumeUpToDate', async () => {
  const overrideFile = '0.61.5/flowconfig.windows.conflict';
  const strategy = UpgradeStrategies.assumeUpToDate(overrideFile);

  await evaluateStrategy({
    strategy,
    overrideFile,
    upgradeVersion: '0.62.2',
    allowConflicts: true,
    expected: {
      referenceFile: '0.61.5/flowconfig.windows.conflict',
      overrideName: overrideFile,
      filesWritten: false,
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
      overrideName: overrideFile,
      filesWritten: true,
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
      overrideName: overrideFile,
      filesWritten: true,
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
      overrideName: overrideFile,
      filesWritten: false,
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
      overrideName: overrideFile,
      filesWritten: false,
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
      overrideName: overrideFile,
      filesWritten: true,
      hasConflicts: false,
    },
  });
});

test('copyDirectory - New Files', async () => {
  const overrideDir = 'src/jest';
  const baseDir = 'jest';

  const strategy = UpgradeStrategies.copyDirectory(overrideDir, baseDir);
  await usingFiles([], async overrideRepo => {
    const results = await strategy.upgrade(
      gitReactRepo,
      overrideRepo,
      '0.62.2',
      true,
    );
    expect(results).toEqual({
      overrideName: overrideDir,
      filesWritten: true,
      hasConflicts: false,
    });

    expect((await overrideRepo.listFiles()).sort()).toEqual([
      'src/jest/MockNativeMethods.js',
      'src/jest/assetFileTransformer.js',
      'src/jest/mockComponent.js',
      'src/jest/preprocessor.js',
      'src/jest/renderer.js',
      'src/jest/setup.js',
    ]);
  });
});

test('copyDirectory - New Content', async () => {
  const overrideFiles = [
    '0.62.2/jest/MockNativeMethods.js',
    '0.62.2/jest/assetFileTransformer.js',
    '0.62.2/jest/mockComponent.js',
    '0.62.2/jest/preprocessor.js',
    '0.62.2/jest/renderer.js',
    '0.62.2/jest/setup.js',
  ];

  const strategy = UpgradeStrategies.copyDirectory('0.62.2/jest', 'jest');
  await usingFiles(overrideFiles, async overrideRepo => {
    const correctHash = await hashFileOrDirectory(
      overrideFiles[0],
      overrideRepo,
    );

    expect(correctHash).not.toBeNull();

    await overrideRepo.writeFile(overrideFiles[0], 'Garbage Data');
    expect(
      await hashFileOrDirectory(overrideFiles[0], overrideRepo),
    ).not.toEqual(correctHash);

    const results = await strategy.upgrade(
      gitReactRepo,
      overrideRepo,
      '0.62.2',
      true,
    );
    expect(results).toEqual({
      overrideName: '0.62.2/jest',
      filesWritten: true,
      hasConflicts: false,
    });

    expect(await hashFileOrDirectory(overrideFiles[0], overrideRepo)).toEqual(
      correctHash,
    );
  });
});

test('copyDirectory - Deleted Content', async () => {
  const overrideFiles = [
    '0.62.2/jest/MockNativeMethods.js',
    '0.62.2/jest/assetFileTransformer.js',
    '0.62.2/jest/mockComponent.js',
    '0.62.2/jest/preprocessor.js',
    '0.62.2/jest/renderer.js',
    '0.62.2/jest/setup.js',
  ];

  const strategy = UpgradeStrategies.copyDirectory('0.62.2/jest', 'jest');
  await usingFiles(overrideFiles, async overrideRepo => {
    await overrideRepo.writeFile('0.62.2/jest/extraFile.txt', 'Delete me');
    expect(await overrideRepo.stat('0.62.2/jest/extraFile.txt')).toBe('file');

    const results = await strategy.upgrade(
      gitReactRepo,
      overrideRepo,
      '0.62.2',
      true,
    );
    expect(results).toEqual({
      overrideName: '0.62.2/jest',
      filesWritten: true,
      hasConflicts: false,
    });

    expect(await overrideRepo.stat('0.62.2/jest/extraFile.txt')).toBe('none');
  });
});

test('copyDirectory - Preserves Line Endings', async () => {
  const overrideFiles = [
    '0.62.2/jest/MockNativeMethods.js',
    '0.62.2/jest/assetFileTransformer.js',
    '0.62.2/jest/mockComponent.js',
    '0.62.2/jest/preprocessor.js',
    '0.62.2/jest/renderer.js',
    '0.62.2/jest/setup.js',
  ];

  const strategy = UpgradeStrategies.copyDirectory('0.62.2/jest', 'jest');
  await usingFiles(overrideFiles, async overrideRepo => {
    const origContent = (await overrideRepo.readFile(
      overrideFiles[1],
    ))!.toString();
    const switchedEndings = origContent.includes('\r\n')
      ? origContent.replace(/\r\n/g, '\n')
      : origContent.replace(/(?<!\r)\n/g, '\r\n');

    expect(origContent).not.toEqual(switchedEndings);

    await overrideRepo.writeFile(overrideFiles[1], switchedEndings);

    const results = await strategy.upgrade(
      gitReactRepo,
      overrideRepo,
      '0.62.2',
      true,
    );
    expect(results).toEqual({
      overrideName: '0.62.2/jest',
      filesWritten: true,
      hasConflicts: false,
    });

    expect((await overrideRepo.readFile(overrideFiles[1]))!.toString()).toEqual(
      switchedEndings,
    );
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
  await usingFiles([opts.overrideFile], async (overrideRepo, overridesPath) => {
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

    if (isUtf8(actualContent) && isUtf8(expectedContent)) {
      expect(actualContent.toString().replace(/\r\n/g, '\n')).toBe(
        expectedContent.toString().replace(/\r\n/g, '\n'),
      );
    } else {
      expect(actualContent.compare(expectedContent)).toBe(0);
    }
  });
}
