/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
/**
 * Search for files matching the pattern under the target folder.
 * @param folder The absolute path to target folder.
 * @param filenamePattern The pattern to search for.
 * @return  Return the array of relative file paths.
 */
export declare function findFiles(folder: string, filenamePattern: string): string[];
/**
 * Search for the windows sub-folder under the target folder.
 * @param folder The absolute path to the target folder.
 * @return The absolute path to the windows folder, if it exists.
 */
export declare function findWindowsFolder(folder: string): string | null;
/**
 * Checks if the target file path is a RNW solution file by checking if it contains the string "ReactNative".
 * @param filePath The absolute file path to check.
 * @return Whether the path is to a RNW solution file.
 */
export declare function isRnwSolution(filePath: string): boolean;
/**
 * Search for the RNW solution files under the target folder.
 * @param winFolder The absolute path to target folder.
 * @return Return the array of relative file paths.
 */
export declare function findSolutionFiles(winFolder: string): string[];
/**
 * Checks if the target file path is a RNW lib project file.
 * @param filePath The absolute file path to check.
 * @return Whether the path is to a RNW lib project file.
 */
export declare function isRnwDependencyProject(filePath: string): boolean;
/**
 * Search for the RNW lib project files under the target folder.
 * @param winFolder The absolute path to target folder.
 * @return Return the array of relative file paths.
 */
export declare function findDependencyProjectFiles(winFolder: string): string[];
/**
 * Search for the RNW app project files under the target folder.
 * @param winFolder The absolute path to target folder.
 * @return Return the array of relative file paths.
 */
export declare function findAppProjectFiles(winFolder: string): string[];
/**
 * Returns the programming language of the project file.
 * @param projectPath The project file path to check.
 * @return The language string: cpp, cs, or null if unknown.
 */
export declare function getProjectLanguage(projectPath: string): 'cpp' | 'cs' | null;
/**
 * Reads in the contents of the target project file.
 * @param projectPath The target project file path.
 * @return The project file contents.
 */
export declare function readProjectFile(projectPath: string): Document;
/**
 * Search for the given property in the project contents and return its value.
 * @param projectContents The XML project contents.
 * @param propertyName The property to look for.
 * @return The value of the tag if it exists.
 */
export declare function tryFindPropertyValue(projectContents: Node, propertyName: string): string | null;
export declare function findPropertyValue(projectContents: Node, propertyName: string, filePath: string): string;
/**
 * Search for the given import project in the project contents and return if it exists.
 * @param projectContents The XML project contents.
 * @param projectName The project to look for.
 * @return If the target exists.
 */
export declare function importProjectExists(projectContents: Node, projectName: string): boolean;
/**
 * Gets the name of the project from the project contents.
 * @param projectPath The project file path to check.
 * @param projectContents The XML project contents.
 * @return The project name.
 */
export declare function getProjectName(projectContents: Node): string;
/**
 * Gets the namespace of the project from the project contents.
 * @param projectContents The XML project contents.
 * @return The project namespace.
 */
export declare function getProjectNamespace(projectContents: Node): string | null;
/**
 * Gets the guid of the project from the project contents.
 * @param projectContents The XML project contents.
 * @return The project guid.
 */
export declare function getProjectGuid(projectContents: Node): string | null;
