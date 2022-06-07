/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import renderPropsFile from '../renderPropsFile';

test('0.0.0-canary.100', async () => {
  expect(await renderPropsFile('0.0.0-canary.100')).toMatchSnapshot();
});

test('0.67.0-preview.1', async () => {
  expect(await renderPropsFile('0.67.0-preview.1')).toMatchSnapshot();
});

test('0.66.5', async () => {
  expect(await renderPropsFile('0.66.5')).toMatchSnapshot();
});
