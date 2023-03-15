/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {dumpVisualTree} from '@react-native-windows/automation-commands';
import {goToComponentExample} from './RNTesterNavigation';

beforeAll(async () => {
  await goToComponentExample('Button');
});

describe('ButtonTests', () => {
  test('Buttons have default styling', async () => {
    const dump = await dumpVisualTree('button_default_styling');
    expect(dump).toMatchSnapshot();
  });
  test('Buttons can have custom colors', async () => {
    const dump = await dumpVisualTree('cancel_button');
    expect(dump).toMatchSnapshot();
  });
  test('Buttons can be disabled', async () => {
    const dump = await dumpVisualTree('disabled_button');
    expect(dump).toMatchSnapshot();
  });
  test('Buttons can have accessibility labels', async () => {
    const dump = await dumpVisualTree('accessibilityLabel_button');
    expect(dump).toMatchSnapshot();
  });
  test('Buttons can have accessibility states', async () => {
    const dump = await dumpVisualTree('accessibilityState_button');
    expect(dump).toMatchSnapshot();
  });
  test('Buttons can have accessibility props', async () => {
    const dump = await dumpVisualTree('accessibility_props');
    expect(dump).toMatchSnapshot();
  });
  test('Buttons can have their accessibility and keyboard focus disabled', async () => {
    const dump = await dumpVisualTree('accessible_focusable_button');
    expect(dump).toMatchSnapshot();
  });
});
