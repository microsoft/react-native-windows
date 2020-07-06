/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {usingOverrideRepo} from './Resource';

test('listFiles - No Glob', async () => {
  const overrideFiles = [
    '0.59.9/Icon-60@2x.conflict.png',
    '0.61.5/flowconfig.windows.conflict',
    '0.62.2/flowconfig.windows.addition',
  ];

  await usingOverrideRepo(overrideFiles, async ovrRepo => {
    expect((await ovrRepo.listFiles()).sort()).toEqual(overrideFiles);
  });
});

test('listFiles - Single Glob', async () => {
  const overrideFiles = [
    '0.59.9/Icon-60@2x.conflict.png',
    '0.61.5/flowconfig.windows.conflict',
    '0.62.2/flowconfig.windows.addition',
  ];

  await usingOverrideRepo(overrideFiles, async ovrRepo => {
    expect(await ovrRepo.listFiles(['**/*.conflict'])).toEqual([
      '0.61.5/flowconfig.windows.conflict',
    ]);
  });
});

test('listFiles - Multiple Globs', async () => {
  const overrideFiles = [
    '0.59.9/Icon-60@2x.conflict.png',
    '0.61.5/flowconfig.windows.conflict',
    '0.62.2/flowconfig.windows.addition',
  ];

  await usingOverrideRepo(overrideFiles, async ovrRepo => {
    expect(
      (await ovrRepo.listFiles(['**/*.conflict', '**/*.addition'])).sort(),
    ).toEqual([
      '0.61.5/flowconfig.windows.conflict',
      '0.62.2/flowconfig.windows.addition',
    ]);
  });
});

test('listFiles - Negation Glob', async () => {
  const overrideFiles = [
    '0.59.9/Icon-60@2x.conflict.png',
    '0.61.5/flowconfig.windows.conflict',
    '0.62.2/flowconfig.windows.addition',
  ];

  await usingOverrideRepo(overrideFiles, async ovrRepo => {
    expect((await ovrRepo.listFiles(['**', '!**/*.conflict'])).sort()).toEqual([
      '0.59.9/Icon-60@2x.conflict.png',
      '0.62.2/flowconfig.windows.addition',
    ]);
  });
});

test('listFiles - setFileContents', async () => {
  const overrideFiles = [
    '0.59.9/Icon-60@2x.conflict.png',
    '0.61.5/flowconfig.windows.conflict',
    '0.62.2/flowconfig.windows.addition',
  ];

  await usingOverrideRepo(overrideFiles, async ovrRepo => {
    await ovrRepo.setFileContents(
      '0.59.9/Icon-60@2x.conflict.png',
      Buffer.from('foo'),
    );
    const contents = await ovrRepo.getFileContents(
      '0.59.9/Icon-60@2x.conflict.png',
    );

    expect(contents).not.toBeNull();
    expect(contents!.toString()).toBe('foo');
  });
});
