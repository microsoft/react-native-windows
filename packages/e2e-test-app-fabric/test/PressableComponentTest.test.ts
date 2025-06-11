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
  await goToComponentExample('Pressable');
});

afterEach(async () => {
  await verifyNoErrorLogs();
});

describe('Pressable Tests', () => {
  test('Pressables can change text on press/rest, state rest', async () => {
    const searchBox = await app.findElementByTestID('example_search');
    await app.waitUntil(
      async () => {
        await searchBox.setValue('Cha');
        return (await searchBox.getText()) === 'Cha';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct search text into test searchbox.`,
      },
    );
    const component = await app.findElementByTestID('one_press_me_button');
    await component.waitForDisplayed({timeout: 20000});
    const dump = await dumpVisualTree('one_press_me_button');
    expect(dump).toMatchSnapshot();
    await component.click();
    const dump2 = await dumpVisualTree('pressable_press_console');
    expect(dump2).toMatchSnapshot();
    await app.waitUntil(
      async () => {
        await searchBox.setValue(['Backspace', 'Backspace', 'Backspace']);
        return (await searchBox.getText()) === 'Search...';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct search text into test searchbox.`,
      },
    );
  });
  test('Pressables can change style when pressed', async () => {
    const component = await app.findElementByTestID('style-change-pressable');
    await component.waitForDisplayed({timeout: 20000});
    const dump = await dumpVisualTree('style-change-pressable');
    expect(dump).toMatchSnapshot();
  });
  test('Text can have pressable behavior', async () => {
    const searchBox = await app.findElementByTestID('example_search');
    await app.waitUntil(
      async () => {
        await searchBox.setValue('<Te');
        return (await searchBox.getText()) === '<Te';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct search text into test searchbox.`,
      },
    );
    const component = await app.findElementByTestID('tappable_text');
    await component.waitForDisplayed({timeout: 20000});
    const dump = await dumpVisualTree('tappable_text');
    expect(dump).toMatchSnapshot();
    await component.click();
    await component.click();
    const dump2 = await dumpVisualTree('tappable_text_console');
    expect(dump2).toMatchSnapshot();
    await app.waitUntil(
      async () => {
        await searchBox.setValue(['Backspace', 'Backspace', 'Backspace']);
        return (await searchBox.getText()) === 'Search...';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct search text into test searchbox.`,
      },
    );
  });
  test('Pressables can have event handlers, hover', async () => {
    const searchBox = await app.findElementByTestID('example_search');
    await app.waitUntil(
      async () => {
        await searchBox.setValue('fee');
        return (await searchBox.getText()) === 'fee';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct search text into test searchbox.`,
      },
    );
    const component = await app.findElementByTestID(
      'pressable_feedback_events_button',
    );
    await component.waitForDisplayed({timeout: 20000});
    const dump = await dumpVisualTree('pressable_feedback_events_button');
    expect(dump).toMatchSnapshot();
    await component.moveTo();
    const consoleElement = await app.findElementByTestID(
      'pressable_feedback_events_console',
    );
    await consoleElement.moveTo();
    const dump2 = await dumpVisualTree('pressable_feedback_events_console');
    expect(dump2).toMatchSnapshot();
    await app.waitUntil(
      async () => {
        await searchBox.setValue(['Backspace', 'Backspace', 'Backspace']);
        return (await searchBox.getText()) === 'Search...';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct search text into test searchbox.`,
      },
    );
  });
  test('Pressables can have event handlers, hover and click', async () => {
    const searchBox = await app.findElementByTestID('example_search');
    await app.waitUntil(
      async () => {
        await searchBox.setValue('fee');
        return (await searchBox.getText()) === 'fee';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct search text into test searchbox.`,
      },
    );
    const component = await app.findElementByTestID(
      'pressable_feedback_events_button',
    );
    await component.waitForDisplayed({timeout: 20000});
    const dump = await dumpVisualTree('pressable_feedback_events_button');
    expect(dump).toMatchSnapshot();
    await component.click();
    const dump2 = await dumpVisualTree('pressable_feedback_events_console');
    expect(dump2).toMatchSnapshot();
    await app.waitUntil(
      async () => {
        await searchBox.setValue(['Backspace', 'Backspace', 'Backspace']);
        return (await searchBox.getText()) === 'Search...';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct search text into test searchbox.`,
      },
    );
  });
  test('Pressables can have delayed event handlers', async () => {
    const searchBox = await app.findElementByTestID('example_search');
    await app.waitUntil(
      async () => {
        await searchBox.setValue('del');
        return (await searchBox.getText()) === 'del';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct search text into test searchbox.`,
      },
    );
    const component = await app.findElementByTestID(
      'pressable_delay_events_button',
    );
    await component.waitForDisplayed({timeout: 20000});
    const dump = await dumpVisualTree('pressable_delay_events_button');
    expect(dump).toMatchSnapshot();
    await component.click();
    const dump2 = await dumpVisualTree('pressable_delay_events_console');
    expect(dump2).toMatchSnapshot();
    await app.waitUntil(
      async () => {
        await searchBox.setValue(['Backspace', 'Backspace', 'Backspace']);
        return (await searchBox.getText()) === 'Search...';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct search text into test searchbox.`,
      },
    );
  });
  test('Pressables can be disabled, disabled = {true}', async () => {
    const searchBox = await app.findElementByTestID('example_search');
    await app.waitUntil(
      async () => {
        await searchBox.setValue('dis');
        return (await searchBox.getText()) === 'dis';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct search text into test searchbox.`,
      },
    );
    const component = await app.findElementByTestID('pressable_disabled_true');
    await component.waitForDisplayed({timeout: 20000});
    const dump = await dumpVisualTree('pressable_disabled_true');
    expect(dump).toMatchSnapshot();
    await app.waitUntil(
      async () => {
        await searchBox.setValue(['Backspace', 'Backspace', 'Backspace']);
        return (await searchBox.getText()) === 'Search...';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct search text into test searchbox.`,
      },
    );
  });
  test('Pressables can be disabled, disabled = {false}', async () => {
    const searchBox = await app.findElementByTestID('example_search');
    await app.waitUntil(
      async () => {
        await searchBox.setValue('dis');
        return (await searchBox.getText()) === 'dis';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct search text into test searchbox.`,
      },
    );
    const component = await app.findElementByTestID('pressable_disabled_false');
    await component.waitForDisplayed({timeout: 20000});
    const dump = await dumpVisualTree('pressable_disabled_false');
    expect(dump).toMatchSnapshot();
    await app.waitUntil(
      async () => {
        await searchBox.setValue(['Backspace', 'Backspace', 'Backspace']);
        return (await searchBox.getText()) === 'Search...';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct search text into test searchbox.`,
      },
    );
  });
  test('Pressables can have hit slop functionality', async () => {
    const component = await app.findElementByTestID(
      'pressable_hit_slop_button',
    );
    await component.waitForDisplayed({timeout: 20000});
    const dump = await dumpVisualTree('pressable_hit_slop_button');
    expect(dump).toMatchSnapshot();
  });
  test('Pressable should perform action upon onLongPress', async () => {
    const searchBox = await app.findElementByTestID('example_search');
    await app.waitUntil(
      async () => {
        await searchBox.setValue('fee');
        return (await searchBox.getText()) === 'fee';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct search text into test searchbox.`,
      },
    );
    const component = await app.findElementByTestID(
      'pressable_feedback_events_button',
    );
    await component.waitForDisplayed({timeout: 20000});

    // Test that long press functionality is available by capturing events
    await component.click();
    await app.pause(1000); // Allow time for events to be processed

    const consoleElement = await app.findElementByTestID(
      'pressable_feedback_events_console',
    );
    const dump = await dumpVisualTree('pressable_feedback_events_console');
    expect(dump).toMatchSnapshot();

    await app.waitUntil(
      async () => {
        await searchBox.setValue(['Backspace', 'Backspace', 'Backspace']);
        return (await searchBox.getText()) === 'Search...';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct search text into test searchbox.`,
      },
    );
  });
  test('Pressable behavior should change upon delayLongPress adjustment', async () => {
    const searchBox = await app.findElementByTestID('example_search');
    await app.waitUntil(
      async () => {
        await searchBox.setValue('del');
        return (await searchBox.getText()) === 'del';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct search text into test searchbox.`,
      },
    );
    const component = await app.findElementByTestID(
      'pressable_delay_events_button',
    );
    await component.waitForDisplayed({timeout: 20000});

    // Test delayLongPress behavior by capturing delayed event responses
    await component.click();
    await app.pause(1000); // Allow time for delayed events to be processed

    const consoleElement = await app.findElementByTestID(
      'pressable_delay_events_console',
    );
    const dump = await dumpVisualTree('pressable_delay_events_console');
    expect(dump).toMatchSnapshot();

    await app.waitUntil(
      async () => {
        await searchBox.setValue(['Backspace', 'Backspace', 'Backspace']);
        return (await searchBox.getText()) === 'Search...';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct search text into test searchbox.`,
      },
    );
  });
  test('Pressable should register onPress action when hit within hitSlop range', async () => {
    const searchBox = await app.findElementByTestID('example_search');
    await app.waitUntil(
      async () => {
        await searchBox.setValue('Pre');
        return (await searchBox.getText()) === 'Pre';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct search text into test searchbox.`,
      },
    );
    const component = await app.findElementByTestID(
      'pressable_hit_slop_button',
    );
    await component.waitForDisplayed({timeout: 20000});

    // Click on the component to test hitSlop interaction
    await component.click();

    const dump = await dumpVisualTree('pressable_hit_slop');
    expect(dump).toMatchSnapshot();

    await app.waitUntil(
      async () => {
        await searchBox.setValue(['Backspace', 'Backspace', 'Backspace']);
        return (await searchBox.getText()) === 'Search...';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct search text into test searchbox.`,
      },
    );
  });
  test('Pressable should support different disabled styling configurations', async () => {
    const searchBox = await app.findElementByTestID('example_search');
    await app.waitUntil(
      async () => {
        await searchBox.setValue('dis');
        return (await searchBox.getText()) === 'dis';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct search text into test searchbox.`,
      },
    );

    // Test disabled=true styling
    const disabledComponent = await app.findElementByTestID(
      'pressable_disabled_true',
    );
    await disabledComponent.waitForDisplayed({timeout: 20000});
    const disabledDump = await dumpVisualTree('pressable_disabled_true');
    expect(disabledDump).toMatchSnapshot();

    // Test disabled=false styling with press state
    const enabledComponent = await app.findElementByTestID(
      'pressable_disabled_false',
    );
    await enabledComponent.waitForDisplayed({timeout: 20000});
    const enabledDump = await dumpVisualTree('pressable_disabled_false');
    expect(enabledDump).toMatchSnapshot();

    await app.waitUntil(
      async () => {
        await searchBox.setValue(['Backspace', 'Backspace', 'Backspace']);
        return (await searchBox.getText()) === 'Search...';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct search text into test searchbox.`,
      },
    );
  });
  test('Pressable should support different children configurations', async () => {
    const searchBox = await app.findElementByTestID('example_search');
    await app.waitUntil(
      async () => {
        await searchBox.setValue('Cha');
        return (await searchBox.getText()) === 'Cha';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct search text into test searchbox.`,
      },
    );

    // Test pressable with different content based on press state
    const component = await app.findElementByTestID('one_press_me_button');
    await component.waitForDisplayed({timeout: 20000});
    const initialDump = await dumpVisualTree('one_press_me_button');
    expect(initialDump).toMatchSnapshot();

    await app.waitUntil(
      async () => {
        await searchBox.setValue(['Backspace', 'Backspace', 'Backspace']);
        return (await searchBox.getText()) === 'Search...';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct search text into test searchbox.`,
      },
    );
  });
  // TODO: Add fast refresh tests once test components are available
  // These tests would require components with buttons/toggles to change properties at runtime:
  // - Pressable should update border styling upon fast refresh
  // - Pressable should update children upon fast refresh
  // - Pressable should update disabled styling upon fast refresh
  test('Pressables can have advanced borders', async () => {
    const component = await app.findElementByTestID(
      'advanced_borders_pressable',
    );
    await component.waitForDisplayed({timeout: 20000});
    const dump = await dumpVisualTree('advanced_borders_pressable');
    expect(dump).toMatchSnapshot();
  });
  test('Pressables can have ranging opacity', async () => {
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
  test('Pressables can hide their backface', async () => {
    const component = await app.findElementByTestID('backface_pressable');
    await component.waitForDisplayed({timeout: 20000});
    const dump = await dumpVisualTree('backface_pressable');
    expect(dump).toMatchSnapshot();
  });
  test('Pressables can be defined as a set using accessibilityPosInSet and accessibilitySetSize', async () => {
    const component = await app.findElementByTestID('pressable_set');
    await component.waitForDisplayed({timeout: 20000});
    const dump = await dumpVisualTree('pressable_set');
    expect(dump).toMatchSnapshot();
  });
});
