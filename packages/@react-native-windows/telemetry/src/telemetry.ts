/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import * as coreOneDS from '@microsoft/1ds-core-js';
import {PostChannel, IChannelConfiguration} from '@microsoft/1ds-post-js';

import * as basePropUtils from './utils/basePropUtils';
import * as versionUtils from './utils/versionUtils';
import * as errorUtils from './utils/errorUtils';
import * as projectUtils from './utils/projectUtils';

export interface TelemetryOptions {
  setupString: string;
  preserveErrorMessages: boolean;
  populateNpmPackageVersions: boolean;
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

// 1DS instrumentation key
const RNW_1DS_INSTRUMENTATION_KEY = '';

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
  protected static appInsightsCore?: coreOneDS.AppInsightsCore = undefined;

  protected static options: TelemetryOptions = {
    setupString: Telemetry.getDefaultSetupString(), // We default to our 1DS instrumentation key, but callers can easily override it in setup.
    preserveErrorMessages: false,
    populateNpmPackageVersions: true,
  };

  protected static isTestEnvironment: boolean = basePropUtils.isCliTest();

  protected static commandInfo: CommandInfo = {};

  // Stores the version of a list of packages used by the RNW app project.
  protected static versionsProp: Record<string, string> = {};

  protected static projectProp?:
    | projectUtils.AppProjectInfo
    | projectUtils.DependencyProjectInfo = undefined;

  // Store "Common Properties" in a single object. This will be logged in all telemetry events.
  protected static commonProperties: {[key: string]: string} = {};

  protected static getDefaultSetupString(): string {
    // Enable overriding the default setup string via an environment variable
    return process.env[ENV_SETUP_OVERRIDE] ?? RNW_1DS_INSTRUMENTATION_KEY;
  }

  protected static reset(): void {
    // Reset client
    if (Telemetry.appInsightsCore) {
      Telemetry.appInsightsCore.flush();
      Telemetry.appInsightsCore = undefined;
    }

    // Reset local members
    Telemetry.options = {
      setupString: Telemetry.getDefaultSetupString(),
      preserveErrorMessages: false,
      populateNpmPackageVersions: true,
    };
    Telemetry.commandInfo = {};
    Telemetry.versionsProp = {};
    Telemetry.projectProp = undefined;
  }

  static isEnabled(): boolean {
    return Telemetry.appInsightsCore !== undefined;
  }

  static getSessionId(): string {
    return basePropUtils.getSessionId();
  }

  /** Sets up the Telemetry static to be used elsewhere. */
  static async setup(options?: Partial<TelemetryOptions>) {
    if (Telemetry.appInsightsCore) {
      // Bail since we've already setup
      return;
    }

    // Bail if we're in CI and not capturing CI
    if (
      !Telemetry.isTestEnvironment &&
      basePropUtils.isCI() &&
      !basePropUtils.captureCI()
    ) {
      return;
    }

    // Save off options for later
    Object.assign(Telemetry.options, options);

    Telemetry.setupClient();

    await Telemetry.setupBaseProperties();
  }

  private static basicTelemetryInitializer(
    envelope: coreOneDS.ITelemetryItem,
  ): boolean {
    // Filter out "legacy" events from older stable branches
    if (envelope.name && EventNamesWeTrack.includes(envelope.name)) {
      return true;
    }

    return false;
  }

  /** Sets up Telemetry.appInsightsCore. */
  private static setupClient() {
    const postChannel: PostChannel = new PostChannel();

    const coreConfiguration: coreOneDS.IExtendedConfiguration = {
      instrumentationKey: Telemetry.getDefaultSetupString(),
    };

    const postChannelConfig: IChannelConfiguration = {
      eventsLimitInMem: 5000,
    };

    coreConfiguration.extensionConfig = {};
    coreConfiguration.extensionConfig[postChannel.identifier] =
      postChannelConfig;

    // Allow overriding the endpoint URL via an environment variable.
    if (process.env[ENV_PROXY_OVERRIDE] !== undefined) {
      coreConfiguration.endpointUrl = process.env[ENV_PROXY_OVERRIDE];
    }

    Telemetry.appInsightsCore = new coreOneDS.AppInsightsCore();
    Telemetry.appInsightsCore.initialize(
      coreConfiguration,
      [postChannel] /* extensions */,
    );

    Telemetry.appInsightsCore.addTelemetryInitializer(
      Telemetry.basicTelemetryInitializer,
    );
  }

