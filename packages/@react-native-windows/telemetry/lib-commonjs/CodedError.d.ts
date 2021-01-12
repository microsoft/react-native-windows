/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
export declare const CodedErrors: {
    Success: number;
    UnsupportedReactNativeVersion: number;
    UserCancel: number;
    NoReactNativeFound: number;
    NoPackageJSon: number;
    NoLatestReactNativeWindows: number;
    NoAutoMatchingReactNativeWindows: number;
    IncompatibleOptions: number;
    NoReactNativeDependencies: number;
    NoMatchingPackageVersion: number;
    NoSolution: number;
    NoPropertyInProject: number;
    CopyProjectTemplateNoSourcePath: number;
    CopyProjectTemplateNoDestPath: number;
    CopyProjectTemplateNoProjectName: number;
    NoWindowsConfig: number;
    IncompleteConfig: number;
    InvalidConfig: number;
    NeedAutolinking: number;
    AddProjectToSolution: number;
    Autolinking: number;
    MinSDKVersionNotMet: number;
    BadSDKVersionFormat: number;
    NoSDK: number;
    NoMSBuild: number;
    NoVSWhere: number;
    MSBuildError: number;
    NoAppPackage: number;
    NoAppxManifest: number;
    NoDevice: number;
    AppDidNotDeploy: number;
    InvalidDevicesOutput: number;
    RemoveOldAppVersionFailure: number;
    EnableDevModeFailure: number;
    InstallAppFailure: number;
    InstallAppDependenciesFailure: number;
    CheckNetIsolationFailure: number;
    InstallAppToDeviceFailure: number;
    UninstallAppOnDeviceFailure: number;
    DeployRecipeFailure: number;
    AppStartupFailure: number;
    Unknown: number;
};
export declare type CodedErrorType = keyof typeof CodedErrors;
/**
 * Represents an error whose message might contain user-originating content,
 * therefore when transmitting telemetry, only the type should be sent.
 * @param type a stable ID identifying the type of error.
 * @param message the error text. This should only be used for display to the user.
 * @param data any additional metadata that is safe to collect for telemetry purposes.
 */
export declare class CodedError extends Error {
    readonly data?: Record<string, any> | undefined;
    constructor(type: CodedErrorType, message: string, data?: Record<string, any> | undefined);
}
