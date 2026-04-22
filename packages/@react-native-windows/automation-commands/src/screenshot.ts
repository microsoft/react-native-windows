/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

/**
 * Take a screenshot of the app window
 */
export default async function createScreenshot(
  opts?: {
    accessibilityId: string,
    screenshotsPath?: string,
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
