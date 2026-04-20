/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

// import { AutomationClient } from '@react-native-windows/automation-channel';

// declare global {
//   const automationClient: AutomationClient | undefined;
// }

/**
 * Dump a section of the native visual tree.
 */
export default async function createScreenshot(
  opts?: {
    accessibilityId: string,
    location?: { x: number, y: number, width: number, height: number };
  },
): Promise<void> {
  if (!automationClient) {
    throw new Error('RPC client is not enabled');
  }

  const dumpResponse = await automationClient.invoke('CreateScreenshot', {
    ...opts,
  });

  if (dumpResponse.type === 'error') {
    throw new Error(dumpResponse.message);
  }
}
