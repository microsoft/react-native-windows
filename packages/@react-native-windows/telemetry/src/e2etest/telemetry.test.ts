/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as coreOneDS from '@microsoft/1ds-core-js';
import * as path from 'path';

import {
  Telemetry,
  TelemetryOptions,
  CommandStartInfo,
  CommandEndInfo,
  CommandEventName,
  CodedErrorEventName,
} from '../telemetry';

import * as basePropUtils from '../utils/basePropUtils';
import * as errorUtils from '../utils/errorUtils';
import * as projectUtils from '../utils/projectUtils';
import * as versionUtils from '../utils/versionUtils';

export class TelemetryTest extends Telemetry {
  protected static hasTestTelemetryProviders: boolean;
  protected static testTelemetryProvidersRan: boolean;

  /** Run at the beginning of each test. */
  static async startTest(options?: Partial<TelemetryOptions>) {
    TelemetryTest.hasTestTelemetryProviders = false;
    TelemetryTest.testTelemetryProvidersRan = false;

    if (TelemetryTest.isEnabled()) {
      Telemetry.reset();
    }

    // Ensure that we don't actually fire events when testing
    Telemetry.isTestEnvironment = true;

    await Telemetry.setup(options);
  }

  /** Run at the end of each test where telemetry was fired. */
  static endTest(finalCallback?: () => void): void {
    Telemetry.appInsightsCore?.flush(undefined /* isAsync */, () => {
      // Your callback logic here
      if (TelemetryTest.hasTestTelemetryProviders) {
        expect(TelemetryTest.testTelemetryProvidersRan).toBe(true);
      }
      if (finalCallback) {
        finalCallback();
      }
    });
  }

  /** Sets that the telemetry provider has run. */
  static setTestTelemetryProvidersRan() {
    TelemetryTest.testTelemetryProvidersRan = true;
  }

  /** Retrieves the value of a common property.*/
  static getCommonProperty(key: string): string | undefined {
    return TelemetryTest.commonProperties[key];
  }

  /** Retrieves the version of the specified tool/package. */
  static getVersion(key: string): string | null {
    return key in TelemetryTest.versionsProp
      ? TelemetryTest.versionsProp[key]
      : null;
  }

  /** Retrieves the value of the preserveErrorMessages option. */
  static getPreserveErrorMessages(): boolean {
    return TelemetryTest.options.preserveErrorMessages;
  }

  /** Adds a telemetry processor, usually for verifying the envelope. */
  static addTelemetryInitializer(
    telemetryInitializer: (envelope: coreOneDS.ITelemetryItem) => boolean,
  ): void {
    TelemetryTest.appInsightsCore?.addTelemetryInitializer(
      telemetryInitializer,
    );
    TelemetryTest.hasTestTelemetryProviders = true;
  }
}

test('setup() verify session id is valid and a common property', async () => {
  await TelemetryTest.startTest();

  const sessionId = TelemetryTest.getSessionId();
  expect(sessionId).toBeDefined();
  expect(sessionId!).toHaveLength(32);
  expect(sessionId!).toBe(basePropUtils.getSessionId());
  expect(TelemetryTest.getCommonProperty('sessionId')).toBe(sessionId!);

  TelemetryTest.endTest();
});

test('setup() verify static common property values with async sources', async () => {
  await TelemetryTest.startTest();

  const props: Record<string, () => Promise<string | undefined>> = {
    deviceId: basePropUtils.deviceId,
    deviceLocale: basePropUtils.deviceLocale,
  };

  for (const key in props) {
    if (!(key in Object.prototype)) {
      const value = TelemetryTest.getCommonProperty(key);
      expect(value).toBeDefined();
      expect(value).toBe(await props[key]());
    }
  }

  TelemetryTest.endTest();
});

