/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import * as coreOneDS from '@microsoft/1ds-core-js';

// Post 1DS seems to provide configuration settings to post events
// https://microsoft.github.io/ApplicationInsights-JS/webSdk/1ds-post-js/interfaces/IChannelConfiguration.html
// import * as postOneDS from '@microsoft/1ds-post-js';

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
const RNW_1DS_INSTURMENTATION_KEY = "49ff6d3ef12f4578a7b75a2573d9dba8-026332b2-2d50-452f-ad0d-50f921c97a9d-7145"

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

  // Store "Common Properties", which will be logged for all telemetry events.
  // Unlike AppInsights, 1DS does not provide an API to store such properties.
  protected static commonProperties: {[key: string]: string} = {};

  protected static getDefaultSetupString(): string {
    // Enable overriding the default setup string via an environment variable
    return process.env[ENV_SETUP_OVERRIDE] ?? RNW_1DS_INSTURMENTATION_KEY;
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
    if (!this.isTestEnvironment && basePropUtils.isCI() && !basePropUtils.captureCI()) {
      return;
    }

    // Save off options for later
    Object.assign(Telemetry.options, options);

    Telemetry.setupClient();

    await Telemetry.setupBaseProperties();

    // Setup telemetry initializers (formerly "processors" on App Insights)
    this.addTelemetryInitializers();
  }

  private static basicTelemetryInitializer(envelope: coreOneDS.ITelemetryItem) : boolean
  {
    if (envelope.tags) {
      delete envelope.tags['ai.cloud.roleInstance'];  
    }
    
    // Filter out "legacy" events from older stable branches
    const properties = envelope.data?.baseData?.properties;
    if (
      properties?.eventName &&
      EventNamesWeTrack.includes(properties.eventName)
    ) {
      return true;
    }

    return false;
  }

  private static errorTelemetryInitializer(envelope: coreOneDS.ITelemetryItem) : boolean
  {
    if (envelope.data?.baseType === 'ExceptionData') {
      const data = envelope.data.baseData;
      if (data?.exceptions) {
        for (const exception of data.exceptions) {
          for (const frame of exception.parsedStack) {
            errorUtils.sanitizeErrorStackFrame(frame);
          }

          // Exception message must never be blank, or AI will reject it
          exception.message = exception.message || '[None]';

          // CodedError has non-PII information in its 'type' member, plus optionally some more info in its 'data'.
          // The message may contain PII information. This can be sanitized, but for now delete it.
          // Note that the type of data.exceptions[0] is always going to be ExceptionDetails. It is not the original thrown exception.
          // https://github.com/microsoft/ApplicationInsights-node.js/issues/707
          if (Telemetry.options.preserveErrorMessages) {
            exception.message = errorUtils.sanitizeErrorMessage(
              exception.message,
            );
          } else {
            exception.message = '[Removed]';
          }
        }
      }
    }

    return true;
  }

  private static addTelemetryInitializers()
  {
    Telemetry.appInsightsCore?.addTelemetryInitializer(Telemetry.basicTelemetryInitializer);
    Telemetry.appInsightsCore?.addTelemetryInitializer(Telemetry.errorTelemetryInitializer);
  }

  /** Sets up Telemetry.appInsightsCore. */
  private static setupClient() {
    // appInsights.Configuration.setInternalLogging(false, false);

    const coreConfig: coreOneDS.IExtendedConfiguration = {
      instrumentationKey: process.env[ENV_SETUP_OVERRIDE] ?? RNW_1DS_INSTURMENTATION_KEY,
      extensions: [],
      extensionConfig: []
    }

    // Allow overriding the endpoint URL via an environment variable.
    if (process.env[ENV_PROXY_OVERRIDE] !== undefined) {
      coreConfig.endpointUrl = process.env[ENV_PROXY_OVERRIDE];
    }
  
    Telemetry.appInsightsCore = new coreOneDS.AppInsightsCore();
    Telemetry.appInsightsCore.initialize(coreConfig, [] /* extensions */);
  }

  /** Sets up any base properties that all telemetry events require. */
  private static async setupBaseProperties() {
    Telemetry.commonProperties.deviceId =
      await basePropUtils.deviceId();
    Telemetry.commonProperties.deviceArchitecture =
      basePropUtils.deviceArchitecture();
    Telemetry.commonProperties.nodeArchitecture =
      basePropUtils.nodeArchitecture();
    Telemetry.commonProperties.devicePlatform =
      basePropUtils.devicePlatform();
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
    Telemetry.commonProperties.sampleRate = basePropUtils
      .sampleRate()
      .toString();
    Telemetry.commonProperties.isTest = Telemetry.isTestEnvironment.toString();
    Telemetry.commonProperties.sessionId = Telemetry.getSessionId();

    // NOTE: Temporarily store sampleRate into CommonProperties
    //Telemetry.config.samplingPercentage = basePropUtils.sampleRate();
    Telemetry.commonProperties.samplingPercentage = basePropUtils.sampleRate().toString();

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
    telemetryItem.ver = "4.0";
    telemetryItem.time = new Date().toISOString();
    telemetryItem.iKey = RNW_1DS_INSTURMENTATION_KEY; // Is this needed?

    // Populate "common" properties into Part B, since most of them
    // don't adhere to the Part A extensions.
    telemetryItem.baseData = {
      common: {
        device: {
          id: Telemetry.commonProperties.deviceId,
          architecture: Telemetry.commonProperties.deviceArchitecture,
          platform: Telemetry.commonProperties.devicePlatform,
          locale: Telemetry.commonProperties.deviceLocale,
          numCPUs: Telemetry.commonProperties.numCPUs,
          totalMemory: Telemetry.commonProperties.totalMemory,
          diskFreeSpace: Telemetry.commonProperties.deviceDiskFreeSpace
        },
        nodeArchitecture: Telemetry.commonProperties.nodeArchitecture,
        ciCaptured: Telemetry.commonProperties.ciCaptured,
        ciType: Telemetry.commonProperties.ciType,
        isMsftInternal: Telemetry.commonProperties.isMsftInternal,
        sampleRate: Telemetry.commonProperties.sampleRate,
        isCliTest: Telemetry.commonProperties.isTest,
        sessionId: Telemetry.commonProperties.sessionId
      },
      // Set project and versions props, belonging to Part B.
      project: Telemetry.projectProp,
      versions: Telemetry.versionsProp
    };

    // Send and post the telemetry event!
    Telemetry.appInsightsCore!.track(telemetryItem);
    Telemetry.appInsightsCore!.flush();
  }

  private static trackCommandEvent(extraProps?: Record<string, any>) {
    var telemetryItem: coreOneDS.ITelemetryItem = {name: CommandEventName};

    // Set command data, which belongs to Part C.
    telemetryItem.data = {
      command: {
        options: Telemetry.commandInfo.startInfo?.options,
        defaultOptions: Telemetry.commandInfo.startInfo?.defaultOptions,
        args: Telemetry.commandInfo.startInfo?.args,
        durationInSecs:
          (Telemetry.commandInfo.endTime! - Telemetry.commandInfo.startTime!) /
          1000,
        resultCode: Telemetry.commandInfo.endInfo?.resultCode,
      },
    };

    // Set extra props to "additionalData", temporary name
    telemetryItem.data.additionalData = {};
    Object.assign(telemetryItem.data.additionalData, extraProps);
  
    // Fire event
    Telemetry.trackEvent(telemetryItem);
  }

  static trackException(error: Error, extraProps?: Record<string, any>) {
    if (!Telemetry.appInsightsCore) {
      return;
    }

    var telemetryItem: coreOneDS.ITelemetryItem = {name: CodedErrorEventName};

    // Save off CodedError info in Part C.
    const codedError =
      error instanceof errorUtils.CodedError
        ? (error as errorUtils.CodedError)
        : null;

    telemetryItem.data = {
      codedError: {
        type: codedError?.type ?? 'Unknown',
        data: codedError?.data ?? {},
      }
    };

    // Copy msBuildErrorMessages into the codedError.data object
    if ((error as any).msBuildErrorMessages) {
      // Always grab MSBuild error codes if possible
      telemetryItem.data.codedError.data.msBuildErrors = (error as any).msBuildErrorMessages
        .map(errorUtils.tryGetErrorCode)
        .filter((msg: string | undefined) => msg);

      // Grab sanitized MSBuild error messages if we're preserving them
      if (Telemetry.options.preserveErrorMessages) {
        telemetryItem.data.codedError.data.msBuildErrorMessages = (
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
        telemetryItem.data.codedError.data[f] = (error as any)[f];
      }
    }

    Telemetry.trackEvent(telemetryItem);
  }
}
