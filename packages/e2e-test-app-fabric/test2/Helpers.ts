/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {AutomationClient} from '@react-native-windows/automation-channel';

declare global {
  const automationClient: AutomationClient | undefined;
}

type ErrorsResult = {
  errors: string[];
};

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
