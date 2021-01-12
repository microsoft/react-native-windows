/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
import { BuildConfig, RunWindowsOptions } from '../runWindowsOptions';
import MSBuildTools from './msbuildtools';
import { Config } from '@react-native-community/cli-types';
export declare function getBuildConfiguration(options: RunWindowsOptions): BuildConfig;
export declare function deployToDevice(options: RunWindowsOptions, verbose: boolean): Promise<void>;
export declare function deployToDesktop(options: RunWindowsOptions, verbose: boolean, config: Config, buildTools: MSBuildTools): Promise<void>;
export declare function startServerInNewWindow(options: RunWindowsOptions, verbose: boolean): Promise<void>;
