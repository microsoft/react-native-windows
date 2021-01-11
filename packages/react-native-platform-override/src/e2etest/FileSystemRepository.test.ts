/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {usingFiles} from './Resource';

test('listFiles - No Glob', async () => {
  const overrideFiles = [
    '0.59.9/Icon-60@2x.conflict.png',
    '0.61.5/flowconfig.windows.conflict',
    '0.62.2/flowconfig.windows.addition',
  ];

  await usingFiles(overrideFiles, async repo => {
    expect((await repo.listFiles()).sort()).toEqual(overrideFiles);
  });
});

test('listFiles - Single Glob', async () => {
  const overrideFiles = [
    '0.59.9/Icon-60@2x.conflict.png',
    '0.61.5/flowconfig.windows.conflict',
    '0.62.2/flowconfig.windows.addition',
  ];

  await usingFiles(overrideFiles, async repo => {
    expect(await repo.listFiles(['**/*.conflict'])).toEqual([
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

  await usingFiles(overrideFiles, async repo => {
    expect(
      (await repo.listFiles(['**/*.conflict', '**/*.addition'])).sort(),
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

  await usingFiles(overrideFiles, async repo => {
    expect((await repo.listFiles(['**', '!**/*.conflict'])).sort()).toEqual([
      '0.59.9/Icon-60@2x.conflict.png',
      '0.62.2/flowconfig.windows.addition',
    ]);
  });
});

test('writeFile - Existing File', async () => {
  const overrideFiles = [
    '0.59.9/Icon-60@2x.conflict.png',
    '0.61.5/flowconfig.windows.conflict',
    '0.62.2/flowconfig.windows.addition',
  ];

  await usingFiles(overrideFiles, async repo => {
    await repo.writeFile('0.59.9/Icon-60@2x.conflict.png', 'foo');
    const contents = await repo.readFile('0.59.9/Icon-60@2x.conflict.png');

    expect(contents).not.toBeNull();
    expect(contents!.toString()).toBe('foo');
  });
});

test('writeFile - New File', async () => {
  await usingFiles([], async repo => {
    await repo.writeFile('newDir/Hello.txt', 'Hello World!');
    const contents = await repo.readFile('newDir/Hello.txt');

    expect(contents).not.toBeNull();
    expect(contents!.toString()).toBe('Hello World!');
  });
});

test('stat - File', async () => {
  await usingFiles(['0.59.9/Icon-60@2x.conflict.png'], async repo => {
    expect(await repo.stat('0.59.9/Icon-60@2x.conflict.png')).toBe('file');
  });
});

test('stat - Directory', async () => {
  await usingFiles(['0.59.9/Icon-60@2x.conflict.png'], async repo => {
    expect(await repo.stat('0.59.9')).toBe('directory');
  });
});

test('stat - None', async () => {
  await usingFiles(['0.59.9/Icon-60@2x.conflict.png'], async repo => {
    expect(await repo.stat('0.59.9/IDontExist.txt')).toBe('none');
  });
});

test('deleteFile - Exists', async () => {
  await usingFiles(['0.59.9/Icon-60@2x.conflict.png'], async repo => {
    expect(await repo.stat('0.59.9/Icon-60@2x.conflict.png')).toBe('file');
    await repo.deleteFile('0.59.9/Icon-60@2x.conflict.png');
    expect(await repo.stat('0.59.9/Icon-60@2x.conflict.png')).toBe('none');
  });
});

test('deleteFile - Doesnt Exist', async () => {
  await usingFiles(['0.59.9/Icon-60@2x.conflict.png'], async repo => {
    // @ts-ignore Typings don't know about rejects
    expect(repo.deleteFile('nope')).rejects.toThrow();
  });
});
