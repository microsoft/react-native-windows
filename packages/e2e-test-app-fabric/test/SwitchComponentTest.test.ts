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

const searchBox = async (input: string) => {
  const searchBox = await app.findElementByTestID('example_search');
  await app.waitUntil(
    async () => {
      await searchBox.setValue(input);
      return (await searchBox.getText()) === input;
    },
    {
      interval: 1500,
      timeout: 5000,
      timeoutMsg: `Unable to enter correct search text into test searchbox.`,
    },
  );
};

beforeAll(async () => {
  // If window is partially offscreen, tests will fail to click on certain elements
  await app.setWindowPosition(0, 0);
  await app.setWindowSize(1000, 1250);
  await goToComponentExample('Switch');
});

afterEach(async () => {
  await verifyNoErrorLogs();
});

describe('SwitchTests', () => {
  test('Switches can be set to true/false, initial false', async () => {
    await searchBox('tru');
    const component = await app.findElementByTestID('on-off-initial-off');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('on-off-initial-off');
    expect(dump).toMatchSnapshot();
    await component.click();
    const dump2 = await dumpVisualTree('on-off-initial-off');
    expect(dump2).toMatchSnapshot();
  });

  test('Switches can be set to true/false, initial true', async () => {
    await searchBox('tru');
    const dump = await dumpVisualTree('on-off-initial-on');
    expect(dump).toMatchSnapshot();
  });

  test('Switches can be disabled, initial false', async () => {
    await searchBox('dis');
    const component = await app.findElementByTestID('disabled-initial-off');
    await component.waitForDisplayed({timeout: 5000});
    const dump = await dumpVisualTree('disabled-initial-off');
    expect(dump).toMatchSnapshot();
    await component.click();
    const dump2 = await dumpVisualTree('disabled-initial-off');
    expect(dump2).toMatchSnapshot();
  });

  test('Switches can be disabled, initial true', async () => {
    await searchBox('dis');
    const dump = await dumpVisualTree('disabled-initial-on');
    expect(dump).toMatchSnapshot();
  });

  test('Custom colors can be provided, initial false', async () => {
    await searchBox('cus');
    const dump = await dumpVisualTree('initial-false-switch');
    expect(dump).toMatchSnapshot();
  });

  test('Custom colors can be provided, initial true', async () => {
    await searchBox('cus');
    const dump = await dumpVisualTree('initial-true-switch');
    expect(dump).toMatchSnapshot();
  });

  test('Change events can be detected, event top', async () => {
    await searchBox('cha');
    const dump = await dumpVisualTree('event-switch-top');
    expect(dump).toMatchSnapshot();
  });

  test('Change events can be detected, event bottom', async () => {
    await searchBox('cha');
    const dump = await dumpVisualTree('event-switch-bottom');
    expect(dump).toMatchSnapshot();
  });

  test('Change events can be detected, event regression top', async () => {
    await searchBox('cha');
    const dump = await dumpVisualTree('event-switch-regression-top');
    expect(dump).toMatchSnapshot();
  });

  test('Change events can be detected, events regression bottom', async () => {
    await searchBox('cha');
    const dump = await dumpVisualTree('event-switch-regression-bottom');
    expect(dump).toMatchSnapshot();
  });
  test('Controlled switch', async () => {
    await searchBox('con');
    const dump = await dumpVisualTree('controlled-switch');
    expect(dump).toMatchSnapshot();
  });
  test('Switches can have customized accessibility', async () => {
    await searchBox('acc');
    const dump = await dumpVisualTree('accessibility-switch');
    expect(dump).toMatchSnapshot();
  });
});