test('setup() verify static common property values with sync sources', async () => {
  await TelemetryTest.startTest();

  const props: Record<string, () => string | undefined> = {
    deviceArchitecture: () => basePropUtils.deviceArchitecture(),
    nodePlatform: () => basePropUtils.nodePlatform(),
    deviceNumCPUs: () => basePropUtils.deviceNumCPUs().toString(),
    deviceTotalMemory: () => basePropUtils.deviceTotalMemory().toString(),
    ciCaptured: () => basePropUtils.captureCI().toString(),
    ciType: () => basePropUtils.ciType(),
    isMsftInternal: () => basePropUtils.isMsftInternal().toString(),
    isTest: () => 'true',
  };

  for (const key in props) {
    if (!(key in Object.prototype)) {
      const value = TelemetryTest.getCommonProperty(key);
      expect(value).toBeDefined();
      expect(value).toBe(props[key]());
    }
  }

  TelemetryTest.endTest();
});

test('setup() verify other common property values are defined', async () => {
  await TelemetryTest.startTest();

  const props: string[] = ['deviceDiskFreeSpace'];

  for (const key of props) {
    const value = TelemetryTest.getCommonProperty(key);
    expect(value).toBeDefined();
  }

  TelemetryTest.endTest();
});

test('setup() verify tool versions are populated', async () => {
  await TelemetryTest.startTest();

  const props: Record<string, () => Promise<string | null>> = {
    node: versionUtils.getNodeVersion,
    npm: versionUtils.getNpmVersion,
    yarn: versionUtils.getYarnVersion,
    VisualStudio: versionUtils.getVisualStudioVersion,
  };

  for (const key in props) {
    if (!(key in Object.prototype)) {
      const value = await props[key]();
      expect(value).toBe(TelemetryTest.getVersion(key));
    }
  }

  TelemetryTest.endTest();
});

test('tryUpdateVersionsProp() returns true for adding a new version', async () => {
  await TelemetryTest.startTest();

  const name = 'test';
  const version = '1.0';
  expect(
    await TelemetryTest.tryUpdateVersionsProp(name, async () => version),
  ).toBe(true);
  expect(TelemetryTest.getVersion(name)).toBe(version);

  TelemetryTest.endTest();
});

test('tryUpdateVersionsProp() returns false for adding an existing version with refresh is false', async () => {
  await TelemetryTest.startTest();

  const name = 'test';
  const version = '1.0';

  expect(
    await TelemetryTest.tryUpdateVersionsProp(name, async () => version),
  ).toBe(true);

  let getValueCalled = false;
  expect(
    await TelemetryTest.tryUpdateVersionsProp(name, async () => {
      getValueCalled = true;
      return version;
    }),
  ).toBe(false);

  expect(getValueCalled).toBe(false);

  TelemetryTest.endTest();
});

test('tryUpdateVersionsProp() returns true for adding an existing version with refresh is true', async () => {
  await TelemetryTest.startTest();

  const name = 'test';
  const version = '1.0';

  expect(
    await TelemetryTest.tryUpdateVersionsProp(name, async () => version),
  ).toBe(true);

  let getValueCalled = false;
  expect(
    await TelemetryTest.tryUpdateVersionsProp(
      name,
      async () => {
        getValueCalled = true;
        return version;
      },
      true,
    ),
  ).toBe(true);

  expect(getValueCalled).toBe(true);

  TelemetryTest.endTest();
});

/** Returns the CommandStartInfo for our fake 'test-command'. */
function getTestCommandStartInfo(): CommandStartInfo {
  return {
    commandName: 'test-command',
    args: {
      testArg1: 'true',
      testArg2: '10',
      testArg3: 'testValue',
    },
    options: {
      testArg0: 'unsetArg',
      testArg1: true,
      testArg2: 10,
      testArg3: 'testValue',
    },
    defaultOptions: {
      testArg0: 'unsetArg',
      testArg1: false,
      testArg2: 0,
      testArg3: 'defaultValue',
    },
  };
}

/** Returns the CommandEndInfo for our fake 'test-command'. */
function getTestCommandEndInfo(
  resultCode: errorUtils.CodedErrorType,
): CommandEndInfo {
  return {
    resultCode,
  };
}

