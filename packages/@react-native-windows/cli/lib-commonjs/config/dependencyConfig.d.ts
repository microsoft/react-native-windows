/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
export interface ProjectDependency {
    projectFile: string;
    directDependency: boolean;
    projectName: string;
    projectLang: 'cpp' | 'cs' | null;
    projectGuid: string | null;
    cppHeaders: string[];
    cppPackageProviders: string[];
    csNamespaces: string[];
    csPackageProviders: string[];
}
export interface NuGetPackageDependency {
    packageName: string;
    packageVersion: string;
    cppHeaders: string[];
    cppPackageProviders: string[];
    csNamespaces: string[];
    csPackageProviders: string[];
}
export interface WindowsDependencyConfig {
    folder: string;
    sourceDir?: string;
    solutionFile?: string | null;
    projects: ProjectDependency[];
    nugetPackages: NuGetPackageDependency[];
}
/**
 * Gets the config of any RNW native modules under the target folder.
 * @param userConfig A manually specified override config.
 * @return The config if any RNW native modules exist.
 */
export declare function dependencyConfigWindows(folder: string, userConfig?: Partial<WindowsDependencyConfig> | null): WindowsDependencyConfig | null;
