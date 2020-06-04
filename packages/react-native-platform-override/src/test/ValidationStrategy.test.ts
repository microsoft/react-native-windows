/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {
  MockOverrideFileRepository,
  MockReactFileRepository,
} from './MockFileRepository';

import {ValidationStrategies} from '../ValidationStrategy';
import {hashContent} from '../Hash';

const reactRepo = new MockReactFileRepository([
  {filename: 'foo.js', content: 'ABCD'},
]);
const overrideRepo = new MockOverrideFileRepository([
  {filename: 'foo.windows.js', content: 'DCBA'},
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

test('baseFileUpToDate - Up to date', async () => {
  expect(
    await ValidationStrategies.baseFileUpToDate(
      'foo.windows.js',
      'foo.js',
      hashContent('ABCD'),
    ).validate(overrideRepo, reactRepo),
  ).toEqual([]);
});

test('baseFileUpToDate - Out of date', async () => {
  expect(
    await ValidationStrategies.baseFileUpToDate(
      'foo.windows.js',
      'foo.js',
      hashContent('Old content'),
    ).validate(overrideRepo, reactRepo),
  ).toEqual([{type: 'outOfDate', overrideName: 'foo.windows.js'}]);
});

test('baseFileUpToDate - No Override', async () => {
  expect(
    await ValidationStrategies.baseFileUpToDate(
      'nope.windows.js',
      'foo.js',
      hashContent('ABCD'),
    ).validate(overrideRepo, reactRepo),
  ).toEqual([]);
});

test('baseFileUpToDate - No Base', async () => {
  expect(
    await ValidationStrategies.baseFileUpToDate(
      'foo.windows.js',
      'nope.js',
      hashContent('ABCD'),
    ).validate(overrideRepo, reactRepo),
  ).toEqual([]);
});

test('overrideCopyOfBaseFile - Exact Copy', async () => {
  expect(
    await ValidationStrategies.overrideCopyOfBaseFile(
      'foo.windows.js',
      'foo.js',
    ).validate(
      new MockOverrideFileRepository([
        {filename: 'foo.windows.js', content: 'ABCD'},
      ]),
      new MockReactFileRepository([{filename: 'foo.js', content: 'ABCD'}]),
    ),
  ).toEqual([]);
});

test('overrideCopyOfBaseFile - Different Line Endings', async () => {
  expect(
    await ValidationStrategies.overrideCopyOfBaseFile(
      'foo.windows.js',
      'foo.js',
    ).validate(
      new MockOverrideFileRepository([
        {filename: 'foo.windows.js', content: 'ABCD\n'},
      ]),
      new MockReactFileRepository([{filename: 'foo.js', content: 'ABCD\r\n'}]),
    ),
  ).toEqual([]);
});

test('overrideCopyOfBaseFile - Not a copy', async () => {
  expect(
    await ValidationStrategies.overrideCopyOfBaseFile(
      'foo.windows.js',
      'foo.js',
    ).validate(overrideRepo, reactRepo),
  ).toEqual([
    {type: 'overrideDifferentFromBase', overrideName: 'foo.windows.js'},
  ]);
});

test('overrideCopyOfBaseFile - No override', async () => {
  expect(
    await ValidationStrategies.overrideCopyOfBaseFile(
      'nope.js',
      'foo.js',
    ).validate(overrideRepo, reactRepo),
  ).toEqual([]);
});

test('overrideCopyOfBaseFile - No base', async () => {
  expect(
    await ValidationStrategies.overrideCopyOfBaseFile(
      'foo.windows.js',
      'nope.js',
    ).validate(overrideRepo, reactRepo),
  ).toEqual([]);
});
