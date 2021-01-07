/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {
  MockReactFileRepository,
  MockWritableFileRepository,
} from './MockFileRepository';
import {hashContent, hashFileOrDirectory} from '../Hash';
import {ValidationStrategies} from '../ValidationStrategy';

const reactRepo = new MockReactFileRepository([
  {filename: 'foo.js', content: 'ABCD'},
  {filename: 'RNTester/index.js', content: '1234'},
  {filename: 'RNTester/abs.js', content: '12eee34'},
]);
const overrideRepo = new MockWritableFileRepository([
  {filename: 'foo.windows.js', content: 'DCBA'},
  {filename: 'bar/RNTester/index.js', content: '1234'},
  {filename: 'bar/RNTester/abs.js', content: '12eee34'},
  {filename: 'bar/RNTester/def.windowsjs', content: 'Extra'},
]);

test('overrideFileExists - File Exists', async () => {
  expect(
    await ValidationStrategies.overrideFileExists('foo.windows.js').validate(
      overrideRepo,
      reactRepo,
    ),
  ).toEqual([]);
});

test('overrideFileExists - File Does Not Exist', async () => {
  expect(
    await ValidationStrategies.overrideFileExists('nope.windows.js').validate(
      overrideRepo,
      reactRepo,
    ),
  ).toEqual([{type: 'overrideNotFound', overrideName: 'nope.windows.js'}]);
});

test('overrideFileExists - Is Directory', async () => {
  expect(
    await ValidationStrategies.overrideFileExists('bar').validate(
      overrideRepo,
      reactRepo,
    ),
  ).toEqual([{type: 'expectedFile', overrideName: 'bar'}]);
});

test('overrideDirectoryExists - Directory Exists', async () => {
  expect(
    await ValidationStrategies.overrideDirectoryExists('bar/RNTester').validate(
      overrideRepo,
      reactRepo,
    ),
  ).toEqual([]);
});

test('overrideDirectoryExists - Directory Does Not Exist', async () => {
  expect(
    await ValidationStrategies.overrideDirectoryExists('nope').validate(
      overrideRepo,
      reactRepo,
    ),
  ).toEqual([{type: 'overrideNotFound', overrideName: 'nope'}]);
});

test('overrideDirectoryExists - Is File', async () => {
  expect(
    await ValidationStrategies.overrideDirectoryExists(
      'foo.windows.js',
    ).validate(overrideRepo, reactRepo),
  ).toEqual([{type: 'expectedDirectory', overrideName: 'foo.windows.js'}]);
});

test('baseFileExists - File Exists', async () => {
  expect(
    await ValidationStrategies.baseFileExists(
      'foo.windows.js',
      'foo.js',
    ).validate(overrideRepo, reactRepo),
  ).toEqual([]);
});

test('baseFileExists - File Does Not Exist', async () => {
  expect(
    await ValidationStrategies.baseFileExists(
      'foo.windows.js',
      'nope.js',
    ).validate(overrideRepo, reactRepo),
  ).toEqual([{type: 'baseNotFound', overrideName: 'foo.windows.js'}]);
});

test('baseFileExists - Is Directory', async () => {
  expect(
    await ValidationStrategies.baseFileExists(
      'foo.windows.js',
      'RNTester',
    ).validate(overrideRepo, reactRepo),
  ).toEqual([{type: 'expectedFile', overrideName: 'foo.windows.js'}]);
});

test('baseDirectoryExists - Directory Exists', async () => {
  expect(
    await ValidationStrategies.baseDirectoryExists(
      'bar/RNTester',
      'RNTester',
    ).validate(overrideRepo, reactRepo),
  ).toEqual([]);
});

test('baseDirectoryExists - Directory Does Not Exist', async () => {
  expect(
    await ValidationStrategies.baseDirectoryExists(
      'bar/RNTester',
      'nope',
    ).validate(overrideRepo, reactRepo),
  ).toEqual([{type: 'baseNotFound', overrideName: 'bar/RNTester'}]);
});

test('baseDirectoryExists - Is File', async () => {
  expect(
    await ValidationStrategies.baseDirectoryExists(
      'bar/RNTester',
      'foo.js',
    ).validate(overrideRepo, reactRepo),
  ).toEqual([{type: 'expectedDirectory', overrideName: 'bar/RNTester'}]);
});

test('baseUpToDate - Up To Date File', async () => {
  expect(
    await ValidationStrategies.baseUpToDate(
      'foo.windows.js',
      'foo.js',
      hashContent('ABCD'),
    ).validate(overrideRepo, reactRepo),
  ).toEqual([]);
});

