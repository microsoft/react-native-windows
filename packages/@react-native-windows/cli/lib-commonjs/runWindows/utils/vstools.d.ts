/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
import { Project } from '../../config/projectConfig';
export declare const dotNetCoreProjectTypeGuid = "{9A19103F-16F7-4668-BE54-9A1E7A4F7556}";
/**
 * Adds the necessary info from a VS project into a VS solution file so that it will build.
 * @param slnFile The Absolute path to the target VS solution file.
 * @param project The object representing the project info.
 * @param verbose If true, enable verbose logging.
 * @param checkMode It true, don't make any changes.
 * @return Whether any changes were necessary.
 */
export declare function addProjectToSolution(slnFile: string, project: Project, verbose?: boolean, checkMode?: boolean): boolean;
