/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
import MSBuildTools from './msbuildtools';
import { RunWindowsOptions, BuildConfig, BuildArch } from '../runWindowsOptions';
import { Config } from '@react-native-community/cli-types';
export declare function buildSolution(buildTools: MSBuildTools, slnFile: string, buildType: BuildConfig, buildArch: BuildArch, msBuildProps: Record<string, string>, verbose: boolean, target?: string, buildLogDirectory?: string): Promise<void>;
export declare function restoreNuGetPackages(slnFile: string, buildTools: MSBuildTools, verbose: boolean): Promise<void>;
export declare function getAppSolutionFile(options: RunWindowsOptions, config: Config): string | null;
export declare function getAppProjectFile(options: RunWindowsOptions, config: Config): string | null;
export declare function parseMsBuildProps(options: RunWindowsOptions): Record<string, string>;
