/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import * as appInsights from 'applicationinsights';

import * as basePropUtils from './utils/basePropUtils';
import * as versionUtils from './utils/versionUtils';
import * as errorUtils from './utils/errorUtils';
import * as projectUtils from './utils/projectUtils';

export interface TelemetryOptions {
  setupString: string;
  preserveErrorMessages: boolean;
}

export interface CommandStartInfo {
  commandName: string;
  args: Record<string, any>;
  options: Record<string, any>;
  defaultOptions: Record<string, any>;
}

export interface CommandEndInfo {
  resultCode: errorUtils.CodedErrorType;
}

interface CommandInfo {
  startTime?: number;
  endTime?: number;
  startInfo?: CommandStartInfo;
  endInfo?: CommandEndInfo;
}

// This is our key with the AI backend
const RNWSetupString = '795006ca-cf54-40ee-8bc6-03deb91401c3';

// Environment variable to override the default setup string
const ENV_SETUP_OVERRIDE = 'RNW_TELEMETRY_SETUP';

// Environment variable to override the http proxy (such as http://localhost:8888 for Fiddler debugging)
const ENV_PROXY_OVERRIDE = 'RNW_TELEMETRY_PROXY';

export const CommandEventName = 'RNWCLI.Command';
export const CodedErrorEventName = 'RNWCLI.CodedError';

// These are the event names we're tracking
export const EventNamesWeTrack: string[] = [
  CommandEventName,
  CodedErrorEventName,
];

// These are NPM packages we care about, in terms of capturing versions used
// and getting more details about when reporting errors
export const NpmPackagesWeTrack: string[] = [
  '@react-native-community/cli',
  '@react-native-windows/cli',
  '@react-native-windows/telemetry',
  'react',
  'react-native',
  'react-native-windows',
  'react-native-windows-init',
];

// These are NPM packages we care about, in terms of capturing versions used
export const NuGetPackagesWeTrack: string[] = [
  'Microsoft.UI.Xaml',
  'Microsoft.Windows.CppWinRT',
  'Microsoft.WinUI',
];

/**
 * The Telemetry class is responsible for reporting telemetry for RNW CLI.
 */
export class Telemetry {
  protected static client?: appInsights.TelemetryClient = undefined;
  protected static options: TelemetryOptions = {
    setupString: Telemetry.getDefaultSetupString(), // We default to our AI key, but callers can easily override it in setup
    preserveErrorMessages: false,
  };

  protected static isTest: boolean = basePropUtils.isCliTest();
  protected static commandInfo: CommandInfo = {};
  protected static versionsProp: Record<string, string> = {};
  protected static projectProp?:
    | projectUtils.AppProjectInfo
    | projectUtils.DependencyProjectInfo = undefined;

  protected static getDefaultSetupString(): string {
    // Enable overriding the default setup string via an environment variable
    return process.env[ENV_SETUP_OVERRIDE] ?? RNWSetupString;
  }

  protected static reset(): void {
    // Reset client
    if (Telemetry.client) {
      Telemetry.client.flush();
      Telemetry.client = undefined;
    }

    // Reset local members
    Telemetry.options = {
      setupString: Telemetry.getDefaultSetupString(),
      preserveErrorMessages: false,
    };
    Telemetry.commandInfo = {};
    Telemetry.versionsProp = {};
    Telemetry.projectProp = undefined;
  }

  static isEnabled(): boolean {
    return Telemetry.client !== undefined;
  }

  static getSessionId(): string {
    return basePropUtils.getSessionId();
  }

  /** Sets up the Telemetry static to be used elsewhere. */
  static async setup(options?: Partial<TelemetryOptions>) {
    if (Telemetry.client) {
      // Bail since we've already setup
      return;
    }

    // Bail if we're in CI and not capturing CI
    if (!this.isTest && basePropUtils.isCI() && !basePropUtils.captureCI()) {
      return;
    }

    // Save off options for later
    Object.assign(Telemetry.options, options);

    Telemetry.setupClient();

    await Telemetry.setupBaseProperties();

    Telemetry.setupTelemetryProcessors();
  }

  /** Sets up Telemetry.client. */
  private static setupClient() {
    appInsights.Configuration.setInternalLogging(
      Telemetry.isTest,
      Telemetry.isTest,
    );

    Telemetry.client = new appInsights.TelemetryClient(
      Telemetry.options.setupString,
    );

    // Allow overriding the proxy server via an environment variable
    const proxyServer = process.env[ENV_PROXY_OVERRIDE];
    if (proxyServer) {
      Telemetry.client.config.proxyHttpUrl = proxyServer;
      Telemetry.client.config.proxyHttpsUrl = proxyServer;
    }

    Telemetry.client.config.disableAppInsights = Telemetry.isTest;
    Telemetry.client.channel.setUseDiskRetryCaching(!Telemetry.isTest);
  }

