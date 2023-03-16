/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import renderPropsFile from '../renderPropsFile';

test('0.0.0-canary.100', async () => {
  expect(await renderPropsFile('0.0.0-canary.100', 'abc123')).toMatchSnapshot();
});

test('0.67.0-preview.1', async () => {
  expect(await renderPropsFile('0.67.0-preview.1', 'def456')).toMatchSnapshot();
});

test('0.66.5', async () => {
  expect(await renderPropsFile('0.66.5', '123abc')).toMatchSnapshot();
});
