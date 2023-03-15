/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {dumpVisualTree} from '@react-native-windows/automation-commands';
import {goToComponentExample} from './RNTesterNavigation';

beforeAll(async () => {
  await goToComponentExample('Pressable');
});

describe('PressableTests', () => {
  test('Pressables can change text on press/rest, state rest', async () => {
    const dump = await dumpVisualTree('pressable_press_console');
    expect(dump).toMatchSnapshot();
  });
  test('Pressables can have event handlers', async () => {
    const dump = await dumpVisualTree('pressable_feedback_events_button');
    expect(dump).toMatchSnapshot();
  });
  test('Pressables can have delayed event handlers', async () => {
    const dump = await dumpVisualTree('pressable_delay_events_button');
    expect(dump).toMatchSnapshot();
  });
  test('Pressables can have hit slop functionality', async () => {
    const dump = await dumpVisualTree('pressable_hit_slop_button');
    expect(dump).toMatchSnapshot();
  });
  test('Pressables can have customized borders and corners', async () => {
    const dump = await dumpVisualTree('advanced_borders_pressable');
    expect(dump).toMatchSnapshot();
  });
  test('Pressables can have ranging opacity', async () => {
    const dump = await dumpVisualTree('opacity_pressable');
    expect(dump).toMatchSnapshot();
  });
  test('Pressables can have their accessibility and keyboard focus removed', async () => {
    const dump = await dumpVisualTree('accessible_pressable');
    expect(dump).toMatchSnapshot();
  });
  test('Pressables can have tooltips', async () => {
    const dump = await dumpVisualTree('tooltip_pressable');
    expect(dump).toMatchSnapshot();
  });
  test('Pressables can hide their children from accessibility focus', async () => {
    const dump = await dumpVisualTree('no_hide_pressable');
    expect(dump).toMatchSnapshot();
  });
  test('Pressables can hide their backface', async () => {
    const dump = await dumpVisualTree('backface_pressable');
    expect(dump).toMatchSnapshot();
  });
});