  /** Sets up any base properties that all telemetry events require. */
  private static async setupBaseProperties() {
    Telemetry.client!.commonProperties.deviceId = await basePropUtils.deviceId();
    Telemetry.client!.commonProperties.deviceLocale = await basePropUtils.deviceLocale();
    Telemetry.client!.commonProperties.deviceNumCPUs = basePropUtils
      .deviceNumCPUs()
      .toString();
    Telemetry.client!.commonProperties.deviceTotalMemory = basePropUtils
      .deviceTotalMemory()
      .toString();
    Telemetry.client!.commonProperties.deviceDiskFreeSpace = basePropUtils
      .deviceDiskFreeSpace()
      .toString();
    Telemetry.client!.commonProperties.ciCaptured = basePropUtils
      .captureCI()
      .toString();
    Telemetry.client!.commonProperties.ciType = basePropUtils.ciType();
    Telemetry.client!.commonProperties.isMsftInternal = basePropUtils
      .isMsftInternal()
      .toString();

    Telemetry.client!.config.samplingPercentage = basePropUtils.sampleRate();

    if (Telemetry.isTest) {
      Telemetry.client!.commonProperties.isTest = true.toString();
    }

    Telemetry.client!.commonProperties.sessionId = Telemetry.getSessionId();

    await Telemetry.populateToolsVersions();
    await Telemetry.populateNpmPackageVersions();
  }

  /** Sets up any telemetry processors. */
  private static setupTelemetryProcessors() {
    Telemetry.client!.addTelemetryProcessor(Telemetry.basicTelemetryProcessor);
    Telemetry.client!.addTelemetryProcessor(Telemetry.errorTelemetryProcessor);
  }

  /**
   * Performs the processing necessary (mostly PII sanitization) for all events.
   * @param envelope The ApplicationInsights event envelope.
   * @param _contextObjects An optional context object.
   * @returns Whether to kee
   */
  private static basicTelemetryProcessor(
    envelope: appInsights.Contracts.EnvelopeTelemetry,
    _contextObjects?: {
      [name: string]: any;
    },
  ): boolean {
    delete envelope.tags['ai.cloud.roleInstance'];

    // Filter out "legacy" events from older stable branches
    const properties = envelope.data.baseData?.properties;
    if (
      properties?.eventName &&
      EventNamesWeTrack.includes(properties.eventName)
    ) {
      return true;
    }

    return false;
  }

  /**
   * Performs the processing necessary (mostly PII sanitization) for error events.
   * @param envelope
   * @param _contextObjects
   * @returns
   */
  private static errorTelemetryProcessor(
    envelope: appInsights.Contracts.EnvelopeTelemetry,
    _contextObjects?: {
      [name: string]: any;
    },
  ): boolean {
    if (envelope.data.baseType === 'ExceptionData') {
      const data = envelope.data.baseData;
      if (data?.exceptions) {
        for (const exception of data.exceptions) {
          for (const frame of exception.parsedStack) {
            errorUtils.sanitizeErrorStackFrame(frame);
          }
          // CodedError has non-PII information in its 'type' member, plus optionally some more info in its 'data'.
          // The message may contain PII information. This can be sanitized, but for now delete it.
          // Note that the type of data.exceptions[0] is always going to be ExceptionDetails. It is not the original thrown exception.
          // https://github.com/microsoft/ApplicationInsights-node.js/issues/707
          if (Telemetry.options.preserveErrorMessages) {
            exception.message = errorUtils.sanitizeErrorMessage(
              exception.message,
            );
          } else {
            delete exception.message;
          }
        }
      }
    }
    return true;
  }

  /** Tries to update the version of the named package/tool by calling getValue(). */
  static async tryUpdateVersionsProp(
    name: string,
    getValue: () => Promise<string | null>,
    forceRefresh?: boolean,
  ): Promise<boolean> {
    if (!Telemetry.client) {
      return true;
    }

    if (forceRefresh === true || !Telemetry.versionsProp[name]) {
      const value = await getValue();
      if (value) {
        Telemetry.versionsProp[name] = value;
        return true;
      }
    }
    return false;
  }

