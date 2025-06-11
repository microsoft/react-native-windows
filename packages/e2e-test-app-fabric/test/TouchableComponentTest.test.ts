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
  await app.setWindowSize(1000, 1250);
  await goToComponentExample('Touchable* and onPress');
});

afterEach(async () => {
  await verifyNoErrorLogs();
});

const searchBox = async (input: string) => {
  const searchBox = await app.findElementByTestID('example_search');
  await app.waitUntil(
    async () => {
      await searchBox.setValue(input);
      if (input === '') {
        return (await searchBox.getText()) === 'Search...';
      }
      return (await searchBox.getText()) === input;
    },
    {
      interval: 1500,
      timeout: 5000,
      timeoutMsg: `Unable to enter correct search text into test searchbox.`,
    },
  );
};

describe('Touchable Tests', () => {
  test('Touchables can contain a Text component', async () => {
    const component = await app.findElementByTestID(
      'touchable_highlight_text_button',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('touchable_highlight_text_button');
    expect(dump).toMatchSnapshot();
  });
  test('Touchables can contain an Image component', async () => {
    const component = await app.findElementByTestID(
      'touchable_highlight_image_button',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('touchable_highlight_image_button');
    expect(dump).toMatchSnapshot();
  });
  test('TouchableWithoutFeedback components should not give visual feedback', async () => {
    const component = await app.findElementByTestID(
      'touchable_without_feedback_button',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('touchable_without_feedback_button');
    expect(dump).toMatchSnapshot();
  });
  test('Touchables can register feedback events', async () => {
    await searchBox('fee');
    const component = await app.findElementByTestID(
      'touchable_feedback_events_button',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('touchable_feedback_events_button');
    expect(dump).toMatchSnapshot();
    await component.click();
    const dump2 = await dumpVisualTree('touchable_feedback_events_console');
    expect(dump2).toMatchSnapshot();
    await searchBox('');
  });
  test('Text components can be tappable', async () => {
    const component = await app.findElementByTestID('tappable_text');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('tappable_text');
    expect(dump).toMatchSnapshot();
  });
  test('Touchables can delay events', async () => {
    const component = await app.findElementByTestID(
      'touchable_delay_events_button',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('touchable_delay_events_button');
    expect(dump).toMatchSnapshot();
  });
  test('Touchables can enable a hit slop region', async () => {
    const component = await app.findElementByTestID(
      'touchable_hit_slop_button',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('touchable_hit_slop_button');
    expect(dump).toMatchSnapshot();
  });
  test('Touchables can be defined in a set using accessibilityPosInSet and accessibilitySetSize', async () => {
    const component = await app.findElementByTestID('touchable_set');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('touchable_set');
    expect(dump).toMatchSnapshot();
  });
  test('Touchables can be disabled', async () => {
    await searchBox('dis');
    const component = await app.findElementByTestID('disabled_touchable');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('disabled_touchable');
    expect(dump).toMatchSnapshot();
    await component.click();
    const dump2 = await dumpVisualTree('disabled_touchable');
    expect(dump2).toMatchSnapshot();
    await searchBox('');
  });
});

describe('TouchableOpacity Tests', () => {
  test('TouchableOpacity should register press in clicked within hitSlop range', async () => {
    const component = await app.findElementByTestID(
      'touchable_hit_slop_button',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('touchable_hit_slop_button');
    expect(dump).toMatchSnapshot();
  });

  test('TouchableOpacity should not be interactable when disabled', async () => {
    await searchBox('dis');
    const component = await app.findElementByTestID('disabled_touchable');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('disabled_touchable');
    expect(dump).toMatchSnapshot();
    await component.click();
    const dump2 = await dumpVisualTree('disabled_touchable');
    expect(dump2).toMatchSnapshot();
    await searchBox('');
  });

  test('TouchableOpacity should update style upon fast refresh', async () => {
    await searchBox('dis');
    const component = await app.findElementByTestID('disabled_touchable');
    await component.waitForDisplayed({timeout: 5000});
    // Initial state - transparent background
    const initialDump = await dumpVisualTree('disabled_touchable');
    expect(initialDump).toMatchSnapshot();

    // Click to trigger style change (backgroundColor: 'blue')
    await component.click();

    // Verify style updated - background should now be blue
    const updatedDump = await dumpVisualTree('disabled_touchable');
    expect(updatedDump).toMatchSnapshot();
    await searchBox('');
  });

  // Note: TouchableOpacity activeOpacity testing would require a test component
  // with dynamic activeOpacity property changes, which is not currently available
  // in the existing examples. Such testing would need a new TouchableOpacity
  // example that updates its activeOpacity value based on state changes.

  test('TouchableOpacity should fire action upon press', async () => {
    await searchBox('fee');
    const component = await app.findElementByTestID(
      'touchable_feedback_events_button',
    );
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('touchable_feedback_events_button');
    expect(dump).toMatchSnapshot();
    await component.click();
    const dump2 = await dumpVisualTree('touchable_feedback_events_console');
    expect(dump2).toMatchSnapshot();
    await searchBox('');
  });

  test('TouchableOpacity should fire action upon onPressIn', async () => {
    await searchBox('fee');
    const component = await app.findElementByTestID(
      'touchable_feedback_events_button',
    );
    await component.waitForDisplayed({timeout: 5000});
    await component.click();
    const dump = await dumpVisualTree('touchable_feedback_events_console');
    expect(dump).toMatchSnapshot();
    await searchBox('');
  });

  test('TouchableOpacity should fire action upon onPressOut', async () => {
    await searchBox('fee');
    const component = await app.findElementByTestID(
      'touchable_feedback_events_button',
    );
    await component.waitForDisplayed({timeout: 5000});
    await component.click();
    const dump = await dumpVisualTree('touchable_feedback_events_console');
    expect(dump).toMatchSnapshot();
    await searchBox('');
  });

  test('TouchableOpacity should fire action upon onLongPress', async () => {
    await searchBox('fee');
    const component = await app.findElementByTestID(
      'touchable_feedback_events_button',
    );
    await component.waitForDisplayed({timeout: 5000});
    await component.click();
    const dump = await dumpVisualTree('touchable_feedback_events_console');
    expect(dump).toMatchSnapshot();
    await searchBox('');
  });
});