test('baseUpToDate - Out Of Date File', async () => {
  expect(
    await ValidationStrategies.baseUpToDate(
      'foo.windows.js',
      'foo.js',
      hashContent('Old content'),
    ).validate(overrideRepo, reactRepo),
  ).toEqual([{type: 'outOfDate', overrideName: 'foo.windows.js'}]);
});

test('baseUpToDate - Up To Date Directory', async () => {
  expect(
    await ValidationStrategies.baseUpToDate(
      'bar/RNTester',
      'RNTester',
      (await hashFileOrDirectory('RNTester', reactRepo))!,
    ).validate(overrideRepo, reactRepo),
  ).toEqual([]);
});

test('baseUpToDate - Out Of Date Directory', async () => {
  expect(
    await ValidationStrategies.baseUpToDate(
      'bar/RNTester',
      'RNTester',
      'OldHash',
    ).validate(overrideRepo, reactRepo),
  ).toEqual([{type: 'outOfDate', overrideName: 'bar/RNTester'}]);
});

test('baseUpToDate - No Override', async () => {
  expect(
    await ValidationStrategies.baseUpToDate(
      'nope.windows.js',
      'foo.js',
      hashContent('ABCD'),
    ).validate(overrideRepo, reactRepo),
  ).toEqual([]);
});

test('baseUpToDate - No Base', async () => {
  expect(
    await ValidationStrategies.baseUpToDate(
      'foo.windows.js',
      'nope.js',
      hashContent('ABCD'),
    ).validate(overrideRepo, reactRepo),
  ).toEqual([]);
});

test('overrideCopyOfBase - File Is Copy', async () => {
  expect(
    await ValidationStrategies.overrideCopyOfBase(
      'foo.windows.js',
      'foo.js',
    ).validate(
      new MockWritableFileRepository([
        {filename: 'foo.windows.js', content: 'ABCD'},
      ]),
      new MockReactFileRepository([{filename: 'foo.js', content: 'ABCD'}]),
    ),
  ).toEqual([]);
});

test('overrideCopyOfBase - Directory Is Copy', async () => {
  expect(
    await ValidationStrategies.overrideCopyOfBase('src/foo', 'foo').validate(
      new MockWritableFileRepository([
        {filename: 'src/foo/a.js', content: 'ABCD'},
        {filename: 'src/foo/b.js', content: 'EFGH'},
      ]),
      new MockReactFileRepository([
        {filename: 'foo/b.js', content: 'EFGH'},
        {filename: 'foo/a.js', content: 'ABCD'},
      ]),
    ),
  ).toEqual([]);
});

test('overrideCopyOfBase - Different Line Endings', async () => {
  expect(
    await ValidationStrategies.overrideCopyOfBase(
      'foo.windows.js',
      'foo.js',
    ).validate(
      new MockWritableFileRepository([
        {filename: 'foo.windows.js', content: 'ABCD\n'},
      ]),
      new MockReactFileRepository([{filename: 'foo.js', content: 'ABCD\r\n'}]),
    ),
  ).toEqual([]);
});

test('overrideCopyOfBase - File Not A Copy', async () => {
  expect(
    await ValidationStrategies.overrideCopyOfBase(
      'foo.windows.js',
      'foo.js',
    ).validate(overrideRepo, reactRepo),
  ).toEqual([
    {type: 'overrideDifferentFromBase', overrideName: 'foo.windows.js'},
  ]);
});

test('overrideCopyOfBase - Directory Not A Copy', async () => {
  expect(
    await ValidationStrategies.overrideCopyOfBase('src/foo', 'foo').validate(
      new MockWritableFileRepository([
        {filename: 'src/foo/a.js', content: 'ABCDDCBA'},
        {filename: 'src/foo/b.js', content: 'EFGH'},
      ]),
      new MockReactFileRepository([
        {filename: 'foo/b.js', content: 'EFGH'},
        {filename: 'foo/a.js', content: 'ABCD'},
      ]),
    ),
  ).toEqual([{type: 'overrideDifferentFromBase', overrideName: 'src/foo'}]);
});

test('overrideCopyOfBase - Directory Has Extra File', async () => {
  expect(
    await ValidationStrategies.overrideCopyOfBase(
      'bar/RNTester',
      'RNTester',
    ).validate(overrideRepo, reactRepo),
  ).toEqual([
    {type: 'overrideDifferentFromBase', overrideName: 'bar/RNTester'},
  ]);
});