function getTestCommandProjectInfo(): projectUtils.AppProjectInfo {
  return {
    id: projectUtils.getProjectId('test-app-project'),
    platforms: ['windows'],
    rnwLang: 'cpp',
    usesTS: true,
    usesRNConfig: false,
    jsEngine: 'Chakra',
    rnwSource: 'Source',
    dependencies: [
      {
        id: projectUtils.getProjectId('test-module-project'),
        platforms: ['android', 'windows'],
        rnwLang: 'cpp',
      },
    ],
  };
}

function getExtraProps(): Record<string, any> {
  return {
    extraProp1: true,
    extraProp2: 1234,
    extraProp3: 'test',
    extraProp4: ['test'],
    extraProp5: {
      nestedProp1: true,
      nestedProp2: 1234,
    },
  };
}

/** Asynchronously waits the number in ms. */
async function promiseDelay(ms: number) {
  return new Promise(resolve => setTimeout(resolve, ms));
}

/** The body of the fake 'test-command' which will throw the provided error. */
async function testCommandBody(errorToThrow?: Error): Promise<void> {
  await promiseDelay(100);
  if (errorToThrow) {
    throw errorToThrow;
  }
}

/** Runs the complete 'test-command' with the right Telemetry setup and cleanup. */
async function runTestCommandE2E(commandBody: () => Promise<void>) {
  TelemetryTest.startCommand(getTestCommandStartInfo());
  TelemetryTest.setProjectInfo(getTestCommandProjectInfo());
  let errorCode: errorUtils.CodedErrorType = 'Success';
  let caughtError: Error | undefined;
  try {
    await commandBody();
  } catch (ex) {
    caughtError = ex instanceof Error ? (ex as Error) : new Error(String(ex));
    errorCode =
      caughtError instanceof errorUtils.CodedError
        ? (caughtError as errorUtils.CodedError).type
        : 'Unknown';
    TelemetryTest.trackException(caughtError);
  }
  TelemetryTest.endCommand(getTestCommandEndInfo(errorCode), getExtraProps());
}

/** Verifies the contents of events fired during the 'test-command'. */
function verifyTestCommandTelemetryProcessor(
  caughtErrors: Error[],
  expectedResultCode?: errorUtils.CodedErrorType,
  expectedError?: Error,
): (envelope: coreOneDS.ITelemetryItem) => boolean {
  return envelope => {
    TelemetryTest.setTestTelemetryProvidersRan();

    try {
      // Processor has run, so the test can (potentially) pass
      const properties = envelope.baseData;
      expect(properties).toBeDefined();

      // Verify basics
      const commonProperties = properties!.common;
      expect(commonProperties.commandName).toBe('test-command');

      // Verify versions info
      const versions = properties!.versions;
      expect(versions).toBeDefined();

      expect(Object.keys(versions).length).toBeGreaterThan(0);
      for (const key of Object.keys(versions)) {
        expect(versions[key]).toBe(TelemetryTest.getVersion(key));
      }

      // Verify project info
      const project = properties!.project;
      expect(project).toStrictEqual(getTestCommandProjectInfo());

      // Verify properties exclusive to error scenarios
      if (envelope.name === CodedErrorEventName) {
        // Verify exception info
        const exceptionData = envelope.data!.exceptionData;
        expect(exceptionData).toBeDefined();
        expect(exceptionData.message).toBeDefined();
        expect(exceptionData.message).not.toBe('');

        expect(exceptionData.message).toBe(
          TelemetryTest.getPreserveErrorMessages()
            ? errorUtils.sanitizeErrorMessage(expectedError?.message || 'None')
            : '[Removed]',
        );

        // Verify coded error info
        const codedError = envelope.data!.codedError;
        expect(codedError).toBeDefined();

        expect(codedError.type).toBe(
          expectedError instanceof errorUtils.CodedError
            ? expectedError.type
            : 'Unknown',
        );

        expect(codedError.data).toStrictEqual(
          (expectedError as errorUtils.CodedError).data ?? {},
        );
      } else {
        // If this is not error scenario, it must be a command successful event.
        expect(envelope.name).toBe(CommandEventName);

        // Verify command info
        const expectedInfo = getTestCommandStartInfo();

        const command = envelope.data!.command;
        expect(command).toBeDefined();
        expect(command.args).toStrictEqual(expectedInfo.args);
        expect(command.options).toStrictEqual(expectedInfo.options);
        expect(command.defaultOptions).toStrictEqual(
          expectedInfo.defaultOptions,
        );
        expect(command.durationInSecs).toBeGreaterThan(0);
        expect(command.resultCode).toBe(expectedResultCode ?? 'Success');

        // Verify extra props
        const extraProps = getExtraProps();
        expect(envelope.data?.additionalData).toStrictEqual(extraProps);
      }
    } catch (ex) {
      caughtErrors.push(
        ex instanceof Error ? (ex as Error) : new Error(String(ex)),
      );
    }

    return true;
  };
}

