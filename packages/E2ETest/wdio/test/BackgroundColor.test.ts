/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */

import { goToComponentExample, assertTreeDumpPassed } from './Helpers';

beforeAll(async () => {
  await goToComponentExample('LegacyTextBackgroundColorTest');
});

describe('TextBackgroundColorTest', () => {
  /* Test case #1: view and image displayed with no border and cornerRadius */
  it('TextBackgroundColorTest', async () => {
    await assertTreeDumpPassed();
  });
});
