/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as appInsights from 'applicationinsights';
import CorrelationIdManager from 'applicationinsights/out/Library/CorrelationIdManager';
import * as path from 'path';

import {
  Telemetry,
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
  static async startTest() {
    TelemetryTest.hasTestTelemetryProviders = false;
    TelemetryTest.testTelemetryProvidersRan = false;

    jest.setTimeout(10000); // These E2E tests can run longer than the default 5000ms

    if (TelemetryTest.isEnabled()) {
      Telemetry.reset();
    }

    // Ensure that we don't actually fire events when testing
    Telemetry.isTest = true;

    await Telemetry.setup({preserveErrorMessages: true});
    
    // Workaround for https://github.com/microsoft/ApplicationInsights-node.js/issues/833
    Telemetry.client!.config.correlationIdRetryIntervalMs = 500;
    CorrelationIdManager.cancelCorrelationIdQuery(
      Telemetry.client!.config,
      () => {},
    );
  }

  /** Run at the end of each test where telemetry was fired. */
  static endTest(finalCallback: () => void): void {
    Telemetry.client?.flush({
      callback: _ => {
        if (TelemetryTest.hasTestTelemetryProviders) {
          expect(TelemetryTest.testTelemetryProvidersRan).toBe(true);
        }
        finalCallback();
      },
    });
  }

  /** Sets that the telemetry provider has run. */
  static setTestTelemetryProvidersRan() {
    TelemetryTest.testTelemetryProvidersRan = true;
  }

  /** Retrieves the value of a common property.*/
  static getCommonProperty(key: string): string | undefined {
    return TelemetryTest.client?.commonProperties[key];
  }

  /** Retrieves the version of the specified tool/package. */
  static getVersion(key: string): string | null {
    return key in TelemetryTest.versionsProp
      ? TelemetryTest.versionsProp[key]
      : null;
  }

  /** Adds a telemetry processor, usually for verifying the envelope. */
  static addTelemetryProcessor(
    telemetryProcessor: (
      envelope: appInsights.Contracts.EnvelopeTelemetry,
      contextObjects?: {
        [name: string]: any;
      },
    ) => boolean,
  ): void {
    TelemetryTest.client?.addTelemetryProcessor(telemetryProcessor);
    TelemetryTest.hasTestTelemetryProviders = true;
  }
}

beforeEach(async () => {
  await TelemetryTest.startTest();
});

test('setup() verify session id is valid and a common property', async () => {
  const sessionId = TelemetryTest.getSessionId();
  expect(sessionId).toBeDefined();
  expect(sessionId!).toHaveLength(32);
  expect(sessionId!).toBe(basePropUtils.getSessionId());
  expect(sessionId!).toBe(TelemetryTest.getCommonProperty('sessionId'));
});

test('setup() verify static common property values with async sources', async () => {
  const props: Record<string, () => Promise<string | undefined>> = {
    deviceId: basePropUtils.deviceId,
    deviceLocale: basePropUtils.deviceLocale,
  };

  for (const key in props) {
    if (!(key in Object.prototype)) {
      const value = await props[key]();
      expect(value).toBeDefined();
      expect(value).toBe(TelemetryTest.getCommonProperty(key));
    }
  }
});

test('setup() verify static common property values with sync sources', () => {
  const props: Record<string, () => string | undefined> = {
    deviceNumCPUs: () => basePropUtils.deviceNumCPUs().toString(),
    deviceTotalMemory: () => basePropUtils.deviceTotalMemory().toString(),
    ciCaptured: () => basePropUtils.captureCI().toString(),
    ciType: () => basePropUtils.ciType(),
    isMsftInternal: () => basePropUtils.isMsftInternal().toString(),
    isTest: () => 'true',
  };

  for (const key in props) {
    if (!(key in Object.prototype)) {
      const value = props[key]();
      expect(value).toBeDefined();
      expect(value).toBe(TelemetryTest.getCommonProperty(key));
    }
  }
});

test('setup() verify other common property values are defined', () => {
  const props: string[] = ['deviceDiskFreeSpace'];

  for (const key of props) {
    const value = TelemetryTest.getCommonProperty(key);
    expect(value).toBeDefined();
  }
});