test('Telemetry run test command end to end, verify event fires', async () => {
  await TelemetryTest.startTest();

  // AI eats errors thrown in telemetry processors
  const caughtErrors: Error[] = [];
  TelemetryTest.addTelemetryInitializer(
    verifyTestCommandTelemetryProcessor(caughtErrors),
  );

  await runTestCommandE2E(testCommandBody);

  TelemetryTest.endTest(() => {
    // Check if any errors were thrown
    expect(caughtErrors).toHaveLength(0);
  });
});

const testTelemetryOptions = [
  {preserveErrorMessages: false},
  {preserveErrorMessages: true},
];

test.each(testTelemetryOptions)(
  'Telemetry run test command end to end with CodedError, verify events fire %s',
  async options => {
    await TelemetryTest.startTest(options);

    const expectedError = new errorUtils.CodedError(
      'MSBuildError',
      'test error',
    );

    // AI eats errors thrown in telemetry processors
    const caughtErrors: Error[] = [];
    TelemetryTest.addTelemetryInitializer(
      verifyTestCommandTelemetryProcessor(
        caughtErrors,
        expectedError.type,
        expectedError,
      ),
    );

    await runTestCommandE2E(() => testCommandBody(expectedError));

    TelemetryTest.endTest(() => {
      // Check if any errors were thrown
      expect(caughtErrors).toHaveLength(0);
    });
  },
);

test.each(testTelemetryOptions)(
  'Telemetry run test command end to end with CodedError (with error in message), verify events fire %s',
  async options => {
    await TelemetryTest.startTest(options);

    const expectedError = new errorUtils.CodedError(
      'MSBuildError',
      'error FOO2020: test error',
    );

    // AI eats errors thrown in telemetry processors
    const caughtErrors: Error[] = [];
    TelemetryTest.addTelemetryInitializer(
      verifyTestCommandTelemetryProcessor(
        caughtErrors,
        expectedError.type,
        expectedError,
      ),
    );

    await runTestCommandE2E(() => testCommandBody(expectedError));

    TelemetryTest.endTest(() => {
      // Check if any errors were thrown
      expect(caughtErrors).toHaveLength(0);
    });
  },
);

test.each(testTelemetryOptions)(
  'Telemetry run test command end to end with CodedError (with data), verify events fire %s',
  async options => {
    await TelemetryTest.startTest(options);

    const expectedError = new errorUtils.CodedError(
      'MSBuildError',
      'test error',
      {foo: 42},
    );

    // AI eats errors thrown in telemetry processors
    const caughtErrors: Error[] = [];
    TelemetryTest.addTelemetryInitializer(
      verifyTestCommandTelemetryProcessor(
        caughtErrors,
        expectedError.type,
        expectedError,
      ),
    );

    await runTestCommandE2E(() => testCommandBody(expectedError));

    TelemetryTest.endTest(() => {
      // Check if any errors were thrown
      expect(caughtErrors).toHaveLength(0);
    });
  },
);

test.each(testTelemetryOptions)(
  'Telemetry run test command end to end with Error, verify events fire %s',
  async options => {
    await TelemetryTest.startTest(options);

    const expectedError = new Error('error FOO2020: test error');

    // AI eats errors thrown in telemetry processors
    const caughtErrors: Error[] = [];
    TelemetryTest.addTelemetryInitializer(
      verifyTestCommandTelemetryProcessor(
        caughtErrors,
        'Unknown',
        expectedError,
      ),
    );

    await runTestCommandE2E(() => testCommandBody(expectedError));

    TelemetryTest.endTest(() => {
      // Check if any errors were thrown
      expect(caughtErrors).toHaveLength(0);
    });
  },
);