  /** Sets up any base properties that all telemetry events require. */
  private static async setupBaseProperties() {
    Telemetry.commonProperties.deviceId = await basePropUtils.deviceId();
    Telemetry.commonProperties.fullBuildInfo =
      await basePropUtils.fullBuildInfo();
    Telemetry.commonProperties.deviceArchitecture =
      basePropUtils.deviceArchitecture();
    Telemetry.commonProperties.nodeArchitecture =
      basePropUtils.nodeArchitecture();
    Telemetry.commonProperties.nodePlatform = basePropUtils.nodePlatform();
    Telemetry.commonProperties.deviceClass = basePropUtils.deviceClass();
    Telemetry.commonProperties.deviceLocale =
      await basePropUtils.deviceLocale();
    Telemetry.commonProperties.deviceNumCPUs = basePropUtils
      .deviceNumCPUs()
      .toString();
    Telemetry.commonProperties.deviceTotalMemory = basePropUtils
      .deviceTotalMemory()
      .toString();
    Telemetry.commonProperties.deviceDiskFreeSpace = basePropUtils
      .deviceDiskFreeSpace()
      .toString();
    Telemetry.commonProperties.ciCaptured = basePropUtils
      .captureCI()
      .toString();
    Telemetry.commonProperties.ciType = basePropUtils.ciType();
    Telemetry.commonProperties.isMsftInternal = basePropUtils
      .isMsftInternal()
      .toString();
    Telemetry.commonProperties.isTest = Telemetry.isTestEnvironment.toString();
    Telemetry.commonProperties.sessionId = Telemetry.getSessionId();

    await Telemetry.populateToolsVersions();
    if (Telemetry.options.populateNpmPackageVersions) {
      await Telemetry.populateNpmPackageVersions();
    }
  }

