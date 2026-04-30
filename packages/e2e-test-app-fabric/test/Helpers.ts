/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import type { AutomationElement } from '@react-native-windows/automation';
import { AutomationClient } from '@react-native-windows/automation-channel';

import { createScreenshot } from '@react-native-windows/automation-commands';
import { tmpdir } from 'os';
import { join } from 'path';
import fs from '@react-native-windows/fs';

declare global {
  const automationClient: AutomationClient | undefined;
}

type ErrorsResult = {
  errors: string[];
};

export interface ImageSnapshotConfig {
  failureThreshold?: number;
  failureThresholdType?: 'percent'
}

export async function verifyElementVisualSnapshot(component: AutomationElement, config?: ImageSnapshotConfig) {
  const { x, y } = await component.getLocation();
  const { width, height } = await component.getSize();
  await createScreenshot({ screenshotsPath: tmpdir(), location: { x, y, width, height } });
  const myFancyImage = fs.readFileSync(join(tmpdir(), "./RNTester.png"));
  expect(myFancyImage).toMatchImageSnapshot({
    failureThreshold: 0.01,
    failureThresholdType: 'percent',
    ...config
  });
}

export async function verifyNoErrorLogs(
  errorFilter?: (errors: string[]) => string[],
): Promise<void> {
  if (!automationClient) {
    throw new Error('RPC client is not enabled');
  }

  const response = await automationClient.invoke('ListErrors', {});

  if (response.type === 'error') {
    throw new Error(response.message);
  }

  const result = response.result as ErrorsResult;

  const errors = errorFilter ? errorFilter(result.errors) : result.errors;
  expect(errors.join('\n')).toBeFalsy();
}
