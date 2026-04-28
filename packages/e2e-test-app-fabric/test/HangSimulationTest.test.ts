/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

// Test-only: validates the hang-dump capture path of the E2E pipeline.
//
// Auto-skips unless RNW_SIMULATE_HANG=1, which the pipeline only sets when
// `simulateHangForTesting=true` is passed to .ado/jobs/e2e-test.yml. When
// active, asks the app to jam its UI thread via the `HangForTesting`
// automation command; the post-failure ProcDump step in the pipeline then
// captures a full memory dump of the still-alive packaged-app process.

import {app} from '@react-native-windows/automation';
import {AutomationClient} from '@react-native-windows/automation-channel';

declare const automationClient: AutomationClient | undefined;

const shouldRun = process.env.RNW_SIMULATE_HANG === '1';

(shouldRun ? describe : describe.skip)('Hang Simulation (TEST ONLY)', () => {
  test('jams the UI thread until the test times out', async () => {
    if (!automationClient) {
      throw new Error('RPC client is not enabled');
    }

    // Asks the app to Post a Sleep(INFINITE) onto its UI dispatcher. The
    // command itself returns quickly; the UI thread is jammed on the next
    // queued work item, so any subsequent UIA query will block.
    await automationClient.invoke('HangForTesting', {});

    // Touch the UI to surface the hang to jest. This call would normally
    // return quickly; with the UI thread jammed it blocks until jest's
    // testTimeout fires.
    const anyElement = await app.findElementByTestID('components-tab');
    await anyElement.waitForDisplayed({timeout: 60000});
  });
});

export {};
