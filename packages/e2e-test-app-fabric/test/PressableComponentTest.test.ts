/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {dumpVisualTree} from '@react-native-windows/automation-commands';
import {goToComponentExample} from './RNTesterNavigation';
import {verifyNoErrorLogs} from './Helpers';
import {app} from '@react-native-windows/automation';

beforeAll(async () => {
  // If window is partially offscreen, tests will fail to click on certain elements
  await app.setWindowPosition(0, 0);
  await goToComponentExample('Pressable');
});

afterEach(async () => {
  await verifyNoErrorLogs();
});

describe('Pressable Tests', () => {
  test('Pressables can change text on press/rest, state rest', async () => {
    const component = await app.findElementByTestID('pressable_press_console');
    await component.waitForDisplayed({timeout: 20000});
    const dump = await dumpVisualTree('pressable_press_console');
    expect(dump).toMatchSnapshot();
  });
  test('Pressables can have event handlers', async () => {
    const component = await app.findElementByTestID(
      'pressable_feedback_events_button',
    );
    await component.waitForDisplayed({timeout: 20000});
    const dump = await dumpVisualTree('pressable_feedback_events_button');
    expect(dump).toMatchSnapshot();
  });
  test('Pressables can have delayed event handlers', async () => {
    const component = await app.findElementByTestID(
      'pressable_delay_events_button',
    );
    await component.waitForDisplayed({timeout: 20000});
    const dump = await dumpVisualTree('pressable_delay_events_button');
    expect(dump).toMatchSnapshot();
  });
  test('Pressables can have hit slop functionality', async () => {
    const component = await app.findElementByTestID(
      'pressable_hit_slop_button',
    );
    await component.waitForDisplayed({timeout: 20000});
    const dump = await dumpVisualTree('pressable_hit_slop_button');
    expect(dump).toMatchSnapshot();
  });
  // Disable tests which don't exist upstream. TestId cannot be found.
  /*test('Pressables can have ranging opacity', async () => {
    const component = await app.findElementByTestID('opacity_pressable');
    await component.waitForDisplayed({timeout: 20000});
    const dump = await dumpVisualTree('opacity_pressable');
    expect(dump).toMatchSnapshot();
  });
  test('Pressables can have their accessibility and keyboard focus removed', async () => {
    const component = await app.findElementByTestID('accessible_pressable');
    await component.waitForDisplayed({timeout: 20000});
    const dump = await dumpVisualTree('accessible_pressable');
    expect(dump).toMatchSnapshot();
  });
  test('Pressables can have tooltips', async () => {
    const component = await app.findElementByTestID('tooltip_pressable');
    await component.waitForDisplayed({timeout: 20000});
    const dump = await dumpVisualTree('tooltip_pressable');
    expect(dump).toMatchSnapshot();
  });
  test('Pressables can hide their children from accessibility focus', async () => {
    const component = await app.findElementByTestID('no_hide_pressable');
    await component.waitForDisplayed({timeout: 20000});
    const dump = await dumpVisualTree('no_hide_pressable');
    expect(dump).toMatchSnapshot();
  });
  test('Pressables can hide their backface', async () => {
    const component = await app.findElementByTestID('backface_pressable');
    await component.waitForDisplayed({timeout: 20000});
    const dump = await dumpVisualTree('backface_pressable');
    expect(dump).toMatchSnapshot();
  });*/
});