test.each(testTelemetryOptions)(
  'Telemetry run test command end to end with Error (no message), verify events fire %s',
  async options => {
    await TelemetryTest.startTest(options);

    const expectedError = new Error();

    // AI eats errors thrown in telemetry processors
    const caughtErrors: Error[] = [];
    TelemetryTest.addTelemetryInitializer(
      verifyTestCommandTelemetryProcessor(
        caughtErrors,
        'Unknown',
        expectedError,
      ),
    );

    await runTestCommandE2E(() => testCommandBody(expectedError));

    TelemetryTest.endTest(() => {
      // Check if any errors were thrown
      expect(caughtErrors).toHaveLength(0);
    });
  },
);

function b(s: string) {
  throw new Error('hello ' + s);
}

function a(s: string) {
  b(s);
}

/** Verifies the contents of an exception's message and stack frames */
function getVerifyStackTelemetryProcessor(
  caughtErrors: Error[],
  expectedError: Error,
): (envelope: coreOneDS.ITelemetryItem) => boolean {
  return envelope => {
    try {
      // Processor has run, so the test can (potentially) pass
      TelemetryTest.setTestTelemetryProvidersRan();

      if (envelope.name === CodedErrorEventName) {
        const data = envelope.data as any;
        expect(data.exceptionData).toBeDefined();
        expect(data.exceptionData.message).toBeDefined();
        expect(data.exceptionData.message).not.toBe('');

        expect(data.exceptionData.message).toBe(
          TelemetryTest.getPreserveErrorMessages()
            ? errorUtils.sanitizeErrorMessage(expectedError.message || 'None')
            : '[Removed]',
        );

        const stack = data.exceptionData.parsedStack;
        expect(stack).toBeDefined();
        expect(stack.length).toBeGreaterThan(2);

        const filename = path.relative(process.cwd(), __filename);
        expect(stack[0].functionName).toEqual('b');
        expect(stack[1].functionName).toEqual('b');
        expect(stack[2].functionName).toEqual('a');
        expect(stack[0].filePath).toEqual(
          `[project_dir]\\???.ts(${filename.length})`,
        );
        expect(stack[1].filePath).toEqual(
          `[project_dir]\\???.ts(${filename.length})`,
        );
        expect(stack[2].filePath).toEqual(
          `[project_dir]\\???.ts(${filename.length})`,
        );
      }
    } catch (ex) {
      caughtErrors.push(
        ex instanceof Error ? (ex as Error) : new Error(String(ex)),
      );
    }

    return true;
  };
}

test.each(testTelemetryOptions)(
  'Telemetry run test command end to end with Error, verify sanitized message and stack %s',
  async options => {
    await TelemetryTest.startTest(options);

    const expectedError = new Error('hello world');

    // AI eats errors thrown in telemetry processors
    const caughtErrors: Error[] = [];
    TelemetryTest.addTelemetryInitializer(
      getVerifyStackTelemetryProcessor(caughtErrors, expectedError),
    );

    await runTestCommandE2E(async () => {
      await promiseDelay(100);
      a('world');
    });

    TelemetryTest.endTest(() => {
      // Check if any errors were thrown
      expect(caughtErrors).toHaveLength(0);
    });
  },
);

test.each(testTelemetryOptions)(
  'Telemetry run test command end to end with Error, verify sanitized message with path and stack %s',
  async options => {
    await TelemetryTest.startTest(options);

    const expectedError = new Error(`hello ${process.cwd()}`);

    // AI eats errors thrown in telemetry processors
    const caughtErrors: Error[] = [];
    TelemetryTest.addTelemetryInitializer(
      getVerifyStackTelemetryProcessor(caughtErrors, expectedError),
    );

    await runTestCommandE2E(async () => {
      await promiseDelay(100);
      a(process.cwd());
    });

    TelemetryTest.endTest(() => {
      // Check if any errors were thrown
      expect(caughtErrors).toHaveLength(0);
    });
  },
);
