/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as http from 'http';
import {getDebugTargets, CDPDebugger} from './CDPDebugger';
import {Metro} from './MetroAutomation';
import * as path from 'path';
import {
  PlaygroundDebugSettings,
  loadPackage,
  primePackageComboBox,
} from './PlaygroundAutomation';
import {testLog} from './TestLog';
import {formatDateTime} from './Utilities';
import {pid} from 'process';
import fs from '@react-native-windows/fs';

// jest does not appear to support custom command-line arguments; making do with environment variables.

if (process.env.DEBUGTEST_LOGFOLDER) {
  if (!fs.existsSync(process.env.DEBUGTEST_LOGFOLDER)) {
    (async () => {
      try {
        await fs.mkdir(process.env.DEBUGTEST_LOGFOLDER!);
      } catch (e) {
        console.log(e);
      }
    })();
  }

  testLog.setFileName(
    `${process.env.DEBUGTEST_LOGFOLDER}/${path.basename(
      __filename,
      '.ts',
    )}.${formatDateTime(new Date())}.log`,
  );
} else {
  testLog.setFileName(
    `${process.env.TEMP}/${path.basename(__filename, '.ts')}.${formatDateTime(
      new Date(),
    )}.log`,
  );
}

const metro = new Metro();

beforeAll(async () => {
  testLog.message(`executing beforeAll on PID ${pid}`);
  await metro.start();
  const window = (await $$('./Window'))[0];
  await window.maximizeWindow();
  await primePackageComboBox();
});

afterAll(() => {
  testLog.message(`executing afterAll on PID ${pid}`);
  metro.stop();
});

test('debug target properties', async () => {
  testLog.message(`executing 'debug target properties' on PID ${pid}`);

  const settings = await PlaygroundDebugSettings.set({
    webDebugger: false,
    directDebugging: true,
    jsEngine: 'Hermes',
  });
  try {
    const isBundleServed = metro.isBundleServed('debugTest01');
    await loadPackage('Samples\\debugTest01', isBundleServed);

    const options: http.RequestOptions = {
      hostname: 'localhost',
      port: 8081,
      path: '/json',
      method: 'GET',
    };

    const request = http.request(options);
    const dataPromise = new Promise<Buffer>((resolve, _reject) => {
      request.on('response', response =>
        response.on('data', data => resolve(data)),
      );
    });
    request.end();

    const data = await dataPromise;
    expect(Buffer.isBuffer(data)).toBeTruthy();
    const debugTargets = JSON.parse(data.toString());
    expect(Array.isArray(debugTargets)).toBeTruthy();
    expect(debugTargets.length).toBeGreaterThanOrEqual(1);
    for (const debugTarget of debugTargets) {
      expect(debugTarget.hasOwnProperty('id')).toBeTruthy();
      expect(debugTarget.hasOwnProperty('title')).toBeTruthy();
      expect(debugTarget.hasOwnProperty('devtoolsFrontendUrl')).toBeTruthy();
      expect(debugTarget.hasOwnProperty('type')).toBeTruthy();
      expect(debugTarget.hasOwnProperty('webSocketDebuggerUrl')).toBeTruthy();
      expect(debugTarget.hasOwnProperty('vm')).toBeTruthy();
    }
  } finally {
    await settings.restore();
  }
});

test('enable, disable', async () => {
  testLog.message(`executing 'enable, disable' test on PID ${pid}`);

  const settings = await PlaygroundDebugSettings.set({
    webDebugger: false,
    directDebugging: true,
    jsEngine: 'Hermes',
  });
  try {
    const isBundleServed = metro.isBundleServed('debugTest01');
    await loadPackage('Samples\\debugTest01', isBundleServed);

    const debugTargets = await getDebugTargets();
    const dbg = new CDPDebugger(debugTargets[0].webSocketDebuggerUrl);

    await dbg.debuggerEnable();
    await dbg.debuggerDisable();
    await dbg.checkOutstandingResponses(3000);

    dbg.close();
  } finally {
    await settings.restore();
  }
});

test('pause, resume', async () => {
  testLog.message(`executing 'pause, resume' test on PID ${pid}`);

  const settings = await PlaygroundDebugSettings.set({
    webDebugger: false,
    directDebugging: true,
    jsEngine: 'Hermes',
  });
  try {
    const isBundleServed = metro.isBundleServed('debugTest01');
    await loadPackage('Samples\\debugTest01', isBundleServed);

    const debugTargets = await getDebugTargets();
    const dbg = new CDPDebugger(debugTargets[0].webSocketDebuggerUrl);

    await dbg.debuggerEnable();

    const pausedEvent = dbg.expectEvent('Debugger.paused');
    await dbg.debuggerPause();
    const eventArgs = await pausedEvent;

    // Example:
    //	{
    //		"method":"Debugger.paused",
    //		"params":
    //		{
    //			"callFrames":
    //			[
    //				{
    //					"callFrameId":"0",
    //					"functionName":"flushedQueue",
    //					"location":{"scriptId":"3","lineNumber":23258,"columnNumber":20},
    //					"scopeChain":
    //					[
    //						{
    //							"type":"global",
    //							"object":{"type":"object","className":"Object","objectId":"1"}
    //						}
    //					],
    //					"url":"",
    //					"this":{"type":"object","objectId":"2"}
    //				}
    //			],
    //			"reason":"other"
    //		}
    //	}
    expect(eventArgs).toHaveProperty('callFrames');
    expect(eventArgs).toHaveProperty('reason');
    expect(eventArgs.callFrames.length).toBeGreaterThanOrEqual(1);
    expect(eventArgs.callFrames[0]).toHaveProperty('functionName');

    const resumedEvent = dbg.expectEvent('Debugger.resumed');
    await dbg.debuggerResume();
    await resumedEvent;

    await dbg.checkOutstandingResponses(3000);

    dbg.close();
  } finally {
    await settings.restore();
  }
});

