/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
import Version from './version';
import { BuildArch, BuildConfig } from '../runWindowsOptions';
export default class MSBuildTools {
    readonly version: string;
    readonly msbuildPath: string;
    readonly installationVersion: string;
    /**
     * @param version is something like 16.0 for 2019
     * @param msbuildPath  Path to MSBuild.exe (x86)
     * @param installationVersion is the full version e.g. 16.3.29411.108
     */
    constructor(version: string, msbuildPath: string, installationVersion: string);
    cleanProject(slnFile: string): void;
    buildProject(slnFile: string, buildType: BuildConfig, buildArch: BuildArch, msBuildProps: Record<string, string>, verbose: boolean, target: string | undefined, buildLogDirectory: string | undefined): Promise<void>;
    static findAvailableVersion(buildArch: BuildArch, verbose: boolean): MSBuildTools;
    static getAllAvailableUAPVersions(): Version[];
}
