/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {goToComponentExample, dumpVisualTree} from './framework';

beforeAll(async () => {
  await goToComponentExample('LegacyTextBackgroundColorTest');
});

describe('LegacyTextBackgroundColorTest', () => {
  /* Test case #1: view and image displayed with no border and cornerRadius */
  test('TextBackgroundColorTest', async () => {
    const dump = await dumpVisualTree('text-color-background-view', {
      additionalProperties: ['TextHighlighters'],
    });
    expect(dump).toMatchSnapshot();
  });
});
