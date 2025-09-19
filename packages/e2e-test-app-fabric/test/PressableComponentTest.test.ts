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
  test('Pressables can have event handlers', async () => {
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
  test('Pressables can have advanced borders', async () => {
    const component = await app.findElementByTestID(
      'advanced_borders_pressable',
    );
    await component.waitForDisplayed({timeout: 20000});
    const dump = await dumpVisualTree('pressable_hit_slop_button');
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
});
