/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import * as appInsights from 'applicationinsights';

import * as sanitizeUtils from './sanitizeUtils';

// Note: All CLI commands will set process.exitCode to the numerical value of
// a thrown CodedError. However node reserves codes 1-192 as per:
// https://nodejs.org/api/process.html#process_exit_codes so we shouldn't
// override those as other tools may be monitoring the error code

export const CodedErrors = {
  Success: 0,
  Unknown: -1,

  // init
  UnsupportedReactNativeVersion: 1000,
  UserCancel: 1001,
  NoReactNativeFound: 1002,
  NoPackageJson: 1003,
  NoLatestReactNativeWindows: 1004,
  NoAutoMatchingReactNativeWindows: 1005,
  IncompatibleOptions: 1006,
  NoReactNativeDependencies: 1007,
  NoMatchingPackageVersion: 1008,

  // run-windows
  NoSolution: 2000,
  // Project generation
  NoPropertyInProject: 2100,
  CopyProjectTemplateNoSourcePath: 2101,
  CopyProjectTemplateNoDestPath: 2102,
  CopyProjectTemplateNoProjectName: 2103,
  // SDK requirements
  MinSDKVersionNotMet: 2200,
  BadSDKVersionFormat: 2201,
  NoSDK: 2202,
  // Build
  NoMSBuild: 2300,
  NoVSWhere: 2301,
  MSBuildError: 2302,
  // Deploy
  NoAppPackage: 2400,
  NoAppxManifest: 2401,
  NoDevice: 2402,
  AppDidNotDeploy: 2403,
  InvalidDevicesOutput: 2404,
  RemoveOldAppVersionFailure: 2405,
  EnableDevModeFailure: 2406,
  InstallAppFailure: 2407,
  InstallAppDependenciesFailure: 2408,
  CheckNetIsolationFailure: 2409,
  InstallAppToDeviceFailure: 2410,
  UninstallAppOnDeviceFailure: 2411,
  DeployRecipeFailure: 2412,
  // Launch
  AppStartupFailure: 2500,

  // autolink-windows
  NoWindowsConfig: 3000,
  IncompleteConfig: 3001,
  InvalidConfig: 3002,
  NeedAutolinking: 3003,
  AddProjectToSolution: 3004,
  Autolinking: 3005,
};

export type CodedErrorType = keyof typeof CodedErrors;

/**
 * Represents an error whose message might contain user-originating content,
 * therefore when transmitting telemetry, only the type should be sent.
 * @param type a stable ID identifying the type of error.
 * @param message the error text. This should only be used for display to the user.
 * @param data any additional metadata that is safe to collect for telemetry purposes.
 */
export class CodedError extends Error {
  constructor(
    public readonly type: CodedErrorType,
    message: string,
    public readonly data?: Record<string, any>,
  ) {
    super(message);
    this.name = type;
  }
}

/**
 * Tries to parse an error code out of an error message.
 * @param msg An error message to process.
 * @returns The parsed error code.
 */
export function tryGetErrorCode(msg: string): string | undefined {
  const errorRegEx = /error (\w+\d+):/gi;
  const m = errorRegEx.exec(msg);
  return m ? m[1] : undefined;
}

/**
 * Sanitize an error message by anonymizing any paths that appear between quotes (''), brackets ([]), or double quotes ("").
 * @param msg The error message to sanitize.
 * @return The message with any paths anonymized.
 */
export function sanitizeErrorMessage(msg: string): string {
  const cpuThreadId = /^\d+(:\d+)?>/g;
  msg = msg.replace(cpuThreadId, '');
  const parts = msg.split(/['[\]"]/g);
  const clean = [];
  const pathRegEx = /(['["]?)([A-Za-z]:|\\)[\\/]([^<>:;,?"*\t\r\n|/\\]+[\\/])+([^<>:;,?"*\t\r\n|]+\/?(['["]?))/gi;
  for (const part of parts) {
    if (pathRegEx.test(part)) {
      pathRegEx.lastIndex = -1;
      let matches: RegExpExecArray | null;
      let noPath = '';
      let last = 0;
      while ((matches = pathRegEx.exec(part))) {
        noPath +=
          part.substr(last, matches!.index - last) +
          sanitizeUtils.getAnonymizedPath(matches[0]);
        last = matches!.index + matches![0].length;
      }
      if (noPath !== '') {
        clean.push(noPath);
      }
    } else if (part !== '') {
      clean.push(part);
    }
  }
  return clean.join('').trim();
}

/**
 * Sanitizes an error stack frame.
 * @param frame
 */
export function sanitizeErrorStackFrame(
  frame: appInsights.Contracts.StackFrame,
): void {
  const parens = frame.method.indexOf('(');
  if (parens !== -1) {
    // case 1: method === 'methodName (rootOfThePath'
    frame.method = frame.method.substr(0, parens).trim();
  } else {
    // case 2: method === <no_method> or something without '(', fileName is full path
  }
  // anonymize the filename
  frame.fileName = sanitizeUtils.getAnonymizedPath(frame.fileName);
  frame.assembly = '';
}