  /** Tries to update the version of the named package/tool by calling getValue(). */
  static async tryUpdateVersionsProp(
    name: string,
    getValue: () => Promise<string | null>,
    forceRefresh?: boolean,
  ): Promise<boolean> {
    if (!Telemetry.appInsightsCore) {
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
    if (!Telemetry.appInsightsCore) {
      return;
    }

    Telemetry.projectProp = info;
  }

  static startCommand(info: CommandStartInfo) {
    if (!Telemetry.appInsightsCore) {
      return;
    }

    // startCommand() was called before invoking endCommand(), bail out.
    if (Telemetry.commandInfo.startInfo) {
      return;
    }

    Telemetry.commandInfo.startTime = Date.now();
    Telemetry.commandInfo.startInfo = info;

    // Set common command props
    Telemetry.commonProperties.commandName = info.commandName;
  }

  static endCommand(info: CommandEndInfo, extraProps?: Record<string, any>) {
    if (!Telemetry.appInsightsCore) {
      return;
    }

    // startCommand() wasn't called, bail out.
    if (!Telemetry.commandInfo.startInfo) {
      return;
    }

    Telemetry.commandInfo.endTime = Date.now();
    Telemetry.commandInfo.endInfo = info;

    Telemetry.trackCommandEvent(extraProps);
  }

  private static trackEvent(telemetryItem: coreOneDS.ITelemetryItem) {
    // Populate Part A
    telemetryItem.ver = '4.0'; // Current Common Schema version
    telemetryItem.time = new Date().toISOString();
    telemetryItem.iKey = RNW_1DS_INSTRUMENTATION_KEY;

    // Populate Part A extensions
    telemetryItem.ext = {};
    telemetryItem.ext.device = {
      id: Telemetry.commonProperties.deviceId,
      deviceClass: Telemetry.commonProperties.deviceClass,
    };
    telemetryItem.ext.os = {
      locale: Telemetry.commonProperties.deviceLocale,
      ver: Telemetry.commonProperties.fullBuildInfo,
    };

    // Populate most of "common" properties into Part B.
    telemetryItem.baseData = {
      common: {
        device: {
          architecture: Telemetry.commonProperties.deviceArchitecture,
          numCPUs: Telemetry.commonProperties.numCPUs,
          totalMemory: Telemetry.commonProperties.totalMemory,
          diskFreeSpace: Telemetry.commonProperties.deviceDiskFreeSpace,
        },
        nodePlatform: Telemetry.commonProperties.nodePlatform,
        nodeArchitecture: Telemetry.commonProperties.nodeArchitecture,
        ciCaptured: Telemetry.commonProperties.ciCaptured,
        ciType: Telemetry.commonProperties.ciType,
        isMsftInternal: Telemetry.commonProperties.isMsftInternal,
        isCliTest: Telemetry.commonProperties.isTest,
        sessionId: Telemetry.commonProperties.sessionId,
        commandName: Telemetry.commonProperties.commandName,
      },
      // Set project and versions props, belonging to Part B.
      project: Telemetry.projectProp,
      versions: Telemetry.versionsProp,
    };

    // Send and post the telemetry event!
    Telemetry.appInsightsCore!.track(telemetryItem);
    Telemetry.appInsightsCore!.flush();
  }

  private static trackCommandEvent(extraProps?: Record<string, any>) {
    const telemetryItem: coreOneDS.ITelemetryItem = {name: CommandEventName};

    // This is logged in Part C.
    const command = {
      options: Telemetry.commandInfo.startInfo?.options,
      defaultOptions: Telemetry.commandInfo.startInfo?.defaultOptions,
      args: Telemetry.commandInfo.startInfo?.args,
      durationInSecs:
        (Telemetry.commandInfo.endTime! - Telemetry.commandInfo.startTime!) /
        1000,
      resultCode: Telemetry.commandInfo.endInfo?.resultCode,
    };

    telemetryItem.data = {
      command: command,
    };

    if (extraProps) {
      telemetryItem.data.additionalData = extraProps;
    }

    // Populate common properties and fire event
    Telemetry.trackEvent(telemetryItem);
  }

  static trackException(error: Error, extraProps?: Record<string, any>) {
    if (!Telemetry.appInsightsCore) {
      return;
    }

    const telemetryItem: coreOneDS.ITelemetryItem = {name: CodedErrorEventName};

    // Save off CodedError info in Part C.
    const codedError =
      error instanceof errorUtils.CodedError
        ? (error as errorUtils.CodedError)
        : null;

    const codedErrorStruct = {
      type: codedError?.type ?? 'Unknown',
      data: codedError?.data ?? {},
    };

    // Copy msBuildErrorMessages into the codedError.data object
    if ((error as any).msBuildErrorMessages) {
      // Always grab MSBuild error codes if possible
      codedErrorStruct.data.msBuildErrors = (error as any).msBuildErrorMessages
        .map(errorUtils.tryGetErrorCode)
        .filter((msg: string | undefined) => msg);

      // Grab sanitized MSBuild error messages if we're preserving them
      if (Telemetry.options.preserveErrorMessages) {
        codedErrorStruct.data.msBuildErrorMessages = (
          error as any
        ).msBuildErrorMessages
          .map(errorUtils.sanitizeErrorMessage)
          .filter((msg: string) => msg);
      }
    }

    // Copy miscellaneous system error fields into the codedError.data object
    const syscallExceptionFieldsToCopy = ['errno', 'syscall', 'code'];
    for (const f of syscallExceptionFieldsToCopy) {
      if ((error as any)[f]) {
        codedErrorStruct.data.codedError.data[f] = (error as any)[f];
      }
    }

    // Break down TS Error object into Exception Data
    const exceptionData = Telemetry.convertErrorIntoExceptionData(error);

    telemetryItem.data = {
      codedError: codedErrorStruct,
      exceptionData: exceptionData,
    };

    Telemetry.trackEvent(telemetryItem);
  }

  static convertErrorIntoExceptionData(error: Error): Record<string, any> {
    const exceptionData = {
      hasFullStack: false,
      message: error.message,
      parsedStack: {},
    };

    exceptionData.message = exceptionData.message || '[None]';

    // CodedError has non-PII information in its 'type' member, plus optionally some more info in its 'data'.
    // The message may contain PII information. This can be sanitized, but for now delete it.
    if (Telemetry.options.preserveErrorMessages) {
      exceptionData.message = errorUtils.sanitizeErrorMessage(
        exceptionData.message,
      );
    } else {
      exceptionData.message = '[Removed]';
    }

    const lines = error.stack?.split('\n');

    const parsedStack = lines?.slice(1).map(line => {
      const errorStackFrame: errorUtils.ErrorStackFrame = {};

      const match = line
        .trim()
        .match(/^\s*at\s+(?:(.*?)\s+\((.*):(\d+):(\d+)\)|(.*):(\d+):(\d+))$/);
      if (match) {
        errorStackFrame.functionName = match[1] || 'N/A'; // Use a default value if no function name
        errorStackFrame.filePath = match[2] || match[5];
        errorStackFrame.lineNumber =
          parseInt(match[3], 10) || parseInt(match[6], 10);
        errorStackFrame.columnNumber =
          parseInt(match[4], 10) || parseInt(match[7], 10);
      }

      return errorStackFrame;
    });

    if (parsedStack) {
      parsedStack.filter(Boolean);

      // Sanitize parsed error stack frames
      for (const frame of parsedStack) {
        errorUtils.sanitizeErrorStackFrame(frame);
      }

      exceptionData.hasFullStack = true;
      exceptionData.parsedStack = parsedStack;
    }

    return exceptionData;
  }
}
