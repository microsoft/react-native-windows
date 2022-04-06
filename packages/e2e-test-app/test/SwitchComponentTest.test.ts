/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {dumpVisualTree} from '@react-native-windows/automation-commands';
import {goToComponentExample} from './RNTesterNavigation';

beforeAll(async () => {
  await goToComponentExample('Switch');
});

describe('SwitchTests', () => {
  test('Switches can be set to true/false, initial false', async () => {
    const dump = await dumpVisualTree('on-off-initial-off');
    expect(dump).toMatchSnapshot();
  });

  test('Switches can be set to true/false, initial true', async () => {
    const dump = await dumpVisualTree('on-off-initial-on');
    expect(dump).toMatchSnapshot();
  });

  test('Switches can be disabled, initial false', async () => {
    const dump = await dumpVisualTree('disabled-initial-off');
    expect(dump).toMatchSnapshot();
  });

  test('Switches can be disabled, initial true', async () => {
    const dump = await dumpVisualTree('disabled-initial-on');
    expect(dump).toMatchSnapshot();
  });

  test('Custom colors can be provided, initial false', async () => {
    const dump = await dumpVisualTree('initial-false-switch');
    expect(dump).toMatchSnapshot();
  });

  test('Custom colors can be provided, initial true', async () => {
    const dump = await dumpVisualTree('initial-true-switch');
    expect(dump).toMatchSnapshot();
  });

  test('Change events can be detected, event top', async () => {
    const dump = await dumpVisualTree('event-switch-top');
    expect(dump).toMatchSnapshot();
  });

  test('Change events can be detected, event bottom', async () => {
    const dump = await dumpVisualTree('event-switch-bottom');
    expect(dump).toMatchSnapshot();
  });

  test('Change events can be detected, event regression top', async () => {
    const dump = await dumpVisualTree('event-switch-regression-top');
    expect(dump).toMatchSnapshot();
  });

  test('Change events can be detected, events regression bottom', async () => {
    const dump = await dumpVisualTree('event-switch-regression-bottom');
    expect(dump).toMatchSnapshot();
  });
  test('Controlled switch', async () => {
    const dump = await dumpVisualTree('controlled-switch');
    expect(dump).toMatchSnapshot();
  });
});