  /** Populates the versions property of tools we care to track. */
  static async populateToolsVersions(refresh?: boolean) {
    await Telemetry.tryUpdateVersionsProp(
      'node',
      versionUtils.getNodeVersion,
      refresh,
    );
    await Telemetry.tryUpdateVersionsProp(
      'npm',
      versionUtils.getNpmVersion,
      refresh,
    );
    await Telemetry.tryUpdateVersionsProp(
      'yarn',
      versionUtils.getYarnVersion,
      refresh,
    );
    await Telemetry.tryUpdateVersionsProp(
      'VisualStudio',
      versionUtils.getVisualStudioVersion,
      refresh,
    );
  }

  /** Populates the versions property of npm packages we care to track. */
  static async populateNpmPackageVersions(refresh?: boolean) {
    for (const npmPackage of NpmPackagesWeTrack) {
      await Telemetry.tryUpdateVersionsProp(
        npmPackage,
        async () => await versionUtils.getVersionOfNpmPackage(npmPackage),
        refresh,
      );
    }
  }

  /** Populates the versions property of nuget packages we care to track. */
  static async populateNuGetPackageVersions(
    projectFile: string,
    refresh?: boolean,
  ) {
    const nugetVersions = await versionUtils.getVersionsOfNuGetPackages(
      projectFile,
      NuGetPackagesWeTrack,
    );

    for (const nugetPackage of NuGetPackagesWeTrack) {
      await Telemetry.tryUpdateVersionsProp(
        nugetPackage,
        async () => nugetVersions[nugetPackage],
        refresh,
      );
    }
  }

  static setProjectInfo(
    info: projectUtils.AppProjectInfo | projectUtils.DependencyProjectInfo,
  ) {
    if (!Telemetry.client) {
      return;
    }

    Telemetry.projectProp = info;
  }

  static startCommand(info: CommandStartInfo) {
    if (!Telemetry.client) {
      return;
    }

    if (Telemetry.commandInfo.startInfo) {
      return;
    }

    Telemetry.commandInfo.startTime = Date.now();
    Telemetry.commandInfo.startInfo = info;

    // Set common command props
    Telemetry.client!.commonProperties.commandName = info.commandName;
  }

  static endCommand(info: CommandEndInfo, extraProps?: Record<string, any>) {
    if (!Telemetry.client) {
      return;
    }

    if (!Telemetry.commandInfo.startInfo) {
      return;
    }

    Telemetry.commandInfo.endTime = Date.now();
    Telemetry.commandInfo.endInfo = info;

    Telemetry.trackCommandEvent(extraProps);
  }

  private static trackCommandEvent(extraProps?: Record<string, any>) {
    const props: Record<string, any> = {
      eventName: CommandEventName,
    };

    // Set command props
    props.command = {
      options: Telemetry.commandInfo.startInfo?.options,
      defaultOptions: Telemetry.commandInfo.startInfo?.defaultOptions,
      args: Telemetry.commandInfo.startInfo?.args,
      durationInSecs:
        (Telemetry.commandInfo.endTime! - Telemetry.commandInfo.startTime!) /
        1000,
      resultCode: Telemetry.commandInfo.endInfo?.resultCode,
    };

    // Set remaining common props
    Object.assign(props, extraProps);
    props.project = Telemetry.projectProp;
    props.versions = Telemetry.versionsProp;

    // Fire event
    Telemetry.client!.trackEvent({name: props.eventName, properties: props});
    Telemetry.client!.flush();
  }

  static trackException(error: Error, extraProps?: Record<string, any>) {
    if (!Telemetry.client) {
      return;
    }

    const props: Record<string, any> = {
      eventName: CodedErrorEventName,
    };

    // Save off CodedError info
    const codedError =
      error instanceof errorUtils.CodedError
        ? (error as errorUtils.CodedError)
        : null;
    props.codedError = {
      type: codedError?.type ?? 'Unknown',
      rawErrorCode: errorUtils.tryGetErrorCode(error.message) ?? '',
      data: codedError?.data ?? {},
    };

    if (codedError?.data) {
      Object.assign(props.codedError.data, codedError.data);
    }

    // Copy miscellaneous system error fields into the codedError.data object
    const syscallExceptionFieldsToCopy = ['errno', 'syscall', 'code'];
    for (const f of syscallExceptionFieldsToCopy) {
      if ((error as any)[f]) {
        props.codedError.data[f] = (error as any)[f];
      }
    }

    // Set remaining common props
    Object.assign(props, extraProps);
    props.project = Telemetry.projectProp;
    props.versions = Telemetry.versionsProp;

    // Fire event
    Telemetry.client!.trackException({
      exception: error,
      properties: props,
    });
    Telemetry.client!.flush();
  }
}