test('overrideCopyOfBase - No Override', async () => {
  expect(
    await ValidationStrategies.overrideCopyOfBase('nope.js', 'foo.js').validate(
      overrideRepo,
      reactRepo,
    ),
  ).toEqual([]);
});

test('overrideCopyOfBase - No Base', async () => {
  expect(
    await ValidationStrategies.overrideCopyOfBase(
      'foo.windows.js',
      'nope.js',
    ).validate(overrideRepo, reactRepo),
  ).toEqual([]);
});

test('overrideDifferentFromBase - File Is Copy', async () => {
  expect(
    await ValidationStrategies.overrideDifferentFromBase(
      'foo.windows.js',
      'foo.js',
    ).validate(
      new MockWritableFileRepository([
        {filename: 'foo.windows.js', content: 'ABCD'},
      ]),
      new MockReactFileRepository([{filename: 'foo.js', content: 'ABCD'}]),
    ),
  ).toEqual([{type: 'overrideSameAsBase', overrideName: 'foo.windows.js'}]);
});

test('overrideDifferentFromBase - Directory Is Copy', async () => {
  expect(
    await ValidationStrategies.overrideDifferentFromBase(
      'src/foo',
      'foo',
    ).validate(
      new MockWritableFileRepository([
        {filename: 'src/foo/a.js', content: 'ABCD'},
        {filename: 'src/foo/b.js', content: 'EFGH'},
      ]),
      new MockReactFileRepository([
        {filename: 'foo/b.js', content: 'EFGH'},
        {filename: 'foo/a.js', content: 'ABCD'},
      ]),
    ),
  ).toEqual([{type: 'overrideSameAsBase', overrideName: 'src/foo'}]);
});

test('overrideDifferentFromBase - Different Line Endings', async () => {
  expect(
    await ValidationStrategies.overrideDifferentFromBase(
      'foo.windows.js',
      'foo.js',
    ).validate(
      new MockWritableFileRepository([
        {filename: 'foo.windows.js', content: 'ABCD\n'},
      ]),
      new MockReactFileRepository([{filename: 'foo.js', content: 'ABCD\r\n'}]),
    ),
  ).toEqual([{type: 'overrideSameAsBase', overrideName: 'foo.windows.js'}]);
});

test('overrideDifferentFromBase - Different Whitespace', async () => {
  expect(
    await ValidationStrategies.overrideDifferentFromBase(
      'foo.windows.js',
      'foo.js',
    ).validate(
      new MockWritableFileRepository([
        {filename: 'foo.windows.js', content: ' A   B \r\n C D\n'},
      ]),
      new MockReactFileRepository([{filename: 'foo.js', content: 'A B C D '}]),
    ),
  ).toEqual([{type: 'overrideSameAsBase', overrideName: 'foo.windows.js'}]);
});

test('overrideDifferentFromBase - File Not A Copy', async () => {
  expect(
    await ValidationStrategies.overrideDifferentFromBase(
      'foo.windows.js',
      'foo.js',
    ).validate(overrideRepo, reactRepo),
  ).toEqual([]);
});

test('overrideDifferentFromBase - Directory Not A Copy', async () => {
  expect(
    await ValidationStrategies.overrideDifferentFromBase(
      'src/foo',
      'foo',
    ).validate(
      new MockWritableFileRepository([
        {filename: 'src/foo/a.js', content: 'ABCDDCBA'},
        {filename: 'src/foo/b.js', content: 'EFGH'},
      ]),
      new MockReactFileRepository([
        {filename: 'foo/b.js', content: 'EFGH'},
        {filename: 'foo/a.js', content: 'ABCD'},
      ]),
    ),
  ).toEqual([]);
});

test('overrideDifferentFromBase - Directory Has Extra File', async () => {
  expect(
    await ValidationStrategies.overrideDifferentFromBase(
      'bar/RNTester',
      'RNTester',
    ).validate(overrideRepo, reactRepo),
  ).toEqual([]);
});

test('overrideDifferentFromBase - No Override', async () => {
  expect(
    await ValidationStrategies.overrideDifferentFromBase(
      'nope.js',
      'foo.js',
    ).validate(overrideRepo, reactRepo),
  ).toEqual([]);
});

test('overrideDifferentFromBase - No Base', async () => {
  expect(
    await ValidationStrategies.overrideDifferentFromBase(
      'foo.windows.js',
      'nope.js',
    ).validate(overrideRepo, reactRepo),
  ).toEqual([]);
});