test('set, remove breakpoint', async () => {
  testLog.message(`executing 'set, remove breakpoint' test on PID ${pid}`);

  const settings = await PlaygroundDebugSettings.set({
    webDebugger: false,
    directDebugging: true,
    jsEngine: 'Hermes',
  });
  try {
    const isBundleServed = metro.isBundleServed('debugTest01');
    await loadPackage('Samples\\debugTest01', isBundleServed);

    const debugTargets = await getDebugTargets();
    const dbg = new CDPDebugger(debugTargets[0].webSocketDebuggerUrl);

    await dbg.debuggerEnable();

    const pausedEvent = dbg.expectEvent('Debugger.paused');
    await dbg.debuggerPause();
    await pausedEvent;

    const {breakpointId} = await dbg.debuggerSetBreakpointByUrl(
      /* url */ 'http://localhost:8081/Samples/debugTest01.bundle?platform=windows&dev=true&hot=true&inlineSourceMap=true',
      /* urlRegex */ '',
      /* lineNumber */ 1392,
      /* columnNumber */ 0,
      /* scriptHash */ '',
    );

    expect(breakpointId).toBe('1');

    await dbg.debuggerRemoveBreakpoint(breakpointId);

    const resumedEvent = dbg.expectEvent('Debugger.resumed');
    await dbg.debuggerResume();
    await resumedEvent;

    await dbg.checkOutstandingResponses(3000);

    dbg.close();
  } finally {
    await settings.restore();
  }
});

// Regression test for the fix for RNW:9662 (https://github.com/microsoft/react-native-windows/issues/9662,
// this test would fail prior to the fix).
test('reload after continue', async () => {
  testLog.message(`executing 'pause, resume' test on PID ${pid}`);

  const settings = await PlaygroundDebugSettings.set({
    webDebugger: false,
    directDebugging: true,
    jsEngine: 'Hermes',
  });
  try {
    const isBundleServed0 = metro.isBundleServed('debugTest01');
    await loadPackage('Samples\\debugTest01', isBundleServed0);

    const debugTargets = await getDebugTargets();
    const dbg = new CDPDebugger(debugTargets[0].webSocketDebuggerUrl);

    await dbg.debuggerEnable();

    const pausedEvent = dbg.expectEvent('Debugger.paused');
    await dbg.debuggerPause();
    await pausedEvent;

    const resumedEvent = dbg.expectEvent('Debugger.resumed');
    await dbg.debuggerResume();
    await resumedEvent;

    const isBundleServed1 = metro.isBundleServed('debugTest01');

    // without the fix, this re-load would hang
    await loadPackage('Samples\\debugTest01', isBundleServed1);

    await dbg.checkOutstandingResponses(3000);

    dbg.close();
  } finally {
    await settings.restore();
  }
});

// Regression test for the fix for RN:34639 (https://github.com/facebook/react-native/issues/34639).
test('execution context identifier in Debugger.scriptParsed event', async () => {
  testLog.message(`executing 'pause, resume' test on PID ${pid}`);

  const settings = await PlaygroundDebugSettings.set({
    webDebugger: false,
    directDebugging: true,
    jsEngine: 'Hermes',
  });
  try {
    const isBundleServed = metro.isBundleServed('debugTest01');
    await loadPackage('Samples\\debugTest01', isBundleServed);

    const debugTargets = await getDebugTargets();
    const dbg = new CDPDebugger(debugTargets[0].webSocketDebuggerUrl);

    const executionContextCreatedEvent = dbg.expectEvent(
      'Runtime.executionContextCreated',
    );
    await dbg.runtimeEnable();

    const scriptParsedEvent = dbg.expectEvent('Debugger.scriptParsed');
    await dbg.debuggerEnable();

    const executionContextCreatedEventArgs = await executionContextCreatedEvent;
    const scriptParsedEventArgs = await scriptParsedEvent;

    expect(executionContextCreatedEventArgs).toHaveProperty('context');
    expect(executionContextCreatedEventArgs.context).toHaveProperty('id');
    expect(scriptParsedEventArgs).toHaveProperty('executionContextId');

    expect(scriptParsedEventArgs.executionContextId).toEqual(
      executionContextCreatedEventArgs.context.id,
    );

    await dbg.checkOutstandingResponses(3000);

    dbg.close();
  } finally {
    await settings.restore();
  }
});