test('setup() verify tool versions are populated', async () => {
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
});

test('tryUpdateVersionsProp() returns true for adding a new version', async () => {
  const name = 'test';
  const version = '1.0';
  expect(
    await TelemetryTest.tryUpdateVersionsProp(name, async () => version),
  ).toBe(true);
  expect(TelemetryTest.getVersion(name)).toBe(version);
});

test('tryUpdateVersionsProp() returns false for adding an existing version with refresh is false', async () => {
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
});

test('tryUpdateVersionsProp() returns true for adding an existing version with refresh is true', async () => {
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

/** Verifys the contents of events fired during the 'test-command'. */
function verifyTestCommandTelemetryProcessor(
  caughtErrors: Error[],
  expectedResultCode?: errorUtils.CodedErrorType,
  expectedError?: Error,
): (
  envelope: appInsights.Contracts.EnvelopeTelemetry,
  contextObjects?: {
    [name: string]: any;
  },
) => boolean {
  return (envelope, _) => {
    try {
      // Processor has run, so the test can (potentially) pass
      TelemetryTest.setTestTelemetryProvidersRan();

      // Verify roleInstance has been removed
      expect(envelope.tags['ai.cloud.roleInstance']).toBeUndefined();

      const properties = envelope.data.baseData?.properties;
      expect(properties).toBeDefined();

      // Verify basics
      expect(properties.commandName).toBe('test-command');

      // Verify versions info
      const versions = JSON.parse(properties.versions);
      expect(versions).toBeDefined();

      // Verify project info
      const project = JSON.parse(properties.project);
      expect(project).toStrictEqual(getTestCommandProjectInfo());

      expect(Object.keys(versions).length).toBeGreaterThan(0);
      for (const key of Object.keys(versions)) {
        expect(versions[key]).toBe(TelemetryTest.getVersion(key));
      }

      if (envelope.data.baseType === 'ExceptionData') {
        // Verify event name
        expect(properties.eventName).toBe(CodedErrorEventName);

        // Verify coded error info
        const codedError = JSON.parse(properties.codedError);
        expect(codedError).toBeDefined();

        expect(codedError.type).toBe(
          expectedError instanceof errorUtils.CodedError
            ? expectedError.type
            : 'Unknown',
        );
        expect(codedError.rawErrorCode).toBe(
          errorUtils.tryGetErrorCode(expectedError?.message ?? '') ?? '',
        );
        expect(codedError.data).toStrictEqual(
          (expectedError as errorUtils.CodedError).data ?? {},
        );
      } else {
        // Verify event name
        expect(envelope.data.baseData?.name).toBe(CommandEventName);
        expect(properties.eventName).toBe(CommandEventName);

        // Verify command info
        const expectedInfo = getTestCommandStartInfo();

        const command = JSON.parse(properties.command);
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
        for (const key of Object.keys(extraProps)) {
          switch (typeof extraProps[key]) {
            case 'object':
              expect(JSON.parse(properties[key])).toStrictEqual(
                extraProps[key],
              );
              break;
            default:
              expect(properties[key]).toBe(extraProps[key].toString());
          }
        }
      }
    } catch (ex) {
      caughtErrors.push(
        ex instanceof Error ? (ex as Error) : new Error(String(ex)),
      );
    }

    return true;
  };
}

test('Telemetry run test command end to end, verify event fires', async done => {
  // AI eats errors thrown in telemetry processors
  const caughtErrors: Error[] = [];
  TelemetryTest.addTelemetryProcessor(
    verifyTestCommandTelemetryProcessor(caughtErrors),
  );

  await runTestCommandE2E(testCommandBody);

  TelemetryTest.endTest(() => {
    // Check if any errors were thrown
    expect(caughtErrors).toHaveLength(0);
    done();
  });
});

test('Telemetry run test command end to end with CodedError, verify events fire', async done => {
  const expectedError = new errorUtils.CodedError('MSBuildError', 'test error');

  // AI eats errors thrown in telemetry processors
  const caughtErrors: Error[] = [];
  TelemetryTest.addTelemetryProcessor(
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
    done();
  });
});

test('Telemetry run test command end to end with CodedError (with error in message), verify events fire', async done => {
  const expectedError = new errorUtils.CodedError(
    'MSBuildError',
    'error FOO2020: test error',
  );

  // AI eats errors thrown in telemetry processors
  const caughtErrors: Error[] = [];
  TelemetryTest.addTelemetryProcessor(
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
    done();
  });
});

