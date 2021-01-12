/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
export interface Project {
    projectFile: string;
    projectName: string;
    projectLang: 'cpp' | 'cs' | null;
    projectGuid: string | null;
    projectTypeGuid?: string;
}
export interface WindowsProjectConfig {
    folder: string;
    sourceDir: string;
    solutionFile: string;
    project: Project;
}
/**
 * Gets the config of any RNW apps under the target folder.
 * @param folder The absolute path to the target folder.
 * @param userConfig A manually specified override config.
 * @return The config if any RNW apps exist.
 */
export declare function projectConfigWindows(folder: string, userConfig?: Partial<WindowsProjectConfig> | null): WindowsProjectConfig | null;
