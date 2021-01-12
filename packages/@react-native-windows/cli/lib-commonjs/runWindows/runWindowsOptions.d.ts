/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
import { CommandOption } from '@react-native-community/cli-types';
export declare type BuildArch = 'x86' | 'x64' | 'ARM' | 'ARM64';
export declare type BuildConfig = 'Debug' | 'DebugBundle' | 'Release' | 'ReleaseBundle';
/**
 *  * Options are the following:
 *    release: Boolean - Specifies release build
 *    root: String - The root of the application
 *    arch: String - The build architecture (x86, x64, ARM, Any CPU)
 *    emulator: Boolean - Deploy to the emulator
 *    device: Boolean - Deploy to a device
 *    target: String - Device GUID to deploy to
 *    remote-debugging: Boolean - Run using remote JS proxy
 *    logging: Boolean - Enables logging
 *    no-packager: Boolean - Do not launch packager while building
 *    bundle: Boolean - Enable Bundle configuration.
 *    no-launch: Boolean - Do not launch the app after deployment
 *    no-build: Boolean - Do not build the solution
 *    no-deploy: Boolean - Do not deploy the app
 *    sln: String - Solution file to build
 *    msbuildprops: String - Comma separated props to pass to msbuild, eg: prop1=value1,prop2=value2
 *    direct-debugging: Number - Enable direct debugging on specified port
 *    no-telemetry: Boolean - Disables sending telemetry that allows analysis of usage and failures of the react-native-windows CLI
 */
export interface RunWindowsOptions {
    release?: boolean;
    root: string;
    arch: BuildArch;
    emulator?: boolean;
    device?: boolean;
    target?: string;
    remoteDebugging?: string;
    logging: boolean;
    packager: boolean;
    bundle: boolean;
    launch: boolean;
    autolink: boolean;
    build: boolean;
    deploy: boolean;
    sln?: string;
    proj?: string;
    msbuildprops?: string;
    buildLogDirectory?: string;
    info: boolean;
    directDebugging?: number;
    telemetry?: boolean;
}
export declare const runWindowsOptions: CommandOption[];
