/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {goToComponentExample, dumpVisualTree} from './framework';

beforeAll(async () => {
  await goToComponentExample('Text');
});

describe('Text examples dump test', () => {
  test('text_dump_test', async () => {
    const dump = await dumpVisualTree('text-dump-test', {
      additionalProperties: ['TextHighlighters'],
    });
    expect(dump).toMatchSnapshot();
  });
});
