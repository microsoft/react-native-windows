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

describe('TextTest', () => {
  test('Text transform', async () => {
    const dump = await dumpVisualTree('text-transform');
    expect(dump).toMatchSnapshot();
  });

  test('Text wrapping', async () => {
    const dump = await dumpVisualTree('text-wrap');
    expect(dump).toMatchSnapshot();
  });

  test('Text padding', async () => {
    const dump = await dumpVisualTree('text-padding');
    expect(dump).toMatchSnapshot();
  });

  test('Text font family', async () => {
    const dump = await dumpVisualTree('text-font-family');
    expect(dump).toMatchSnapshot();
  });

  test('Text size', async () => {
    const dump = await dumpVisualTree('text-size');
    expect(dump).toMatchSnapshot();
  });

  test('Text color', async () => {
    const dump = await dumpVisualTree('text-color');
    expect(dump).toMatchSnapshot();
  });

  test('Text decoration underline', async () => {
    const dump = await dumpVisualTree('text-decoration-underline');
    expect(dump).toMatchSnapshot();
  });

  test('Text decoration solid linethrough', async () => {
    const dump = await dumpVisualTree('text-decoration-solid-linethru');
    expect(dump).toMatchSnapshot();
  });

  test('Text color with children', async () => {
    const dump = await dumpVisualTree('text-outer-color');
    expect(dump).toMatchSnapshot();
  });

  test('Text alignment', async () => {
    const dump = await dumpVisualTree('text-align');
    expect(dump).toMatchSnapshot();
  });

  test('Text unicode characters', async () => {
    const dump = await dumpVisualTree('text-unicode');
    expect(dump).toMatchSnapshot();
  });

  test('Text line height', async () => {
    const dump = await dumpVisualTree('text-line-height');
    expect(dump).toMatchSnapshot();
  });

  test('Text letter spacing', async () => {
    const dump = await dumpVisualTree('text-letter-spacing');
    expect(dump).toMatchSnapshot();
  });

  test('Text force one line', async () => {
    const dump = await dumpVisualTree('text-one-line');
    expect(dump).toMatchSnapshot();
  });

  test('Text is selectable', async () => {
    const dump = await dumpVisualTree('text-selectable');
    expect(dump).toMatchSnapshot();
  });

  test('Text selection color', async () => {
    const dump = await dumpVisualTree('text-selection-color');
    expect(dump).toMatchSnapshot();
  });

  test('Text shadows', async () => {
    const dump = await dumpVisualTree('text-shadow');
    expect(dump).toMatchSnapshot();
  });

  test('Text ellipsize', async () => {
    const dump = await dumpVisualTree('text-ellipsize');
    expect(dump).toMatchSnapshot();
  });

  test('Text font padding', async () => {
    const dump = await dumpVisualTree('text-font-padding');
    expect(dump).toMatchSnapshot();
  });

  test('Text border', async () => {
    const dump = await dumpVisualTree('text-border');
    expect(dump).toMatchSnapshot();
  });

  test('Text background color', async () => {
    const dump = await dumpVisualTree('text-background-color', {
      additionalProperties: ['TextHighlighters'],
    });
    expect(dump).toMatchSnapshot();
  });
});
