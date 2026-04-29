/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {dumpVisualTree} from '@react-native-windows/automation-commands';
import {goToApiExample} from './RNTesterNavigation';
import {verifyNoErrorLogs} from './Helpers';
import {app} from '@react-native-windows/automation';

beforeAll(async () => {
  // If window is partially offscreen, tests will fail to click on certain elements
  await app.setWindowPosition(0, 0);
  await app.setWindowSize(1000, 1250);
  await goToApiExample('Pointer Button');
});

afterEach(async () => {
  await verifyNoErrorLogs();
});

const searchBox = async (input: string) => {
  const searchBox = await app.findElementByTestID('example_search');
  await app.waitUntil(
    async () => {
      // Clear before each attempt: WinAppDriver's setValue can fall back to
      // synthesized keystrokes for custom RN TextInputs, which append rather
      // than replace. Without the clear, a retry produces concatenated text
      // and the comparison never converges.
      await searchBox.clearValue();
      await searchBox.setValue(input);
      if (input === '') {
        return (await searchBox.getText()) === 'Search...';
      } else {
        return (await searchBox.getText()) === input;
      }
    },
    {
      interval: 500,
      timeout: 10000,
      timeoutMsg: `Unable to enter correct search text into test searchbox.`,
    },
  );
};

describe('Pointer Button Tests', () => {
  test('onPointerDown reports correct button property on left click', async () => {
    await searchBox('onPointerDown');
    const component = await app.findElementByTestID('pointer-button-target');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('pointer-button-target');
    expect(dump).toMatchSnapshot();

    // Left click triggers onPointerDown with button=0
    await component.click();
    const stateText = await app.findElementByTestID('pointer-button-state');

    await app.waitUntil(
      async () => {
        const currentText = await stateText.getText();
        return currentText.includes('button=0');
      },
      {
        timeout: 5000,
        timeoutMsg:
          'State text not updated after onPointerDown with button property.',
      },
    );

    const text = await stateText.getText();
    expect(text).toContain('PointerDown');
    expect(text).toContain('button=0');
    expect(text).toContain('buttons=1');
  });
  test('onPointerDown reports correct button property on middle click', async () => {
    await searchBox('onPointerDown');
    const component = await app.findElementByTestID('pointer-button-target');
    await component.waitForDisplayed({timeout: 5000});

    // Middle click triggers onPointerDown with button=1
    await component.click({button: 'middle'});
    const stateText = await app.findElementByTestID('pointer-button-state');

    await app.waitUntil(
      async () => {
        const currentText = await stateText.getText();
        return currentText.includes('button=1');
      },
      {
        timeout: 5000,
        timeoutMsg:
          'State text not updated after onPointerDown with middle button property.',
      },
    );

    const text = await stateText.getText();
    expect(text).toContain('PointerDown');
    expect(text).toContain('button=1');
    expect(text).toContain('buttons=4');
  });
  test('onPointerDown reports correct button property on right click', async () => {
    await searchBox('onPointerDown');
    const component = await app.findElementByTestID('pointer-button-target');
    await component.waitForDisplayed({timeout: 5000});

    // Right click triggers onPointerDown with button=2
    await component.click({button: 'right'});
    const stateText = await app.findElementByTestID('pointer-button-state');

    await app.waitUntil(
      async () => {
        const currentText = await stateText.getText();
        return currentText.includes('button=2');
      },
      {
        timeout: 5000,
        timeoutMsg:
          'State text not updated after onPointerDown with right button property.',
      },
    );

    const text = await stateText.getText();
    expect(text).toContain('PointerDown');
    expect(text).toContain('button=2');
    expect(text).toContain('buttons=2');
  });
  test('onPointerUp reports correct button property on left click', async () => {
    await searchBox('onPointerUp');
    const component = await app.findElementByTestID('pointer-up-button-target');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('pointer-up-button-target');
    expect(dump).toMatchSnapshot();

    // Left click release triggers onPointerUp with button=0
    await component.click();
    const stateText = await app.findElementByTestID('pointer-up-button-state');

    await app.waitUntil(
      async () => {
        const currentText = await stateText.getText();
        return currentText.includes('button=0');
      },
      {
        timeout: 5000,
        timeoutMsg:
          'State text not updated after onPointerUp with button property.',
      },
    );

    const text = await stateText.getText();
    expect(text).toContain('PointerUp');
    expect(text).toContain('button=0');
    expect(text).toContain('buttons=0');
  });
  test('onPointerUp reports correct button property on middle click', async () => {
    await searchBox('onPointerUp');
    const component = await app.findElementByTestID('pointer-up-button-target');
    await component.waitForDisplayed({timeout: 5000});

    // Middle click release triggers onPointerUp with button=1
    await component.click({button: 'middle'});
    const stateText = await app.findElementByTestID('pointer-up-button-state');

    await app.waitUntil(
      async () => {
        const currentText = await stateText.getText();
        return currentText.includes('button=1');
      },
      {
        timeout: 5000,
        timeoutMsg:
          'State text not updated after onPointerUp with middle button property.',
      },
    );

    const text = await stateText.getText();
    expect(text).toContain('PointerUp');
    expect(text).toContain('button=1');
    expect(text).toContain('buttons=0');
  });
  test('onPointerUp reports correct button property on right click', async () => {
    await searchBox('onPointerUp');
    const component = await app.findElementByTestID('pointer-up-button-target');
    await component.waitForDisplayed({timeout: 5000});

    // Right click release triggers onPointerUp with button=2
    await component.click({button: 'right'});
    const stateText = await app.findElementByTestID('pointer-up-button-state');

    await app.waitUntil(
      async () => {
        const currentText = await stateText.getText();
        return currentText.includes('button=2');
      },
      {
        timeout: 5000,
        timeoutMsg:
          'State text not updated after onPointerUp with right button property.',
      },
    );

    const text = await stateText.getText();
    expect(text).toContain('PointerUp');
    expect(text).toContain('button=2');
    expect(text).toContain('buttons=0');
  });
});