test('Telemetry run test command end to end with CodedError (with data), verify events fire', async done => {
  const expectedError = new errorUtils.CodedError(
    'MSBuildError',
    'test error',
    {foo: 42},
  );

  // AI eats errors thrown in telemetry processors
  const caughtErrors: Error[] = [];
  TelemetryTest.addTelemetryProcessor(
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
    done();
  });
});

test('Telemetry run test command end to end with Error, verify events fire', async done => {
  const expectedError = new Error('error FOO2020: test error');

  // AI eats errors thrown in telemetry processors
  const caughtErrors: Error[] = [];
  TelemetryTest.addTelemetryProcessor(
    verifyTestCommandTelemetryProcessor(caughtErrors, 'Unknown', expectedError),
  );

  await runTestCommandE2E(() => testCommandBody(expectedError));

  TelemetryTest.endTest(() => {
    // Check if any errors were thrown
    expect(caughtErrors).toHaveLength(0);
    done();
  });
});

test('Telemetry run test command end to end with Error (no message), verify events fire', async done => {
  const expectedError = new Error();

  // AI eats errors thrown in telemetry processors
  const caughtErrors: Error[] = [];
  TelemetryTest.addTelemetryProcessor(
    verifyTestCommandTelemetryProcessor(caughtErrors, 'Unknown', expectedError),
  );

  await runTestCommandE2E(() => testCommandBody(expectedError));

  TelemetryTest.endTest(() => {
    // Check if any errors were thrown
    expect(caughtErrors).toHaveLength(0);
    done();
  });
});

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
): (
  envelope: appInsights.Contracts.EnvelopeTelemetry,
  contextObjects?: {
    [name: string]: any;
  },
) => boolean {
  return (envelope, _) => {
    try {
      // Processor has run, so the test can (potentially) pass
      TelemetryTest.setTestTelemetryProvidersRan();

      if (envelope.data.baseType === 'ExceptionData') {
        const data = (envelope.data as any).baseData;
        expect(data.exceptions).toBeDefined();
        expect(data.exceptions.length).toBe(1);
        expect(data.exceptions[0].message).toBe(
          errorUtils.sanitizeErrorMessage(expectedError.message),
        );

        const stack = data.exceptions[0].parsedStack;
        expect(stack).toBeDefined();
        expect(stack.length).toBeGreaterThan(2);

        const filename = path.relative(process.cwd(), __filename);
        expect(stack[0].method).toEqual('b');
        expect(stack[1].method).toEqual('a');
        expect(stack[0].fileName).toEqual(
          `[project_dir]\\???.ts(${filename.length})`,
        );
        expect(stack[1].fileName).toEqual(
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

test('Telemetry run test command end to end with Error, verify sanitized message and stack', async done => {
  const expectedError = new Error('hello world');

  // AI eats errors thrown in telemetry processors
  const caughtErrors: Error[] = [];
  TelemetryTest.addTelemetryProcessor(
    getVerifyStackTelemetryProcessor(caughtErrors, expectedError),
  );

  await runTestCommandE2E(async () => {
    await promiseDelay(100);
    a('world');
  });

  TelemetryTest.endTest(() => {
    // Check if any errors were thrown
    expect(caughtErrors).toHaveLength(0);
    done();
  });
});

test('Telemetry run test command end to end with Error, verify sanitized message with path and stack', async done => {
  const expectedError = new Error(`hello ${process.cwd()}`);

  // AI eats errors thrown in telemetry processors
  const caughtErrors: Error[] = [];
  TelemetryTest.addTelemetryProcessor(
    getVerifyStackTelemetryProcessor(caughtErrors, expectedError),
  );

  await runTestCommandE2E(async () => {
    await promiseDelay(100);
    a(process.cwd());
  });

  TelemetryTest.endTest(() => {
    // Check if any errors were thrown
    expect(caughtErrors).toHaveLength(0);
    done();
  });
});
