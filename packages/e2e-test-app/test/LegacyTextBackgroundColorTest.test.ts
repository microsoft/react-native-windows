/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {goToComponentExample, assertTreeDumpPassed} from './Helpers';

beforeAll(async () => {
  await goToComponentExample('LegacyTextBackgroundColorTest');
});

describe('LegacyTextBackgroundColorTest', () => {
  /* Test case #1: view and image displayed with no border and cornerRadius */
  test('TextBackgroundColorTest', async () => {
    await assertTreeDumpPassed();
  });
});
